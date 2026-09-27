#ifndef FRONTIERX_DATA_ROOT_H
#define FRONTIERX_DATA_ROOT_H

#include <filesystem>
#include <string>
#include <vector>

struct IFileSystem;

namespace FrontierX
{
namespace Platform
{

enum class DataRootEntryStatus
{
	Present,
	Missing,
	CaseMismatch,
	Ambiguous,
	NotDirectory,
	PresentArchive,
	InvalidArchive
};

struct DataRootEntry
{
	std::filesystem::path expectedPath;
	std::filesystem::path actualPath;
	DataRootEntryStatus status;
	bool required;
	// For PresentArchive, actualPath identifies the physical UTF, not this virtual directory.
	std::string archiveDirectory = {};
};

std::vector<DataRootEntry> InspectDataRoot(const std::filesystem::path &root);
bool IsDataRootUsable(const std::vector<DataRootEntry> &entries) noexcept;
bool IsDataRootEntryUsable(const DataRootEntry &entry) noexcept;
// Open an inspected packed source through the existing read-only IFileSystem contract.
// The caller owns one output reference. Directory sources are not handled by this function.
bool OpenDataRootArchive(const std::filesystem::path &root, const DataRootEntry &entry,
                        IFileSystem **output, std::string &error);
std::filesystem::path SelectDataRootPath(
	const std::filesystem::path &commandLinePath,
	const std::filesystem::path &environmentPath,
	const std::filesystem::path &fallbackPath);
const char *DataRootEntryStatusName(DataRootEntryStatus status) noexcept;

}
}

#endif
