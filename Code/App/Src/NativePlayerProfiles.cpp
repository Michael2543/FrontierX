#include "Include/NativePlayerProfiles.h"
#include "Include/NativeAppGlobals.h"
#include "UserDefaults.h"
#include <FrontierX/NativeUserDefaults.h>
#include <Filesys.h>
#include <TSmartPointer.h>
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace FrontierX
{
namespace
{
IFileSystem *saveRoot = nullptr; // Borrowed from NativeAppSaveBinding.
COMPTR<IFileSystem> selectedDirectory;
constexpr char LegacyKey[] = "NativeLegacyPlayerName";

bool ValidName(const char *name)
{
    if (!name || !*name || std::strlen(name) >= 32 || std::strpbrk(name, "/\\.:*?\"<>|")) return false;
    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(name); *p; ++p)
        if (*p < 32 || *p >= 127) return false;
    return true;
}
std::string LegacyName()
{
    char name[64]{};
    if (DEFAULTS) DEFAULTS->GetStringFromRegistry(LegacyKey, name, sizeof(name));
    return name;
}
bool OpenPlayers(COMPTR<IFileSystem> &directory)
{
    if (!saveRoot) return false;
    if (saveRoot->GetFileAttributes("Players") != FILE_ATTRIBUTE_DIRECTORY &&
        !saveRoot->CreateDirectory("Players")) return false;
    DAFILEDESC file("Players");
    return saveRoot->CreateInstance(&file, directory) == GR_OK && directory;
}
}
void BindNativePlayerSaveRoot(IFileSystem *root)
{
    selectedDirectory.free();
    saveRoot = root;
    SAVEDIR = root;
}
bool IsNativeLegacyPlayerProfile(const char *name)
{
    return name && LegacyName() == name;
}
std::vector<std::string> NativePlayerProfileNames()
{
    std::vector<std::string> names;
    const auto legacy = LegacyName();
    if (!legacy.empty()) names.push_back(legacy);
    COMPTR<IFileSystem> players;
    if (OpenPlayers(players))
    {
        WIN32_FIND_DATA data{};
        const HANDLE search = players->FindFirstFile("*", &data);
        if (search != INVALID_HANDLE_VALUE)
        {
            do {
                if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ValidName(data.cFileName))
                    names.emplace_back(data.cFileName);
            } while (players->FindNextFile(search, &data));
            players->FindClose(search);
        }
    }
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}
bool CreateNativePlayerProfile(const char *name)
{
    if (!saveRoot || !DEFAULTS || !ValidName(name)) return false;
    if (LegacyName().empty()) return DEFAULTS->SetStringInRegistry(LegacyKey, name) != 0;
    if (IsNativeLegacyPlayerProfile(name)) return true;
    COMPTR<IFileSystem> players;
    return OpenPlayers(players) && players->CreateDirectory(name);
}
bool SelectNativePlayerProfile(const char *name)
{
    if (!saveRoot || !DEFAULTS || !ValidName(name)) return false;
    if (LegacyName().empty() && !CreateNativePlayerProfile(name)) return false;
    COMPTR<IFileSystem> selected;
    if (!IsNativeLegacyPlayerProfile(name))
    {
        COMPTR<IFileSystem> players;
        DAFILEDESC file(name);
        if (!OpenPlayers(players) || players->CreateInstance(&file, selected) != GR_OK || !selected) return false;
    }
    if (!DEFAULTS->SetStringInRegistry("CQPlayerName", name)) return false;
    selectedDirectory = selected;
    SAVEDIR = selectedDirectory ? static_cast<IFileSystem *>(selectedDirectory) : saveRoot;
    return true;
}
bool RenameNativePlayerProfile(const char *previous, const char *name)
{
    if (!ValidName(previous) || !ValidName(name)) return false;
    const auto names = NativePlayerProfileNames();
    if (std::find(names.begin(), names.end(), name) != names.end()) return false;
    if (IsNativeLegacyPlayerProfile(previous))
    {
        if (!RenameNativePlayerDefaults(DEFAULTS,previous,name)) return false;
        if (DEFAULTS->SetStringInRegistry(LegacyKey,name)) return true;
        if (!RenameNativePlayerDefaults(DEFAULTS,name,previous))
            throw std::runtime_error("Native legacy profile settings rollback failed");
        return false;
    }
    COMPTR<IFileSystem> players;
    if (!OpenPlayers(players) || !players->MoveFile(previous,name)) return false;
    if (RenameNativePlayerDefaults(DEFAULTS,previous,name)) return true;
    if (!players->MoveFile(name,previous)) throw std::runtime_error("Native profile rename rollback failed");
    return false;
}
bool DeleteNativePlayerProfile(const char *name)
{
    // The imported flat root also owns runtime files and other profiles.
    // Retain it as the migration anchor instead of recursively deleting it.
    if (!ValidName(name) || IsNativeLegacyPlayerProfile(name)) return false;
    COMPTR<IFileSystem> players, profile;
    DAFILEDESC file(name);
    if (!OpenPlayers(players) || players->CreateInstance(&file, profile) != GR_OK || !profile) return false;
    if (!RecursiveDelete(profile)) return false;
    profile.free();
    return players->RemoveDirectory(name) != 0;
}
}
