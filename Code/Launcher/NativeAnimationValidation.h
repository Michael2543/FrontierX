#ifndef FRONTIERX_NATIVE_ANIMATION_VALIDATION_H
#define FRONTIERX_NATIVE_ANIMATION_VALIDATION_H

struct IEngine;
struct IFileSystem;
struct ISystemContainer;

namespace FrontierX
{
enum class AnimationValidationStatus
{
    Ok,
    OpenFailed,
    LoadFailed,
    ScriptFailed
};

struct AnimationValidationResult
{
    AnimationValidationStatus status = AnimationValidationStatus::OpenFailed;
    unsigned int scripts = 0;
    float duration = 0.0f;
};

AnimationValidationResult ValidateAnimationObject(
    IEngine * engine, IFileSystem * objectRoot,
    const char * objectName, const char * scriptName);

bool ValidateMeshObject(ISystemContainer * system, IEngine * engine,
    IFileSystem * objectRoot, IFileSystem * materialRoot,
    const char * objectName, unsigned int & faceGroups);
}

#endif
