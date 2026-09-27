#include <FrontierX/DataRoot.h>
#include <FrontierX/UtfMetadata.h>
#include <FrontierX/NativeUtfFileSystem.h>

#include <algorithm>
#include <array>
#include <iterator>
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

	for (auto partIterator = relativePath.begin(); partIterator != relativePath.end(); ++partIterator)
	{
		const std::filesystem::path &part = *partIterator;
		const std::filesystem::path exactPath = actualPath / part;
		std::error_code error;
		const std::filesystem::file_status exactStatus = std::filesystem::status(exactPath, error);
		std::filesystem::path matchedPath;
		if (!error && std::filesystem::exists(exactStatus))
		{
			matchedPath = exactPath;
		}
		else
		{
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

			matchedPath = matches.front();
			caseMismatch = true;
		}
		const std::filesystem::file_status matchedStatus = std::filesystem::status(matchedPath, error);
		if (error)
			return {relativePath, {}, DataRootEntryStatus::Missing, required};
		if (std::filesystem::is_regular_file(matchedStatus))
		{
			std::string virtualDirectory;
			for (auto next = std::next(partIterator); next != relativePath.end(); ++next)
			{
				if (!virtualDirectory.empty()) virtualDirectory += '/';
				virtualDirectory += next->generic_string();
			}
			FrontierX::UtfMetadata metadata;
			std::string archiveError;
			if (!FrontierX::InspectUtfMetadata(matchedPath, metadata, archiveError))
				return {relativePath, matchedPath.lexically_relative(root),
					DataRootEntryStatus::InvalidArchive, required};
			std::vector<FrontierX::UtfDirectoryEntry> children;
			if (!FrontierX::ListUtfDirectory(matchedPath, virtualDirectory, children, archiveError))
				return {relativePath, matchedPath.lexically_relative(root),
					DataRootEntryStatus::Missing, required, virtualDirectory};
			return {relativePath, matchedPath.lexically_relative(root),
				DataRootEntryStatus::PresentArchive, required, virtualDirectory};
		}
		if (!std::filesystem::is_directory(matchedStatus))
			return {relativePath, matchedPath.lexically_relative(root), DataRootEntryStatus::NotDirectory, required};
		actualPath = matchedPath;
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
		if (entry.required && !IsDataRootEntryUsable(entry))
			return false;
	return true;
}

bool IsDataRootEntryUsable(const DataRootEntry &entry) noexcept
{
	return entry.status == DataRootEntryStatus::Present ||
		entry.status == DataRootEntryStatus::PresentArchive;
}

bool OpenDataRootArchive(const std::filesystem::path &root, const DataRootEntry &entry,
                        IFileSystem **output, std::string &error)
{
	if (!output) { error = "archive output is null"; return false; }
	*output = nullptr;
	if (entry.status != DataRootEntryStatus::PresentArchive || entry.actualPath.empty() ||
		entry.actualPath.is_absolute())
	{ error = "data source is not an inspected archive"; return false; }
	for (const auto &part : entry.actualPath)
		if (part == ".." || part == ".")
		{ error = "invalid archive source path"; return false; }
	struct ArchiveOwner
	{
		IFileSystem *value = nullptr;
		~ArchiveOwner() { if (value) value->Release(); }
	} current;
	if (!FrontierX::OpenNativeUtfFileSystem(root / entry.actualPath, &current.value, error)) return false;
	for (const auto &part : std::filesystem::path(entry.archiveDirectory))
	{
		const std::string name = part.string();
		DAFILEDESC descriptor(name.c_str());
		IFileSystem *child = nullptr;
		const GENRESULT result = current.value->CreateInstance(&descriptor, reinterpret_cast<void **>(&child));
		if (result != GR_OK || !child)
		{
			if (child) child->Release();
			error = "archive directory mount failed";
			return false;
		}
		current.value->Release();
		current.value = child;
	}
	*output = current.value;
	current.value = nullptr;
	error.clear();
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
	case DataRootEntryStatus::PresentArchive:
		return "present-archive";
	case DataRootEntryStatus::InvalidArchive:
		return "invalid-archive";
	}

	return "unknown";
}

}
}
