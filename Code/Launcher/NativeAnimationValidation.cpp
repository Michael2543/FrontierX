#include "NativeAnimationValidation.h"

#include "ICamera.h"
#include "ENGINE.H"
#include "IAnim.h"
#include "IHardPoint.h"
#include "FrontierX/LegacyFileTypes.h"
#include "FrontierX/NativeMeshManager.h"
#include "IMaterialManager.h"
#include "IMeshManager.h"
#include "renderer.h"
#include "rendpipeline.h"
#include "SYSTEM.H"
#include "TManager.h"
#include "Filesys.h"

#include <cmath>

namespace FrontierX
{
namespace
{
// Original Engine stores the compound root part first, and enumeration
// includes it. An instance's archetype is that part, not the outer CMP ID.
bool CaptureFirstPart(ARCHETYPE_INDEX, ARCHETYPE_INDEX child, void *context)
{
    *static_cast<ARCHETYPE_INDEX *>(context) = child;
    return false;
}

template <class Interface> struct OwnedInterface
{
    Interface * value = nullptr;
    ~OwnedInterface() { if (value) value->Release(); }
    Interface ** receive() { return &value; }
};
}

AnimationValidationResult ValidateAnimationObject(
    IEngine * engine, IFileSystem * objectRoot,
    const char * objectName, const char * scriptName)
{
    AnimationValidationResult result;
    if (!engine || !objectRoot || !objectName || !scriptName) return result;

    IFileSystem * objectFile = nullptr;
    IAnimation * animation = nullptr;
    IAnimation2 * animation2 = nullptr;
    DAFILEDESC descriptor(objectName);
    if (objectRoot->CreateInstance(&descriptor,
            reinterpret_cast<void **>(&objectFile)) != GR_OK || !objectFile ||
        engine->QueryInterface(IID_IAnimation,
            reinterpret_cast<void **>(&animation)) != GR_OK || !animation ||
        engine->QueryInterface(IID_IAnimation2,
            reinterpret_cast<void **>(&animation2)) != GR_OK || !animation2)
    {
        if (animation2) animation2->Release();
        if (animation) animation->Release();
        if (objectFile) objectFile->Release();
        return result;
    }

    const SCRIPT_SET_ARCH scripts = animation->create_script_set_arch(objectFile);
    if (scripts == INVALID_SCRIPT_SET_ARCH)
        result.status = AnimationValidationStatus::LoadFailed;
    else
    {
        const GENRESULT counted = animation2->script_count(scripts, result.scripts);
        result.duration = animation->get_duration(scripts, scriptName);
        result.status = counted == GR_OK && result.scripts != 0 &&
            std::isfinite(result.duration) && result.duration >= 0.0f ?
            AnimationValidationStatus::Ok : AnimationValidationStatus::ScriptFailed;
        animation->release_script_set_arch(scripts);
    }
    animation2->Release();
    animation->Release();
    objectFile->Release();
    return result;
}

bool ValidateMeshObject(ISystemContainer * system, IEngine * engine,
    IFileSystem * objectRoot, IFileSystem * materialRoot,
    const char * objectName, unsigned int & faceGroups)
{
    faceGroups = 0;
    if (!system || !engine || !objectRoot || !materialRoot || !objectName)
        return false;

    OwnedInterface<IRenderer> renderer;
    OwnedInterface<IAnimation> animation;
    OwnedInterface<IHardpoint> hardpoint;
    OwnedInterface<IRenderPipeline> pipeline;
    OwnedInterface<IMaterialManager> materials;
    OwnedInterface<ITManager> textures;
    OwnedInterface<IMeshManager> meshes;
    if (engine->QueryInterface(IID_IRenderer,
            reinterpret_cast<void **>(renderer.receive())) != GR_OK ||
        engine->QueryInterface(IID_IAnimation,
            reinterpret_cast<void **>(animation.receive())) != GR_OK ||
        engine->QueryInterface(IID_IHardpoint,
            reinterpret_cast<void **>(hardpoint.receive())) != GR_OK ||
        system->QueryInterface(IID_IRenderPipeline,
            reinterpret_cast<void **>(pipeline.receive())) != GR_OK ||
        system->QueryInterface(IID_IMaterialManager,
            reinterpret_cast<void **>(materials.receive())) != GR_OK ||
        system->QueryInterface(IID_ITManager,
            reinterpret_cast<void **>(textures.receive())) != GR_OK ||
        system->QueryInterface(IID_IMeshManager,
            reinterpret_cast<void **>(meshes.receive())) != GR_OK)
        return false;

    ITManager::InitInfo textureInfo{pipeline.value};
    textures.value->Initialize(textureInfo);
    IMaterialManager::InitInfo materialInfo{};
    materialInfo.MATDIR = materialRoot;
    materialInfo.PIPE = pipeline.value;
    materialInfo.TMANAGER = textures.value;
    if (materials.value->Initialize(materialInfo) != GR_OK)
        return false;

    IMeshManager::InitInfo meshInfo{};
    meshInfo.ENGINE = engine;
    meshInfo.ANIM = animation.value;
    meshInfo.OBJDIR = objectRoot;
    meshInfo.REND = renderer.value;
    meshInfo.PIPE = pipeline.value;
    meshInfo.HARDPOINT = hardpoint.value;
    meshInfo.MATMAN = materials.value;
    // The App camera is needed for instances, not archetype construction.
    meshes.value->InitManager(meshInfo);
    IMeshArchetype * arch = meshes.value->CreateMeshArch(objectName);
    if (!arch) return false;
    bool valid = arch->GetEngineArchtype() != INVALID_ARCHETYPE_INDEX &&
        arch->GetNumFaceGroups() != 0;
    if (valid)
    {
        valid = engine->get_root_instance_next() == INVALID_INSTANCE_INDEX;
        IMeshInstance * instance = meshes.value->CreateMesh(arch);
        if (instance)
        {
            const INSTANCE_INDEX index = GetNativeMeshInstanceIndex(instance);
            const ARCHETYPE_INDEX instanceArch = index == INVALID_INSTANCE_INDEX ?
                INVALID_ARCHETYPE_INDEX : engine->get_instance_archetype(index);
            const bool compound = engine->is_archetype_compound(arch->GetEngineArchtype());
            ARCHETYPE_INDEX expected = arch->GetEngineArchtype();
            if (compound)
            {
                expected = INVALID_ARCHETYPE_INDEX;
                valid = valid && engine->enumerate_archetype_parts(
                    arch->GetEngineArchtype(), CaptureFirstPart, &expected) == GR_OK;
            }
            valid = valid && expected != INVALID_ARCHETYPE_INDEX && instanceArch == expected;
            if (instanceArch != INVALID_ARCHETYPE_INDEX)
                engine->release_archetype(instanceArch);
            // Original Engine root enumeration tracks compounds only. A simple
            // mesh must still have its own real instance and matching archetype.
            const INSTANCE_INDEX root = engine->get_root_instance_next();
            valid = valid && (compound ?
                root == index : root == INVALID_INSTANCE_INDEX);
            meshes.value->DestroyMesh(instance);
            const ARCHETYPE_INDEX remaining = index == INVALID_INSTANCE_INDEX ?
                INVALID_ARCHETYPE_INDEX : engine->get_instance_archetype(index);
            valid = valid && remaining == INVALID_ARCHETYPE_INDEX &&
                engine->get_root_instance_next() == INVALID_INSTANCE_INDEX;
            if (remaining != INVALID_ARCHETYPE_INDEX)
                engine->release_archetype(remaining);
            if (valid) faceGroups = arch->GetNumFaceGroups();
        }
        else valid = false;
    }
    arch->Release();
    return valid;
}
}
