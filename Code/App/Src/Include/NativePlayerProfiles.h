#ifndef FRONTIERX_NATIVE_PLAYER_PROFILES_H
#define FRONTIERX_NATIVE_PLAYER_PROFILES_H

#if !defined(_WIN32)
#include <string>
#include <vector>
struct IFileSystem;
namespace FrontierX
{
void BindNativePlayerSaveRoot(IFileSystem *root);
std::vector<std::string> NativePlayerProfileNames();
bool CreateNativePlayerProfile(const char *name);
bool SelectNativePlayerProfile(const char *name);
bool RenameNativePlayerProfile(const char *previous, const char *name);
bool DeleteNativePlayerProfile(const char *name);
bool IsNativeLegacyPlayerProfile(const char *name);
}
#endif
#endif
