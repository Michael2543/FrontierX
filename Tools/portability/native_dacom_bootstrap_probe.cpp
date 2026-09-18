#include "DACOM.H"
#include "IProfileParser.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace
{
struct Enumeration
{
    U32 profile = 0;
    U32 profile2 = 0;
};

BOOL32 DACOM_CDECL CountFactory(
    IComponentFactory *, const C8 * interface_name, U32, void * context)
{
    auto & enumeration = *static_cast<Enumeration *>(context);
    if (std::strcmp(interface_name, "IProfileParser") == 0) ++enumeration.profile;
    if (std::strcmp(interface_name, "IProfileParser2") == 0) ++enumeration.profile2;
    return TRUE;
}

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}
}

int main()
{
    try
    {
        ICOManager * first = DACOM_Acquire();
        ICOManager * second = DACOM_Acquire();
        Require(first && first == second,
            "DACOM_Acquire did not return one process-global manager");

        void * queried = nullptr;
        Require(first->QueryInterface("ICOManager", &queried) == GR_OK && queried == first,
            "native global manager query failed");
        static_cast<ICOManager *>(queried)->Release();

        Enumeration enumeration;
        Require(first->EnumerateComponents(nullptr, CountFactory, &enumeration) == GR_OK &&
                enumeration.profile == 1 && enumeration.profile2 == 1,
            "native global manager did not register both production profile factories once");

        const char profile_text[] = "[FrontierX]\nDataRoot=Data\n";
        PROFPARSEDESC2 descriptor;
        void * raw_aggregate = nullptr;
        Require(first->CreateInstance(&descriptor, &raw_aggregate) == GR_OK && raw_aggregate,
            "global manager could not create the production profile parser");
        auto * aggregate = static_cast<IAggregateComponent *>(raw_aggregate);
        IProfileParser2 * parser = nullptr;
        Require(aggregate->QueryInterface(IID_IProfileParser2,
                    reinterpret_cast<void **>(&parser)) == GR_OK && parser,
            "production profile-parser query through global manager failed");
        Require(parser->Initialize2(
                    profile_text, static_cast<U32>(std::strlen(profile_text))) == GR_OK,
            "production profile-parser buffer initialization failed");
        HANDLE section = parser->CreateSection("FrontierX");
        char value[16]{};
        Require(section != 0 && parser->ReadKeyValue(
                    section, "DataRoot", value, sizeof(value)) == 4 &&
                std::strcmp(value, "Data") == 0,
            "production profile parser created by the global manager changed behavior");
        parser->CloseSection(section);
        parser->Release();
        aggregate->Release();

        std::cout << "native global DACOM bootstrap passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native DACOM bootstrap probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
