#include "DACOM.H"
#include "FDump.h"
#include "HeapObj.h"
#include "ILight.h"
#include "3dmath.h"
#include "lightman.h"
#include "FrontierX/NativeMathEngine.h"

#include <cstring>
#include <iostream>

bool RegisterNativeLightManager(ICOManager * manager);

namespace
{
int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...)
{
    return 0;
}

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "light manager probe failed: " << message << '\n';
    return condition;
}

class TestLight final : public ILight
{
public:
    GENRESULT COMAPI QueryInterface(const C8 * name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (name && (std::strcmp(name, IID_ILight) == 0 ||
                     std::strcmp(name, IID_IDAComponent) == 0))
        {
            *instance = static_cast<ILight *>(this);
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 COMAPI AddRef() override { return ++refs; }
    U32 COMAPI Release() override { return --refs; }

    GENRESULT COMAPI GetTransform(Transform & transform) const override
    {
        transform.set_identity();
        return GR_OK;
    }
    GENRESULT COMAPI GetPosition(Vector & position) const override
    {
        position = Vector(0.0f, 0.0f, 5.0f);
        return GR_OK;
    }
    GENRESULT COMAPI GetColor(LightRGB & color) const override
    {
        color = LightRGB{100, 80, 60};
        return GR_OK;
    }
    GENRESULT COMAPI GetDirection(Vector & direction) const override
    {
        direction = Vector(0.0f, 0.0f, -1.0f);
        return GR_OK;
    }
    SINGLE COMAPI GetRange() const override { return 10.0f; }
    BOOL32 COMAPI IsInfinite() const override { return FALSE; }
    SINGLE COMAPI GetCutoff() const override { return 45.0f; }
    U32 COMAPI GetMap() const override { return 0; }

    U32 refs = 1;
};

struct FactoryCapture
{
    IComponentFactory * factory = nullptr;
};

BOOL32 DACOM_CDECL CaptureFactory(IComponentFactory * factory, const C8 *,
                                  U32, void * context)
{
    static_cast<FactoryCapture *>(context)->factory = factory;
    return FALSE;
}
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

int main()
{
    if (!Expect(InitializeDAHeap(1024 * 1024, 0, DAHEAPFLAG_NOMSGS) == TRUE,
                "native heap initialization")) return 1;
    ICOManager * manager = DACOM_Acquire();
    if (!Expect(manager && FrontierX::RegisterNativeMathEngine(manager) &&
                RegisterNativeLightManager(manager),
                "production math and light factory registration")) return 1;
    AGGDESC descriptor("LightManager");
    ILightManager * lights = nullptr;
    if (!Expect(manager->CreateInstance(&descriptor,
                    reinterpret_cast<void **>(&lights)) == GR_OK && lights,
                "production ILightManager aggregate creation")) return 1;
    IAggregateComponent * aggregate = nullptr;
    if (!Expect(lights->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&aggregate)) == GR_OK &&
                aggregate && aggregate->Initialize() == GR_OK,
                "production light aggregate initialization")) return 1;

    lights->set_ambient_light(10, 20, 30);
    Vector vertex(0.0f, 0.0f, 0.0f);
    Vector normal(0.0f, 0.0f, 1.0f);
    LightRGB color;
    lights->light_vertices(&color, &vertex, &normal, 1);
    if (!Expect(color.r == 10 && color.g == 20 && color.b == 30,
                "ambient-only vertex light")) return 1;

    TestLight light;
    lights->register_light(&light);
    ILight * registered = nullptr;
    if (!Expect(lights->get_registered_lights(&registered) == 1 &&
                registered == &light,
                "generated light appears in production registry")) return 1;
    lights->update_lighting(nullptr);
    ILight * active = &light;
    lights->activate_lights(&active, 1);
    lights->light_vertices(&color, &vertex, &normal, 1);
    if (!Expect(color.r > 10 && color.g > 20 && color.b > 30 &&
                color.r <= 255 && color.g <= 255 && color.b <= 255,
                "production lit vertex executes inverse-square-root path within bounds")) return 1;
    const U32 flippedNormalIndex = 0x80000000U;
    LightRGB flipped;
    lights->light_vertices_strided(&flipped, sizeof(flipped), &vertex,
        sizeof(vertex), &normal, sizeof(normal), &flippedNormalIndex,
        sizeof(flippedNormalIndex), 1);
    if (!Expect(flipped.r == 10 && flipped.g == 20 && flipped.b == 30,
                "strided single vertex masks flipped normal index")) return 1;
    LightRGB_U8 color8;
    lights->light_vertices_U8(&color8, &vertex, &normal, 1);
    if (!Expect(color8.r > 10 && color8.g > 20 && color8.b > 30,
                "production App U8 light path")) return 1;
    lights->deactivate_all_lights();
    lights->light_vertices(&color, &vertex, &normal, 1);
    if (!Expect(color.r == 10 && color.g == 20 && color.b == 30,
                "ambient restored after deactivation")) return 1;
    lights->unregister_light(&light);
    if (!Expect(lights->get_registered_lights(nullptr) == 0,
                "production registry releases generated light")) return 1;
    aggregate->Release();
    lights->Release();

    FactoryCapture capture;
    if (!Expect(manager->EnumerateComponents("LightManager", CaptureFactory,
                    &capture) == GR_OK && capture.factory &&
                manager->UnregisterComponent(capture.factory,
                    "LightManager") == GR_OK,
                "production light factory unregister")) return 1;
    if (!Expect(HEAP->GetAvailableMemory() == HEAP->GetHeapSize(),
                "production light heap balance")) return 1;
    std::cout << "production light manager shaded and released generated light\n";
    return 0;
}
