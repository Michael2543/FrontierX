#include <FrontierX/DataRoot.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

namespace
{

bool HasStatus(
	const std::vector<FrontierX::Platform::DataRootEntry> &entries,
	const std::filesystem::path &expectedPath,
	FrontierX::Platform::DataRootEntryStatus status)
{
	for (const FrontierX::Platform::DataRootEntry &entry : entries)
		if (entry.expectedPath == expectedPath && entry.status == status)
			return true;
	return false;
}

int RunSelfTest()
{
	using FrontierX::Platform::SelectDataRootPath;
	if (SelectDataRootPath("cli", "environment", "fallback") != "cli" ||
		SelectDataRootPath({}, "environment", "fallback") != "environment" ||
		SelectDataRootPath({}, {}, "fallback") != "fallback")
	{
		std::cerr << "Data-root selection precedence is incorrect\n";
		return 1;
	}

	const auto uniqueValue = std::chrono::steady_clock::now().time_since_epoch().count();
	const std::filesystem::path root =
		std::filesystem::temp_directory_path() / ("frontierx-data-root-" + std::to_string(uniqueValue));
	std::error_code error;

	for (const char *directory : {
		"Objects", "Interface", "Textures", "Mat", "Profiles", "Maps/Campaign", "Maps/Multiplayer",
		"Movies", "Music", "SFX", "Speech", "MSpeech"})
	{
		std::filesystem::create_directories(root / directory, error);
		if (error)
		{
			std::cerr << "Failed to create data-root test fixture: " << error.message() << '\n';
			std::filesystem::remove_all(root, error);
			return 1;
		}
	}

	const auto validEntries = FrontierX::Platform::InspectDataRoot(root);
	for (const FrontierX::Platform::DataRootEntry &entry : validEntries)
	{
		if (entry.status != FrontierX::Platform::DataRootEntryStatus::Present)
		{
			std::cerr << "Exact data-root fixture was not accepted\n";
			std::filesystem::remove_all(root, error);
			return 1;
		}
	}

	std::filesystem::remove_all(root / "Movies", error);
	const auto optionalMissingEntries = FrontierX::Platform::InspectDataRoot(root);
	if (!FrontierX::Platform::IsDataRootUsable(optionalMissingEntries) ||
		!HasStatus(optionalMissingEntries, "Movies", FrontierX::Platform::DataRootEntryStatus::Missing))
	{
		std::cerr << "Missing optional data directory incorrectly blocked startup\n";
		std::filesystem::remove_all(root, error);
		return 1;
	}
	std::filesystem::create_directories(root / "Movies", error);

	std::filesystem::rename(root / "Objects", root / "objects", error);
	std::filesystem::remove_all(root / "Profiles", error);
	std::ofstream(root / "Profiles").close();
	std::filesystem::remove_all(root / "Textures", error);
	std::filesystem::create_directories(root / "textures", error);
	std::filesystem::create_directories(root / "TEXTURES", error);
	std::filesystem::remove_all(root / "Maps/Multiplayer", error);
	const auto invalidEntries = FrontierX::Platform::InspectDataRoot(root);
	const bool passed =
		!FrontierX::Platform::IsDataRootUsable(invalidEntries) &&
		HasStatus(invalidEntries, "Objects", FrontierX::Platform::DataRootEntryStatus::CaseMismatch) &&
		HasStatus(invalidEntries, "Profiles", FrontierX::Platform::DataRootEntryStatus::NotDirectory) &&
		HasStatus(invalidEntries, "Textures", FrontierX::Platform::DataRootEntryStatus::Ambiguous) &&
		HasStatus(invalidEntries, "Maps/Multiplayer", FrontierX::Platform::DataRootEntryStatus::Missing);

	std::filesystem::remove_all(root, error);
	if (!passed)
	{
		std::cerr << "Data-root diagnostics did not identify invalid entries\n";
		return 1;
	}

	return 0;
}

int Inspect(const std::filesystem::path &root)
{
	std::error_code error;
	if (!std::filesystem::is_directory(root, error))
	{
		std::cerr << "Data root is not a readable directory: " << root.generic_string() << '\n';
		return 2;
	}

	const std::vector<FrontierX::Platform::DataRootEntry> entries =
		FrontierX::Platform::InspectDataRoot(root);
	for (const FrontierX::Platform::DataRootEntry &entry : entries)
	{
		std::cout << entry.expectedPath.generic_string() << ": "
			<< FrontierX::Platform::DataRootEntryStatusName(entry.status)
			<< (entry.required ? " [required]" : " [optional]");
		if (!entry.actualPath.empty() && entry.actualPath != entry.expectedPath)
			std::cout << " (found " << entry.actualPath.generic_string() << ')';
		std::cout << '\n';
	}
	return FrontierX::Platform::IsDataRootUsable(entries) ? 0 : 2;
}

}

int main(int argc, char **argv)
{
	if (argc == 2 && std::string(argv[1]) == "--self-test")
		return RunSelfTest();
	if (argc <= 2)
	{
		const char *environmentPath = std::getenv("FRONTIERX_DATA_ROOT");
		const std::filesystem::path root = FrontierX::Platform::SelectDataRootPath(
			argc == 2 ? std::filesystem::path(argv[1]) : std::filesystem::path(),
			environmentPath != nullptr ? std::filesystem::path(environmentPath) : std::filesystem::path(),
			"Data");
		return Inspect(root);
	}

	std::cerr << "Usage: frontierx_data_root_probe [data-root]|--self-test\n";
	return 1;
}