#ifndef FRONTIERX_DATA_ROOT_H
#define FRONTIERX_DATA_ROOT_H

#include <filesystem>
#include <string>
#include <vector>

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
	NotDirectory
};

struct DataRootEntry
{
	std::filesystem::path expectedPath;
	std::filesystem::path actualPath;
	DataRootEntryStatus status;
	bool required;
};

std::vector<DataRootEntry> InspectDataRoot(const std::filesystem::path &root);
bool IsDataRootUsable(const std::vector<DataRootEntry> &entries) noexcept;
std::filesystem::path SelectDataRootPath(
	const std::filesystem::path &commandLinePath,
	const std::filesystem::path &environmentPath,
	const std::filesystem::path &fallbackPath);
const char *DataRootEntryStatusName(DataRootEntryStatus status) noexcept;

}
}

#endif