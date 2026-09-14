#include <FrontierX/DataRoot.h>

#include <algorithm>
#include <array>
#include <system_error>

namespace
{

struct DataDirectorySpec
{
	const char *path;
	bool required;
};

constexpr std::array<DataDirectorySpec, 12> DataDirectories = {{
	{"Objects", true},
	{"Interface", true},
	{"Textures", true},
	{"Mat", true},
	{"Profiles", true},
	{"Maps/Campaign", true},
	{"Maps/Multiplayer", true},
	{"Movies", false},
	{"Music", false},
	{"SFX", false},
	{"Speech", false},
	{"MSpeech", false}
}};

std::string AsciiLower(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(), [](char character) {
		if (character >= 'A' && character <= 'Z')
			return static_cast<char>(character + ('a' - 'A'));
		return character;
	});
	return value;
}

FrontierX::Platform::DataRootEntry InspectEntry(
	const std::filesystem::path &root,
	const std::filesystem::path &relativePath,
	bool required)
{
	using FrontierX::Platform::DataRootEntry;
	using FrontierX::Platform::DataRootEntryStatus;

	std::filesystem::path actualPath = root;
	bool caseMismatch = false;

	for (const std::filesystem::path &part : relativePath)
	{
		const std::filesystem::path exactPath = actualPath / part;
		std::error_code error;
		const std::filesystem::file_status exactStatus = std::filesystem::status(exactPath, error);
		if (!error && std::filesystem::exists(exactStatus))
		{
			if (!std::filesystem::is_directory(exactStatus))
				return {relativePath, exactPath.lexically_relative(root), DataRootEntryStatus::NotDirectory, required};
			actualPath = exactPath;
			continue;
		}

		std::vector<std::filesystem::path> matches;
		const std::string expectedName = AsciiLower(part.string());
		error.clear();
		for (std::filesystem::directory_iterator iterator(actualPath, error), end;
			!error && iterator != end; iterator.increment(error))
		{
			if (AsciiLower(iterator->path().filename().string()) == expectedName)
				matches.push_back(iterator->path());
		}

		if (error || matches.empty())
			return {relativePath, {}, DataRootEntryStatus::Missing, required};

		std::sort(matches.begin(), matches.end());
		if (matches.size() != 1)
			return {relativePath, matches.front().lexically_relative(root), DataRootEntryStatus::Ambiguous, required};

		const std::filesystem::file_status matchedStatus = std::filesystem::status(matches.front(), error);
		if (error || !std::filesystem::is_directory(matchedStatus))
			return {relativePath, matches.front().lexically_relative(root), DataRootEntryStatus::NotDirectory, required};

		actualPath = matches.front();
		caseMismatch = true;
	}

	return {
		relativePath,
		actualPath.lexically_relative(root),
		caseMismatch ? DataRootEntryStatus::CaseMismatch : DataRootEntryStatus::Present,
		required
	};
}

}

namespace FrontierX
{
namespace Platform
{

std::vector<DataRootEntry> InspectDataRoot(const std::filesystem::path &root)
{
	std::vector<DataRootEntry> entries;
	entries.reserve(DataDirectories.size());
	for (const DataDirectorySpec &directory : DataDirectories)
		entries.push_back(InspectEntry(root, directory.path, directory.required));
	return entries;
}

bool IsDataRootUsable(const std::vector<DataRootEntry> &entries) noexcept
{
	for (const DataRootEntry &entry : entries)
		if (entry.required && entry.status != DataRootEntryStatus::Present)
			return false;
	return true;
}

std::filesystem::path SelectDataRootPath(
	const std::filesystem::path &commandLinePath,
	const std::filesystem::path &environmentPath,
	const std::filesystem::path &fallbackPath)
{
	if (!commandLinePath.empty())
		return commandLinePath;
	if (!environmentPath.empty())
		return environmentPath;
	return fallbackPath;
}

const char *DataRootEntryStatusName(DataRootEntryStatus status) noexcept
{
	switch (status)
	{
	case DataRootEntryStatus::Present:
		return "present";
	case DataRootEntryStatus::Missing:
		return "missing";
	case DataRootEntryStatus::CaseMismatch:
		return "case-mismatch";
	case DataRootEntryStatus::Ambiguous:
		return "ambiguous";
	case DataRootEntryStatus::NotDirectory:
		return "not-directory";
	}

	return "unknown";
}

}
}