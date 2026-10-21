#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeUserDefaults.h"
#include "FrontierX/LegacyPreferenceValues.h"
#include "FrontierX/StartupPreferences.h"
#include "UserDefaults.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
class TemporaryDirectory
{
public:
    TemporaryDirectory()
    {
        path_ = std::filesystem::current_path() / "frontierx_native_user_defaults_probe_data";
        std::error_code error;
        std::filesystem::remove_all(path_, error);
        if (!std::filesystem::create_directory(path_, error) || error)
            throw std::runtime_error("could not create temporary settings directory");
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path & Path() const noexcept { return path_; }

private:
    std::filesystem::path path_;
};

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

IUserDefaults * CreateDefaults(
    FrontierX::NativeComponentManager & manager,
    const std::filesystem::path & path,
    const FrontierX::StartupPreferences & preferences)
{
    FrontierX::RegisterNativeUserDefaults(&manager, path, preferences);
    DACOMDESC descriptor("IUserDefaults");
    void * raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK && raw,
        "native IUserDefaults creation failed");
    return static_cast<IUserDefaults *>(raw);
}
}

int main()
{
    try
    {
        TemporaryDirectory temporary;
        FrontierX::StartupPreferences startup;
        std::string error;
        Require(FrontierX::LoadStartupPreferences(
                    {}, temporary.Path(), {}, startup, &error),
            error.c_str());
        const std::filesystem::path settings_path = startup.selection.path;
        Require(startup.storeResult == FrontierX::PreferenceLoadResult::NotFound,
            "first-run startup preferences did not begin missing");

        FrontierX::NativeComponentManager manager;
        IUserDefaults * defaults = CreateDefaults(manager, settings_path, startup);
        USER_DEFAULTS * live = IUserDefaults::GetDefaults();
        Require(live && live->dwSize == sizeof(USER_DEFAULTS) && live->dwVersion == 11 &&
                live->bHardwareRender && live->bHardwareCursor &&
                live->bSectormapRotates && live->bRightClickOption,
            "native live defaults did not expose source-reviewed initial values");

        const std::array<unsigned char, 4> binary{{0x12, 0x34, 0x56, 0x78}};
        Require(defaults->SetDataInRegistry(
                    "ProbeBinary", const_cast<unsigned char *>(binary.data()), binary.size()) == TRUE,
            "native named binary write failed");
        std::array<unsigned char, 4> binary_output{};
        Require(defaults->GetDataFromRegistry(
                    "ProbeBinary", binary_output.data(), binary_output.size()) == binary.size() &&
                binary_output == binary,
            "native named binary read failed");
        Require(defaults->GetDataFromRegistry("ProbeBinary", binary_output.data(), 3) ==
                FrontierX::LegacyPreferenceValues::BufferTooSmall,
            "native named binary short-buffer result changed");

        Require(defaults->SetStringInRegistry("CQPlayerName", "Admiral") == 1,
            "native player-name write failed");
        std::array<char, 64> text{};
        Require(defaults->GetStringFromRegistry(
                    "CQPlayerName", text.data(), text.size()) == 8 &&
                std::strcmp(text.data(), "Admiral") == 0,
            "native player-name read failed");
        Require(defaults->SetStringInRegistry("CQPlayerName", nullptr) == 1 &&
                defaults->GetStringFromRegistry(
                    "CQPlayerName", text.data(), text.size()) == 0,
            "native named-string deletion failed");
        Require(defaults->SetStringInRegistry("CQPlayerName", "Admiral") == 1,
            "native player-name restoration failed");

        Require(defaults->SetStringInRegistry("maps", 77) == TRUE,
            "native numeric directory write failed");
        std::array<char, 256> directory{};
        Require(defaults->GetStringFromRegistry(directory.data(), 77) == TRUE &&
                std::strcmp(directory.data(), "maps") == 0,
            "native numeric directory read failed");
        Require(defaults->InsertNameIntoMRU("alpha.cqm", 77) == TRUE &&
                defaults->InsertNameIntoMRU("beta.cqm", 77) == TRUE &&
                defaults->FindNameInMRU("beta.cqm", 77) == 0 &&
                defaults->GetNameInMRU(text.data(), 77, 1) == TRUE &&
                std::strcmp(text.data(), "alpha.cqm") == 0,
            "native MRU insertion or lookup failed");
        Require(defaults->RemoveNameFromMRU("beta.cqm", 77) == FALSE &&
                defaults->FindNameInMRU("beta.cqm", 77) == -1,
            "native historical MRU removal contract changed");

        live->iMainX = -27;
        live->iMainWidth = 1280;
        live->scrollRate = 2.5F;
        live->fogMode = FOGOWAR_NONE;
        live->bWindowMode = 1;
        live->bSubtitles = 1;
        live->maxProjectiles = 73;
        Require(defaults->StoreDefaults() == TRUE && std::filesystem::is_regular_file(settings_path),
            "native defaults store did not create the selected settings file");

        std::array<char, 8> dialog{{'x'}};
        Require(defaults->GetInputFilename(dialog.data(), 1) == FALSE && dialog[0] == 0 &&
                defaults->GetOutputFilename(dialog.data(), 1) == FALSE &&
                defaults->GetUserData(nullptr, nullptr, nullptr, 0) == FALSE &&
                defaults->GetScriptData(nullptr, nullptr, nullptr, 0) == FALSE &&
                defaults->UbiEula() == FALSE,
            "unsupported native UI or EULA operation was silently accepted");

        defaults->Release();
        Require(IUserDefaults::GetDefaults() == nullptr,
            "native live defaults pointer survived component destruction");
        Require(manager.ShutDown() == GR_OK, "first native defaults manager shutdown failed");

        FrontierX::StartupPreferences reloaded;
        Require(FrontierX::LoadStartupPreferences(
                    settings_path, {}, {}, reloaded, &error), error.c_str());
        Require(reloaded.userSettings.Source() == FrontierX::NativeUserSettingsSource::Player,
            "stored player settings were not selected on restart");
        FrontierX::NativeComponentManager second_manager;
        defaults = CreateDefaults(second_manager, settings_path, reloaded);
        live = IUserDefaults::GetDefaults();
        Require(live && live->iMainX == -27 && live->iMainWidth == 1280 &&
                live->scrollRate == 2.5F && live->fogMode == FOGOWAR_NONE &&
                live->bWindowMode && live->bSubtitles && live->maxProjectiles == 73,
            "native schema-to-live-defaults restart mapping failed");
        Require(FrontierX::RenameNativePlayerDefaults(defaults,"Admiral","Renamed Admiral"),
            "native player settings rename failed");
        Require(defaults->SetStringInRegistry("CQPlayerName","Renamed Admiral") && defaults->LoadDefaults(),
            "renamed native player settings could not be selected");
        Require(IUserDefaults::GetDefaults()->iMainWidth==1280 &&
                IUserDefaults::GetDefaults()->maxProjectiles==73,
            "native player settings were lost during rename");
        Require(defaults->DeletePlayerDefaults("Renamed Admiral") == TRUE,
            "native player-default deletion failed");
        defaults->ResetToDefaultSettings();
        live = IUserDefaults::GetDefaults();
        Require(live->iMainX == 0 && live->scrollRate == 1.0F &&
                live->fogMode == FOGOWAR_NORMAL && live->maxProjectiles == 20,
            "native live defaults reset failed");
        defaults->Release();
        Require(second_manager.ShutDown() == GR_OK,
            "second native defaults manager shutdown failed");

        std::cout << "native IUserDefaults component passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native IUserDefaults probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
