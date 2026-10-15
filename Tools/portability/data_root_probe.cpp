#include <FrontierX/DataRoot.h>
#include <FrontierX/NativeUtfFileSystem.h>
#include <FrontierX/NativeWritableUtfArchive.h>
#include <FrontierX/UtfMetadata.h>
#include <IUTFWriter.h>
#include <FrontierX/NativeDiskSpace.h>
#include <FrontierX/NativeSystemMap.h>
#include <Filesys.h>
#include <vector.h>
class Transform;
#include "SysMap.h"
#include "StringData.h"
#include "NativeAppMissionMedia.h"
#include "NativeAppGlobals.h"
#include "NativeAppArchetypeBytes.h"

#include <array>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
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

// Packed Maps is the first mount used by the production campaign selector.
bool WriteMaps(const std::filesystem::path &path, bool campaign = true)
{
    std::string names("/", 2);
    auto name = [&](const char *value) {
        const auto offset = static_cast<std::uint32_t>(names.size());
        names.append(value); names.push_back('\0'); return offset;
    };
    const auto campaignName = name(campaign ? "Campaign" : "Other");
    const auto multiplayerName = name("Multiplayer");
    const auto markerName = name("Marker");
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    auto word = [&](std::uint32_t value) {
        for (unsigned shift = 0; shift < 32; shift += 8) output.put(static_cast<char>(value >> shift));
    };
    constexpr std::uint32_t record = 44, namesOffset = 56 + 4 * record;
    for (std::uint32_t value : {0x20465455U, 0x101U, 56U, 4 * record, 0U, record,
                               namesOffset, std::uint32_t(names.size()), std::uint32_t(names.size()),
                               namesOffset + std::uint32_t(names.size()), 1U, 0U, 0U, 0U}) word(value);
    auto entry = [&](std::uint32_t next, std::uint32_t nameOffset, std::uint32_t flags,
                     std::uint32_t child, std::uint32_t size) {
        for (std::uint32_t value : {next, nameOffset, flags, 0U, child, size, size, size, 0U, 0U, 0U}) word(value);
    };
    entry(0, 0, 0x10, record, 0);
    entry(2 * record, campaignName, 0x10, 3 * record, 0);
    entry(0, multiplayerName, 0x10, 0, 0);
    entry(0, markerName, 0, 0, 1);
    output.write(names.data(), static_cast<std::streamsize>(names.size()));
    output.put('M');
    return output.good();
}

// UnitComm::getFilename consumes production StringData lookup. StringData
// must retain opaque StringPack records, including editor files. Extend the
// archive probe to cover owning storage, traversal and ambiguous names.
bool CheckUnitSpeechStrings(const std::filesystem::path &root)
{
    for (unsigned scenario = 0; scenario < 5; ++scenario)
    {
        IFileSystem *archive = nullptr;
        std::string error;
        if (!FrontierX::OpenNativeWritableUtfArchive(root / "speech.utf", &archive, error))
            return false;
        const auto write = [&](const char *name, const void *bytes, DWORD size) {
            DAFILEDESC desc(name);
            desc.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            desc.dwCreationDistribution = CREATE_ALWAYS;
            const HANDLE file = archive->OpenChild(&desc);
            if (file == INVALID_HANDLE_VALUE) return false;
            DWORD written = 0;
            const bool result = archive->WriteFile(file, bytes, size, &written, nullptr) && written == size;
            return archive->CloseHandle(file) && result;
        };
        std::array<char, 64> pack{};
        std::memcpy(pack.data(), "first.wav", 10);
        std::memcpy(pack.data() + 32, "second.wav", 11);
        if (scenario == 2) pack.fill('x');
        bool valid = archive->CreateDirectory("Voices") &&
            archive->SetCurrentDirectory("Voices");
        if (scenario != 4)
            valid = valid && write("SyntheticSelection", pack.data(), scenario == 1 ? 63U : 64U);
        valid = valid && archive->SetCurrentDirectory("..") &&
            archive->CreateDirectory("Parsed Files") && archive->SetCurrentDirectory("Parsed Files") &&
            write("EditorOnly", "x", 1) && archive->SetCurrentDirectory("..");
        if (scenario == 3) valid = valid && write("SyntheticSelection", pack.data(), 64);
        IStringData *owner = valid ? FrontierX::CreateNativeStringData(archive) : nullptr;
        if (scenario < 3)
        {
            U32 size = 0;
            auto *bytes = owner ? static_cast<const char *>(owner->GetArchetypeData("SyntheticSelection", size)) : nullptr;
            archive->Release(); archive = nullptr;
            valid = owner && bytes && size == (scenario == 1 ? 63U : 64U) &&
                std::memcmp(bytes, pack.data(), size) == 0 &&
                owner->GetArchetypeData("SyntheticSelection") == bytes &&
                owner->GetArchetypeData("syntheticselection", size) == nullptr && size == 0 &&
                owner->GetArchetypeData("EditorOnly") == nullptr;
        }
        else valid = valid && !owner;
        if (owner) owner->Release();
        if (archive) archive->Release();
        if (!valid) return false;
    }
    return true;
}

// The original MovieScreen loads an 82 MiB intro through its physical Movies
// owner. Preserve the general cap while testing the narrowly selected owner
// policy, its actual clone/child callers, and both boundaries with sparse data.
bool CheckPhysicalMovieLimits(const std::filesystem::path &root)
{
    const auto directory = root / "Movies";
    std::filesystem::create_directory(directory / "Nested");
    constexpr U32 size = U32(FrontierX::NativePhysicalFileLimit::Standard) + 1U;
    const auto sparse = [](const std::filesystem::path &path, U32 extent) {
        std::ofstream file(path, std::ios::binary);
        file.put(char(0x5a)); file.seekp(std::streamoff(extent) - 1); file.put(char(0xa5));
        return bool(file);
    };
    if (!sparse(directory / "large.mpg", size) ||
        !sparse(directory / "Nested/large.mpg", size) ||
        !sparse(directory / "oversize.mpg", U32(FrontierX::NativePhysicalFileLimit::Movies) + 1U)) return false;
    const auto read = [&](IFileSystem *files, const char *name) {
        DAFILEDESC desc(name);
        HANDLE file = files->OpenChild(&desc);
        if (file == INVALID_HANDLE_VALUE) return false;
        U8 first = 0, last = 0; DWORD count = 0;
        const bool valid = files->GetFileSize(file) == size &&
            files->ReadFile(file, &first, 1, &count, nullptr) && count == 1 && first == 0x5a &&
            files->SetFilePointer(file, S32(size - 1U), nullptr, FILE_BEGIN) == size - 1U &&
            files->ReadFile(file, &last, 1, &count, nullptr) && count == 1 && last == 0xa5;
        return files->CloseHandle(file) && valid;
    };
    const auto rejects = [](IFileSystem *files, const char *name) {
        DAFILEDESC desc(name); HANDLE file = files->OpenChild(&desc);
        if (file == INVALID_HANDLE_VALUE) return true;
        files->CloseHandle(file); return false;
    };
    IFileSystem *standard = nullptr, *movies = nullptr, *invalid = nullptr;
    std::string error;
    if (!FrontierX::OpenNativeDirectoryFileSystem(directory, &standard, error) ||
        !FrontierX::OpenNativeDirectoryFileSystem(directory, &movies, error, FrontierX::NativePhysicalFileLimit::Movies)) return false;
    // MScript::IsAudioThere gates Helper::DoSpeech on file attributes, before
    // choosing original audio/portrait or text fallback. OpenChild alone did
    // not detect the adapter's unconditional "missing" result. Attribute lookup
    // must not read payloads or inherit the movie/ordinary file-size limit.
    bool valid = standard->GetFileAttributes("large.mpg") == FILE_ATTRIBUTE_NORMAL &&
        standard->GetFileAttributes("oversize.mpg") == FILE_ATTRIBUTE_NORMAL &&
        standard->GetFileAttributes("Nested\\large.mpg") == FILE_ATTRIBUTE_NORMAL &&
        standard->GetFileAttributes("\\large.mpg") == FILE_ATTRIBUTE_NORMAL &&
        standard->GetFileAttributes("Nested") == FILE_ATTRIBUTE_DIRECTORY &&
        standard->GetFileAttributes("\\") == FILE_ATTRIBUTE_DIRECTORY &&
        standard->GetFileAttributes("missing.mpg") == 0xffffffffU &&
        standard->GetFileAttributes("LARGE.mpg") == 0xffffffffU &&
        standard->GetFileAttributes("Nested\\..\\large.mpg") == 0xffffffffU &&
        standard->GetFileAttributes(nullptr) == 0xffffffffU &&
        standard->GetFileAttributes("") == 0xffffffffU &&
        rejects(standard, "large.mpg") && read(movies, "large.mpg") && rejects(movies, "oversize.mpg") &&
        !FrontierX::OpenNativeDirectoryFileSystem(directory, &invalid, error,
            static_cast<FrontierX::NativePhysicalFileLimit>(1U)) && !invalid;
    for (const char *name : {"\\", "Nested"})
    {
        DAFILEDESC desc(name); desc.lpImplementation = "DOS"; desc.dwDesiredAccess = GENERIC_READ;
        IFileSystem *child = nullptr;
        const bool created = movies->CreateInstance(&desc, reinterpret_cast<void **>(&child)) == GR_OK && child;
        valid = created && valid;
        if (child) { valid = read(child, "large.mpg") && valid; child->Release(); }
    }
    standard->Release(); movies->Release();
    std::filesystem::remove_all(directory / "Nested");
    std::filesystem::remove(directory / "large.mpg");
    std::filesystem::remove(directory / "oversize.mpg");
    return valid;
}

int RunSelfTest()
{
	using FrontierX::Platform::SelectDataRootPath;
	wchar_t wide[8] = {};
	char narrow[8] = {};
	if (_localAnsiToWide("Sol_T", wide, sizeof(wide)) != 6 ||
		_localWideToAnsi(wide, narrow, sizeof(narrow)) != 6 ||
		std::string(narrow) != "Sol_T" ||
		_localAnsiToWide("Sol_T", wide, sizeof(wchar_t) * 5) != 0 ||
		_localAnsiToWide("\xc3\xa9", wide, sizeof(wide)) != 0 ||
		_localWideToAnsi(L"\u00e9", narrow, sizeof(narrow)) != 0)
	{
		std::cerr << "Native App name conversion changed\n";
		return 1;
	}
	// Mission::SaveAs needs a UTF 1.01 writer. Protect its fixed-width
	// directory/sibling layout against the production reader before binding
	// a writable IFileSystem to SAVEDIR.
	std::vector<FrontierX::UtfArchiveItem> saveItems = {
		{"MT_GlobalData/Globals", false, {1, 2, 3, 4}},
		{"Empty", true, {}},
	};
	std::vector<unsigned char> encoded;
	std::string utfError;
	FrontierX::UtfMetadata metadata;
	std::vector<unsigned char> loaded;
	std::vector<FrontierX::UtfDirectoryEntry> listed;
	if (!FrontierX::EncodeUtfArchive(saveItems, encoded, utfError) ||
		!FrontierX::InspectUtfMetadata(encoded, metadata, utfError) ||
		metadata.directoryEntries != 4 || metadata.reachableFiles != 1 ||
		!FrontierX::ReadUtfEntry(encoded, "MT_GlobalData/Globals", loaded, utfError) ||
		loaded != saveItems[0].bytes ||
		!FrontierX::ListUtfDirectory(encoded, "", listed, utfError) ||
		listed.size() != 2 || listed[0].name != "MT_GlobalData" ||
		listed[1].name != "Empty")
	{
		std::cerr << "Native UTF writer/production reader round trip failed: " << utfError << '\n';
		return 1;
	}
	std::vector<unsigned char> reordered;
	if (!FrontierX::EncodeUtfArchive(saveItems, reordered, utfError) || reordered != encoded)
	{
		std::cerr << "Native UTF writer changed bytes with identical ordered input\n";
		return 1;
	}
	// Original DirectoryManager::addChildEntry appends siblings; original
	// UTF::FindNextFile follows those links. Lexical canonicalization loses
	// an input that the real ObjectList loader/physics consumer observes.
	std::reverse(saveItems.begin(), saveItems.end());
	if (!FrontierX::EncodeUtfArchive(saveItems, reordered, utfError) || reordered == encoded ||
		!FrontierX::ListUtfDirectory(reordered, "", listed, utfError) ||
		listed.size() != 2 || listed[0].name != "Empty" || listed[1].name != "MT_GlobalData")
	{
		std::cerr << "Native UTF writer lost original sibling creation order\n";
		return 1;
	}
	const auto lastValid = reordered;
	saveItems.push_back(saveItems.back());
	if (FrontierX::EncodeUtfArchive(saveItems, reordered, utfError) || reordered != lastValid)
	{
		std::cerr << "Native UTF writer accepted duplicate path or changed output on failure\n";
		return 1;
	}
	saveItems.pop_back();
	saveItems.push_back({"../escaped", false, {9}});
	if (FrontierX::EncodeUtfArchive(saveItems, reordered, utfError) || reordered != lastValid)
	{
		std::cerr << "Native UTF writer accepted traversal or changed output on failure\n";
		return 1;
	}
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
	{
        if (!CheckPhysicalMovieLimits(root))
        {
            std::cerr << "Physical movie limits, clone/child inheritance or sparse sentinels failed\n";
            std::filesystem::remove_all(root, error); return 1;
        }
		const auto writerPath = root / "roundtrip.utf";
		std::ofstream file(writerPath, std::ios::binary | std::ios::trunc);
		file.write(reinterpret_cast<const char *>(encoded.data()),
			static_cast<std::streamsize>(encoded.size()));
		file.close();
		IFileSystem *archive = nullptr;
		if (!file || !FrontierX::OpenNativeUtfFileSystem(writerPath, &archive, utfError))
		{
			std::cerr << "Native UTF writer output did not open through IFileSystem: " << utfError << '\n';
			std::filesystem::remove_all(root, error);
			return 1;
		}
		DAFILEDESC descriptor("MT_GlobalData\\Globals");
		const HANDLE child = archive->OpenChild(&descriptor);
		unsigned char readback[4] = {};
		DWORD bytesRead = 0;
		const bool valid = child != INVALID_HANDLE_VALUE &&
			archive->ReadFile(child, readback, sizeof(readback), &bytesRead, nullptr) &&
			bytesRead == sizeof(readback) &&
			std::equal(std::begin(readback), std::end(readback), loaded.begin()) &&
			archive->CloseHandle(child);
		DAFILEDESC foldedDescriptor("mt_globaldata\\globals");
		const HANDLE foldedChild = archive->OpenChild(&foldedDescriptor);
		unsigned char foldedReadback[4] = {};
		DWORD foldedBytesRead = 0;
		const bool foldedValid = foldedChild != INVALID_HANDLE_VALUE &&
			archive->ReadFile(foldedChild, foldedReadback, sizeof(foldedReadback),
				&foldedBytesRead, nullptr) &&
			foldedBytesRead == sizeof(foldedReadback) &&
			std::equal(std::begin(foldedReadback), std::end(foldedReadback), loaded.begin()) &&
			archive->CloseHandle(foldedChild);
		archive->Release();
		if (!valid || !foldedValid)
		{
			std::cerr << "Native UTF writer output did not round trip through IFileSystem\n";
			std::filesystem::remove_all(root, error);
			return 1;
		}
	}
	{
		const auto writerPath = root / "writable.utf";
		IFileSystem * writable = nullptr;
		if (!FrontierX::OpenNativeWritableUtfArchive(writerPath, &writable, utfError) ||
			std::filesystem::exists(writerPath) ||
			!writable->CreateDirectory("\\MT_GlobalData") ||
			!writable->SetCurrentDirectory("\\MT_GlobalData"))
		{
			std::cerr << "Writable UTF archive startup failed: " << utfError << '\n';
			if (writable) writable->Release();
			std::filesystem::remove_all(root, error);
			return 1;
		}
		DAFILEDESC descriptor("Globals");
		descriptor.lpImplementation = "DOS";
		descriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		descriptor.dwCreationDistribution = CREATE_ALWAYS;
		IFileSystem * child = nullptr;
		DWORD written = 0;
		IUTFWriter * flush = nullptr;
		const unsigned char value[4] = {9, 8, 7, 6};
		WIN32_FIND_DATA sibling{};
		const bool siblingCreated = writable->CreateDirectory("z-order") &&
			writable->CreateDirectory("a-order");
		const HANDLE siblingSearch = siblingCreated
			? writable->FindFirstFile("*-order", &sibling) : INVALID_HANDLE_VALUE;
		const bool siblingOrder = siblingSearch != INVALID_HANDLE_VALUE &&
			std::strcmp(sibling.cFileName, "z-order") == 0 &&
			writable->FindNextFile(siblingSearch, &sibling) &&
			std::strcmp(sibling.cFileName, "a-order") == 0 &&
			!writable->FindNextFile(siblingSearch, &sibling);
		const bool siblingClosed = siblingSearch == INVALID_HANDLE_VALUE ||
			writable->FindClose(siblingSearch);
		const bool saved = siblingOrder && siblingClosed && writable->CreateInstance(&descriptor,
			reinterpret_cast<void **>(&child)) == GR_OK && child &&
			child->WriteFile(nullptr, value, sizeof(value), &written, nullptr) &&
			written == sizeof(value) &&
			writable->QueryInterface(IID_IUTFWriter,
				reinterpret_cast<void **>(&flush)) == GR_OK && flush;
		if (saved) flush->Flush();
		if (flush) flush->Release();
		if (child) child->Release();
		writable->Release();
		std::vector<unsigned char> persisted;
		if (!saved || !FrontierX::ReadUtfEntry(writerPath,
			"MT_GlobalData/Globals", persisted, utfError) ||
			persisted != std::vector<unsigned char>(std::begin(value), std::end(value)) ||
			!FrontierX::ListUtfDirectory(writerPath, "MT_GlobalData", listed, utfError) ||
			listed.size() != 3 || listed[0].name != "z-order" ||
			listed[1].name != "a-order" || listed[2].name != "Globals")
		{
			std::cerr << "Writable UTF IFileSystem/Flush round trip failed: " << utfError << '\n';
			std::filesystem::remove_all(root, error);
			return 1;
		}
	}
	{
		IFileSystem * saveDirectory = nullptr;
		if (!FrontierX::OpenNativeSaveDirectory(root, &saveDirectory, utfError))
		{
			std::cerr << "Native save-directory factory failed: " << utfError << '\n';
			std::filesystem::remove_all(root, error);
			return 1;
		}
		DAFILEDESC save("saved.mission");
		// GameProgress opens a raw twelve-byte DOS record, not a UTF archive.
		DAFILEDESC progress("player.gpf");
		progress.lpImplementation = "DOS";
		progress.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		progress.dwCreationDistribution = CREATE_ALWAYS;
		const U32 progressWords[3] = {1, 0, 1};
		IFileSystem * progressFile = nullptr;
		DWORD progressBytes = 0;
		const auto rawCreateResult = saveDirectory->CreateInstance(&progress,
			reinterpret_cast<void **>(&progressFile));
		bool rawSaved = rawCreateResult == GR_OK && progressFile &&
			progressFile->WriteFile(nullptr, progressWords, sizeof(progressWords),
				&progressBytes, nullptr) && progressBytes == sizeof(progressWords);
		if (progressFile) progressFile->Release();
		progressFile = nullptr;
		progress.dwDesiredAccess = GENERIC_READ;
		progress.dwCreationDistribution = OPEN_EXISTING;
		U32 loadedWords[3] = {};
		const bool rawWritten = rawSaved;
		const auto rawReadResult = saveDirectory->CreateInstance(&progress,
			reinterpret_cast<void **>(&progressFile));
		rawSaved = rawSaved && rawReadResult == GR_OK && progressFile &&
			progressFile->ReadFile(nullptr, loadedWords, sizeof(loadedWords),
				&progressBytes, nullptr) && progressBytes == sizeof(loadedWords) &&
			std::equal(std::begin(progressWords), std::end(progressWords),
				std::begin(loadedWords)) &&
			std::filesystem::file_size(root / "player.gpf") == sizeof(progressWords);
		if (progressFile) progressFile->Release();
		if (!rawSaved)
		{
			std::cerr << "Native GameProgress DOS save-directory round trip failed; create="
				<< rawCreateResult << " written=" << rawWritten << " reopen=" << rawReadResult
				<< " read-bytes=" << progressBytes << '\n';
			saveDirectory->Release();
			std::filesystem::remove_all(root, error);
			return 1;
		}
		save.lpImplementation = "UTF";
		save.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		save.dwCreationDistribution = CREATE_ALWAYS;
		IFileSystem * archive = nullptr;
		IFileSystem * parent = nullptr;
		IUTFWriter * flush = nullptr;
		const bool created = saveDirectory->CreateInstance(&save,
			reinterpret_cast<void **>(&archive)) == GR_OK && archive &&
			archive->GetParentSystem(&parent) == GR_OK && parent == saveDirectory &&
			archive->CreateDirectory("\\MT_GlobalData") &&
			archive->SetCurrentDirectory("\\MT_GlobalData");
		if (parent) parent->Release();
		DAFILEDESC record("Globals");
		record.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		record.dwCreationDistribution = CREATE_ALWAYS;
		const unsigned char value[3] = {4, 3, 2};
		DWORD written = 0;
		HANDLE handle = INVALID_HANDLE_VALUE;
		bool saved = false;
		if (created)
		{
			handle = archive->OpenChild(&record);
			saved = handle != INVALID_HANDLE_VALUE &&
				archive->WriteFile(handle, value, sizeof(value), &written, nullptr) &&
				written == sizeof(value) && archive->CloseHandle(handle);
			IFileSystem * instanceDirectory = nullptr;
			IFileSystem * nestedRecord = nullptr;
			DAFILEDESC instance("MISSION!!TRIGGER\\0");
			instance.lpImplementation = "DOS";
			instance.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			instance.dwCreationDistribution = CREATE_ALWAYS;
			DAFILEDESC child("JUMPGATE_SAVELOAD");
			child.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			child.dwCreationDistribution = CREATE_ALWAYS;
			DWORD nestedWritten = 0;
			saved = saved && archive->SetCurrentDirectory("\\") &&
				archive->CreateDirectory("\\ObjectList") &&
				archive->SetCurrentDirectory("\\ObjectList") &&
				archive->CreateDirectory("Instances") &&
				archive->SetCurrentDirectory("Instances") &&
				archive->CreateDirectory("MISSION!!TRIGGER") &&
				archive->CreateDirectory("MISSION!!TRIGGER\\0") &&
				archive->CreateInstance(&instance,
					reinterpret_cast<void **>(&instanceDirectory)) == GR_OK &&
				instanceDirectory &&
				instanceDirectory->CreateInstance(&child,
					reinterpret_cast<void **>(&nestedRecord)) == GR_OK &&
				nestedRecord &&
				nestedRecord->WriteFile(nullptr, value, sizeof(value),
					&nestedWritten, nullptr) && nestedWritten == sizeof(value);
			if (nestedRecord) nestedRecord->Release();
			if (instanceDirectory) instanceDirectory->Release();
			// MScript saves active programs, then reloads this absolute virtual directory.
			DAFILEDESC program("0-SolTrain_HelionFinished");
			program.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			program.dwCreationDistribution = CREATE_ALWAYS;
			saved = saved && archive->CreateDirectory("\\Script") &&
				archive->CreateDirectory("\\Script\\Programs") &&
				archive->SetCurrentDirectory("\\Script\\Programs");
			const HANDLE programHandle = saved ? archive->OpenChild(&program) : INVALID_HANDLE_VALUE;
			saved = saved && programHandle != INVALID_HANDLE_VALUE &&
				archive->WriteFile(programHandle, value, sizeof(value), &written, nullptr) &&
				written == sizeof(value) && archive->CloseHandle(programHandle);
			// Mission::Save uses the source's unversioned interface name.
			saved = saved && archive->QueryInterface("IUTFWriter",
					reinterpret_cast<void **>(&flush)) == GR_OK && flush;
		}
		if (saved) flush->Flush();
		if (flush) flush->Release();
		if (archive) archive->Release();
		DAFILEDESC reopen("saved.mission");
		IFileSystem * reader = nullptr;
		IFileSystem * reopenedParent = nullptr;
		const bool reopened = saved &&
			saveDirectory->CreateInstance(&reopen,
				reinterpret_cast<void **>(&reader)) == GR_OK && reader &&
			reader->GetParentSystem(&reopenedParent) == GR_OK &&
			reopenedParent == saveDirectory;
		unsigned char throughReader[3] = {};
		DWORD bytesRead = 0;
		bool loadedFromInterface = false;
		if (reopened)
		{
			DAFILEDESC savedRecord("\\MT_GlobalData\\Globals");
			const HANDLE savedHandle = reader->OpenChild(&savedRecord);
			loadedFromInterface = savedHandle != INVALID_HANDLE_VALUE &&
				reader->ReadFile(savedHandle, throughReader,
					sizeof(throughReader), &bytesRead, nullptr) &&
				bytesRead == sizeof(throughReader) &&
				reader->CloseHandle(savedHandle) &&
				std::equal(std::begin(throughReader), std::end(throughReader),
					std::begin(value));
			IFileSystem * programs = nullptr;
			DAFILEDESC programDirectory("\\Script\\Programs");
			loadedFromInterface = loadedFromInterface &&
				reader->SetCurrentDirectory("\\ObjectList") &&
				reader->CreateInstance(&programDirectory,
					reinterpret_cast<void **>(&programs)) == GR_OK && programs;
			if (programs)
			{
				WIN32_FIND_DATA found = {};
				const HANDLE search = programs->FindFirstFile("?*-*", &found);
				DAFILEDESC program(found.cFileName);
				const HANDLE file = search == INVALID_HANDLE_VALUE ? INVALID_HANDLE_VALUE :
					programs->OpenChild(&program);
				loadedFromInterface = loadedFromInterface && search != INVALID_HANDLE_VALUE &&
					std::strcmp(found.cFileName, "0-SolTrain_HelionFinished") == 0 &&
					file != INVALID_HANDLE_VALUE &&
					programs->ReadFile(file, throughReader, sizeof(throughReader), &bytesRead, nullptr) &&
					bytesRead == sizeof(value) &&
					std::equal(std::begin(throughReader), std::end(throughReader), std::begin(value));
				if (file != INVALID_HANDLE_VALUE) programs->CloseHandle(file);
				if (search != INVALID_HANDLE_VALUE) programs->FindClose(search);
				DAFILEDESC traversal("\\Script\\..\\Programs");
				IFileSystem * rejected = nullptr;
				loadedFromInterface = loadedFromInterface &&
					reader->CreateInstance(&traversal, reinterpret_cast<void **>(&rejected)) != GR_OK &&
					!rejected;
				if (rejected) rejected->Release();
				programs->Release();
			}
		}
		if (reopenedParent) reopenedParent->Release();
		if (reader) reader->Release();
		saveDirectory->Release();
		std::vector<unsigned char> persisted;
		if (!reopened || !loadedFromInterface ||
			!FrontierX::ReadUtfEntry(root / "saved.mission",
			"MT_GlobalData/Globals", persisted, utfError) ||
			persisted != std::vector<unsigned char>(std::begin(value), std::end(value)) ||
			!FrontierX::ReadUtfEntry(root / "saved.mission",
			"ObjectList/Instances/MISSION!!TRIGGER/0/JUMPGATE_SAVELOAD",
			persisted, utfError) ||
			persisted != std::vector<unsigned char>(std::begin(value), std::end(value)))
		{
			std::cerr << "Native save-directory create/flush/reopen failed: " << utfError << '\n';
			std::filesystem::remove_all(root, error);
			return 1;
		}
	}
	std::uint64_t available = UINT64_MAX;
	if (!FrontierX::Platform::AvailableDiskBytes(root.string().c_str(), available) ||
		available == UINT64_MAX)
	{
		std::cerr << "Native save-space query failed for an existing filesystem\n";
		std::filesystem::remove_all(root, error);
		return 1;
	}
	available = 7;
	if (FrontierX::Platform::AvailableDiskBytes((root / "missing").string().c_str(), available) ||
		available != 7)
	{
		std::cerr << "Native save-space query accepted a missing path\n";
		std::filesystem::remove_all(root, error);
		return 1;
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

	std::filesystem::remove_all(root / "Maps", error);
	if (!WriteMaps(root / "maps")) return 1;
	const auto packedEntries = FrontierX::Platform::InspectDataRoot(root);
	// PlanetFactory's MeshManager asks IFileSystem::CreateInstance for a
	// nested Windows-style mesh path; the archive must resolve it safely.
	IFileSystem *archive = nullptr;
	if (!FrontierX::OpenNativeUtfFileSystem(root / "maps", &archive, utfError)) return 1;
    const bool attributesValid =
        archive->GetFileAttributes("Campaign\\Marker") == FILE_ATTRIBUTE_NORMAL &&
        archive->GetFileAttributes("campaign\\marker") == FILE_ATTRIBUTE_NORMAL &&
        archive->GetFileAttributes("Campaign") == FILE_ATTRIBUTE_DIRECTORY &&
        archive->GetFileAttributes("campaign") == FILE_ATTRIBUTE_DIRECTORY &&
        archive->GetFileAttributes("\\") == FILE_ATTRIBUTE_DIRECTORY &&
        archive->GetFileAttributes("Campaign\\missing") == 0xffffffffU &&
        archive->GetFileAttributes("Campaign\\..\\Marker") == 0xffffffffU &&
        archive->GetFileAttributes("Campaign\\*") == 0xffffffffU;
	DAFILEDESC nested("Campaign\\Marker");
	IFileSystem *nestedFile = nullptr;
	const GENRESULT nestedResult = archive->CreateInstance(
		&nested, reinterpret_cast<void **>(&nestedFile));
	DAFILEDESC traversal("Campaign\\..\\Marker");
	IFileSystem *traversed = nullptr;
	const GENRESULT traversalResult = archive->CreateInstance(
		&traversal, reinterpret_cast<void **>(&traversed));
	const bool nestedOpened = nestedFile != nullptr;
	const bool traversalOpened = traversed != nullptr;
	if (traversed) traversed->Release();
	if (nestedFile) nestedFile->Release();
	archive->Release();
	if (!attributesValid || nestedResult != GR_OK || !nestedOpened || traversalResult == GR_OK || traversalOpened)
	{
		std::cerr << "Nested archive child or traversal validation failed\n";
		return 1;
	}
	bool mounted = false;
	for (const auto &entry : packedEntries)
	{
		if (entry.expectedPath != "Maps/Campaign") continue;
		IFileSystem *files = nullptr;
		std::string mountError;
		if (entry.actualPath != "maps" || entry.archiveDirectory != "Campaign" ||
			!FrontierX::Platform::OpenDataRootArchive(root, entry, &files, mountError)) return 1;
		if (RecursiveDelete(files) != FALSE)
		{
			std::cerr << "Save cleanup accepted a read-only retail-style archive\n";
			files->Release();
			return 1;
		}
		DAFILEDESC marker("Marker");
		HANDLE child = files->OpenChild(&marker);
		char value = 0;
		DWORD read = 0;
		mounted = child != INVALID_HANDLE_VALUE && files->ReadFile(child, &value, 1, &read, nullptr) &&
			read == 1 && value == 'M' && files->CloseHandle(child);
        // Mission::Reload resolves \Map_Mission1_3.qmission from its Campaign
        // view. BaseUTF anchors absolute paths to that view, not the outer maps.
        // The prior root-wide resolver loses terrain and reaches platform park
        // assertions; this existing synthetic mount check gates campaign load.
        DAFILEDESC absoluteMarker("\\Marker");
        HANDLE absolute = files->OpenChild(&absoluteMarker);
        char absoluteValue = 0;
        DWORD absoluteRead = 0;
        mounted = mounted && absolute != INVALID_HANDLE_VALUE &&
            files->ReadFile(absolute, &absoluteValue, 1, &absoluteRead, nullptr) &&
            absoluteRead == 1 && absoluteValue == 'M';
        if (absolute != INVALID_HANDLE_VALUE) mounted = files->CloseHandle(absolute) && mounted;
        mounted = mounted && files->SetCurrentDirectory("..") == FALSE &&
            files->SetCurrentDirectory("\\") != FALSE &&
            files->GetFileAttributes("Marker") == FILE_ATTRIBUTE_NORMAL &&
            files->GetFileAttributes("\\Marker") == FILE_ATTRIBUTE_NORMAL &&
            files->GetFileAttributes("marker") == FILE_ATTRIBUTE_NORMAL &&
            files->GetFileAttributes("\\") == FILE_ATTRIBUTE_DIRECTORY &&
            files->GetFileAttributes("..\\Marker") == 0xffffffffU &&
            files->GetFileAttributes("\\Multiplayer") == 0xffffffffU;

		files->Release();
	}
	if (!mounted || !FrontierX::Platform::IsDataRootUsable(packedEntries)) return 1;
	if (!WriteMaps(root / "maps", false)) return 1;
	for (const auto &entry : packedEntries)
	{
		if (entry.expectedPath != "Maps/Campaign") continue;
		IFileSystem *stale = nullptr;
		std::string mountError;
		if (FrontierX::Platform::OpenDataRootArchive(root, entry, &stale, mountError) || stale)
			return 1;
	}
	if (!HasStatus(FrontierX::Platform::InspectDataRoot(root), "Maps/Campaign",
		FrontierX::Platform::DataRootEntryStatus::Missing)) return 1;
	std::ofstream(root / "maps", std::ios::trunc) << "UTF ";
	if (!HasStatus(FrontierX::Platform::InspectDataRoot(root), "Maps/Campaign",
		FrontierX::Platform::DataRootEntryStatus::InvalidArchive)) return 1;
	std::filesystem::remove(root / "maps", error);
	std::filesystem::create_directories(root / "Maps/Campaign", error);
	std::filesystem::create_directories(root / "Maps/Multiplayer", error);

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
		HasStatus(invalidEntries, "Profiles", FrontierX::Platform::DataRootEntryStatus::InvalidArchive) &&
		HasStatus(invalidEntries, "Textures", FrontierX::Platform::DataRootEntryStatus::Ambiguous) &&
		HasStatus(invalidEntries, "Maps/Multiplayer", FrontierX::Platform::DataRootEntryStatus::Missing);

    if (!CheckUnitSpeechStrings(root))
    {
        std::cerr << "Production StringData unit speech slot contract failed\n";
        std::filesystem::remove_all(root, error);
        return 1;
    }
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

int InspectMissionFilesystem(const std::filesystem::path &root, const std::string &missionName)
{
	if (missionName.empty() || missionName.find_first_of("/\\") != std::string::npos ||
		missionName == "." || missionName == "..")
	{
		std::cerr << "Mission name must be one archive entry\n";
		return 1;
	}
	using FileOwner = std::unique_ptr<IFileSystem, void (*)(IFileSystem *)>;
	const auto release = [](IFileSystem *file) { if (file) file->Release(); };
	FileOwner campaign(nullptr, release);
	FileOwner mission(nullptr, release);
	FileOwner effects(nullptr, release);
	std::string effectsError;
	const auto entries = FrontierX::Platform::InspectDataRoot(root);
	for (const auto &entry : entries)
	{
		if (entry.expectedPath == "SFX")
		{
			IFileSystem *opened = nullptr;
			if (entry.status == FrontierX::Platform::DataRootEntryStatus::PresentArchive)
				FrontierX::Platform::OpenDataRootArchive(root, entry, &opened, effectsError);
			else if (entry.status == FrontierX::Platform::DataRootEntryStatus::Present)
				FrontierX::OpenNativeDirectoryFileSystem(root / entry.actualPath, &opened, effectsError);
			effects.reset(opened);
			continue;
		}
		if (entry.expectedPath != "Maps/Campaign") continue;
		IFileSystem *opened = nullptr;
		std::string error;
		if (entry.status == FrontierX::Platform::DataRootEntryStatus::PresentArchive)
		{
			if (!FrontierX::Platform::OpenDataRootArchive(root, entry, &opened, error)) break;
		}
		else if (entry.status == FrontierX::Platform::DataRootEntryStatus::Present)
		{
			if (!FrontierX::OpenNativeDirectoryFileSystem(
					root / entry.actualPath, &opened, error)) break;
		}
		campaign.reset(opened);
		continue;
	}
	// SFX::BUTTONPRESS1 maps to interface1.wav in the source sfxdata.xmf.
	// The native App sound owner must read through this same external source.
	DAFILEDESC effectDescriptor("interface1.wav");
	HANDLE effect = effects ? effects->OpenChild(&effectDescriptor) : INVALID_HANDLE_VALUE;
	if (effect == INVALID_HANDLE_VALUE && effects)
	{
		WIN32_FIND_DATA found = {};
		const HANDLE search = effects->FindFirstFile("interface1.wav", &found);
		if (search != INVALID_HANDLE_VALUE)
		{
			WIN32_FIND_DATA next = {};
			const bool unique = !effects->FindNextFile(search, &next);
			if (effects->FindClose(search) && unique &&
				!(found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				DAFILEDESC actual(found.cFileName);
				effect = effects->OpenChild(&actual);
			}
		}
	}
	std::array<unsigned char, 64> waveHeader = {};
	DWORD effectBytes = 0;
	bool effectReadable = false;
	if (effect != INVALID_HANDLE_VALUE)
	{
		effectReadable = effects->ReadFile(effect, waveHeader.data(), waveHeader.size(),
			&effectBytes, nullptr) && effectBytes == waveHeader.size() &&
			std::memcmp(waveHeader.data(), "RIFF", 4) == 0 &&
			std::memcmp(waveHeader.data() + 8, "WAVEfmt ", 8) == 0;
		effectReadable = effects->CloseHandle(effect) && effectReadable;
	}
	if (!effectReadable)
	{
		std::cerr << "Selected mission sound effect source unavailable (archive: "
			<< (effects ? "mounted" : "missing") << "; " << effectsError << ")\n";
		return 2;
	}
	const unsigned waveFormat = waveHeader[20] | (unsigned(waveHeader[21]) << 8);
	std::cout << "Source-mapped SFX WAV format: " << waveFormat << '\n';
	if (!campaign)
	{
		std::cerr << "Campaign source unavailable\n";
		return 2;
	}
	DAFILEDESC missionDescriptor(missionName.c_str());
	IFileSystem *openedMission = nullptr;
	if (campaign->CreateInstance(&missionDescriptor,
			reinterpret_cast<void **>(&openedMission)) != GR_OK || !openedMission)
	{
		std::cerr << "Mission UTF entry unavailable\n";
		return 2;
	}
	mission.reset(openedMission);
	DAFILEDESC parseDescriptor("\\ParseData\\Data.h");
	const HANDLE parse = mission->OpenChild(&parseDescriptor);
	if (parse == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Mission ParseData/Data.h unavailable\n";
		return 2;
	}
	const DWORD parseSize = mission->GetFileSize(parse, nullptr);
	const BOOL closed = mission->CloseHandle(parse);
	if (parseSize == 0 || parseSize == 0xffffffffU || !closed ||
		!mission->SetCurrentDirectory("\\MT_GlobalData"))
	{
		std::cerr << "Mission global-data directory unavailable\n";
		return 2;
	}
	WIN32_FIND_DATA found{};
	const HANDLE search = mission->FindFirstFile("*", &found);
	if (search == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Mission global-data directory is empty\n";
		return 2;
	}
	unsigned count = 1;
	while (mission->FindNextFile(search, &found)) ++count;
	DAFILEDESC globalsDescriptor("Globals");
	const HANDLE globals = mission->OpenChild(&globalsDescriptor);
	DWORD globalsSize = 0;
	char firstByte = 0;
	DWORD read = 0;
	if (globals != INVALID_HANDLE_VALUE)
	{
		globalsSize = mission->GetFileSize(globals, nullptr);
		if (!mission->ReadFile(globals, &firstByte, 1, &read, nullptr) ||
			!mission->CloseHandle(globals)) read = 0;
	}
	if (!mission->FindClose(search) || globalsSize == 0 ||
		globalsSize == 0xffffffffU || read != 1 ||
		!mission->SetCurrentDirectory("\\"))
	{
		std::cerr << "Mission global-data read failed\n";
		return 2;
	}
	if (missionName == "Sol_Train.dmission")
	{
		// Mission::loadPlayerAI probes PLAYERAI/Player02..PlayerNN in a defined mission.
		// This decides whether the selected fresh path requires the full AI factory.
		unsigned savedAiPlayers = 0;
		for (unsigned player = 2; player <= MAX_PLAYERS; ++player)
		{
			char aiDirectory[32];
			std::snprintf(aiDirectory, sizeof(aiDirectory), "\\PLAYERAI\\Player%02u", player);
			if (mission->SetCurrentDirectory(aiDirectory))
			{
				++savedAiPlayers;
				if (!mission->SetCurrentDirectory("\\"))
				{
					std::cerr << "Mission player-AI directory reset failed\n";
					return 2;
				}
			}
		}
		std::cout << "Mission saved player-AI directories: " << savedAiPlayers << '\n';
		if (savedAiPlayers != 0)
		{
			std::cerr << "Selected mission requires the production player-AI owner\n";
			return 2;
		}
		// MusicManager::Load treats an absent MT_MUSIC_DATA/Music child as fresh playlist state.
		DAFILEDESC musicDescriptor("\\MT_MUSIC_DATA\\Music");
		const HANDLE music = mission->OpenChild(&musicDescriptor);
		if (music != INVALID_HANDLE_VALUE)
		{
			const DWORD musicBytes = mission->GetFileSize(music, nullptr);
			if (!mission->CloseHandle(music) || musicBytes == 0xffffffffU)
			{
				std::cerr << "Mission music save inspection failed\n";
				return 2;
			}
			std::cout << "Mission music save bytes: " << musicBytes << '\n';
		}
		else
			std::cout << "Mission music save absent; source MusicManager::Load uses fresh state\n";
		// SysMap::Load treats an absent SysMapSaveload/SysMap child as fresh state.
		DAFILEDESC systemMapDescriptor("\\SysMapSaveload\\SysMap");
		const HANDLE systemMap = mission->OpenChild(&systemMapDescriptor);
		if (systemMap != INVALID_HANDLE_VALUE)
		{
			const DWORD systemMapBytes = mission->GetFileSize(systemMap, nullptr);
			if (!mission->CloseHandle(systemMap) || systemMapBytes == 0xffffffffU)
			{
				std::cerr << "Mission system-map save inspection failed\n";
				return 2;
			}
			std::cout << "Mission system-map save bytes: " << systemMapBytes << '\n';
		}
		else
			std::cout << "Mission system-map save absent; source SysMap::Load uses fresh state\n";
		// MScript::Load distinguishes a fresh mission from saved script state.
		DAFILEDESC scriptDataDescriptor("\\Script\\GlobalData");
		const HANDLE scriptData = mission->OpenChild(&scriptDataDescriptor);
		if (scriptData != INVALID_HANDLE_VALUE)
		{
			const DWORD scriptBytes = mission->GetFileSize(scriptData, nullptr);
			if (!mission->CloseHandle(scriptData) || scriptBytes == 0xffffffffU)
			{
				std::cerr << "Mission script global-data inspection failed\n";
				return 2;
			}
			std::cout << "Mission script GlobalData bytes: " << scriptBytes << '\n';
		}
		else
			std::cout << "Mission script GlobalData absent; native script must initialize fresh state\n";
		// ObjectList::Load traverses these three nested DOS views before QuickLoad.
		struct ExpectedObject { const char * archetype; const char * type; const char * name; DWORD size; };
		constexpr ExpectedObject expected[] = {
			{"FAB!!S_Forger", "MT_QSHIPLOAD", "Forger", 12},
			{"GBOAT!!S_Taos", "MT_QSHIPLOAD", "Taos1", 12},
			{"GBOAT!!S_Taos", "MT_QSHIPLOAD", "Taos2", 12},
			{"PLATBUILDSUP!!S_Acropolis", "MT_PLATFORM_QLOAD", "Acropolis", 8},
		};
		if (!mission->SetCurrentDirectory("\\ObjectList\\QuickSave"))
		{
			std::cerr << "Mission QuickSave directory unavailable\n";
			return 2;
		}
		WIN32_FIND_DATA quickEntry{};
		const HANDLE quickSearch = mission->FindFirstFile("*.*", &quickEntry);
		bool hasForger = false, hasTaos = false, hasAcropolis = false;
		if (quickSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!(quickEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
				hasForger |= std::strcmp(quickEntry.cFileName, "FAB!!S_Forger") == 0;
				hasTaos |= std::strcmp(quickEntry.cFileName, "GBOAT!!S_Taos") == 0;
				hasAcropolis |= std::strcmp(quickEntry.cFileName, "PLATBUILDSUP!!S_Acropolis") == 0;
			} while (mission->FindNextFile(quickSearch, &quickEntry));
		}
		if (quickSearch == INVALID_HANDLE_VALUE ||
			mission->GetLastError() != ERROR_NO_MORE_FILES ||
			!mission->FindClose(quickSearch) ||
			!hasForger || !hasTaos || !hasAcropolis)
		{
			std::cerr << "Mission QuickSave archetype enumeration incomplete\n";
			return 2;
		}
		for (const ExpectedObject &object : expected)
		{
			DAFILEDESC archetypeDescriptor(object.archetype);
			archetypeDescriptor.lpImplementation = "DOS";
			archetypeDescriptor.dwDesiredAccess = GENERIC_READ;
			IFileSystem * archetypeRaw = nullptr;
			if (mission->CreateInstance(&archetypeDescriptor,
				reinterpret_cast<void **>(&archetypeRaw)) != GR_OK || !archetypeRaw)
			{
				std::cerr << "Mission QuickSave archetype directory unavailable\n";
				return 2;
			}
			FileOwner archetype(archetypeRaw, release);
			DAFILEDESC typeDescriptor(object.type);
			typeDescriptor.lpImplementation = "DOS";
			typeDescriptor.dwDesiredAccess = GENERIC_READ;
			IFileSystem * typeRaw = nullptr;
			if (archetype->CreateInstance(&typeDescriptor,
				reinterpret_cast<void **>(&typeRaw)) != GR_OK || !typeRaw)
			{
				std::cerr << "Mission QuickSave type directory unavailable\n";
				return 2;
			}
			FileOwner type(typeRaw, release);
			DAFILEDESC objectDescriptor(object.name);
			const HANDLE child = type->OpenChild(&objectDescriptor);
			if (child == INVALID_HANDLE_VALUE)
			{
				std::cerr << "Mission QuickSave object unavailable\n";
				return 2;
			}
			std::array<char, 16> bytes{};
			DWORD countRead = 0;
			const DWORD size = type->GetFileSize(child, nullptr);
			const bool readComplete = size == object.size &&
				type->ReadFile(child, bytes.data(), size, &countRead, nullptr) &&
				countRead == size;
			if (!type->CloseHandle(child) || !readComplete)
			{
				std::cerr << "Mission QuickSave object read incomplete\n";
				return 2;
			}
		}
		std::cout << "Mission QuickSave object records opened: 4\n";
		// Execute the native ISystemMap branch reached by Mission::Reload.
		ISystemMap *systemMapOwner = FrontierX::CreateNativeSystemMap();
		if (!systemMapOwner)
		{
			std::cerr << "Native system-map owner unavailable\n";
			return 2;
		}
		systemMapOwner->Load(mission.get());
		const bool mapFresh = systemMapOwner->GetMapTiming() == 0;
		systemMapOwner->Release();
		if (!mapFresh)
		{
			std::cerr << "Fresh mission system-map timing is not zero\n";
			return 2;
		}
		std::cout << "Native system-map fresh-load branch executed\n";
	}
	std::cout << "Mission UTF loader paths opened; ParseData bytes: " << parseSize
		<< "; global-data entries: " << count
		<< "; Globals bytes: " << globalsSize << '\n';
	return 0;
}

}

int main(int argc, char **argv)
{
    // ObjectList also collects text leaves such as gametypes.h. Those bytes
    // must not be loaded through BASIC_DATA's bool lvalue (UBSan regression).
    std::array<unsigned char, 8> editorBytes{};
    if (NativeArchetypeEditorFlag(editorBytes.data(), editorBytes.size())) return 1;
    for (unsigned char flag : {1, 13, 255})
    {
        editorBytes[4] = flag;
        if (!NativeArchetypeEditorFlag(editorBytes.data(), editorBytes.size()) ||
            editorBytes[4] != flag) return 1;
    }
    for (std::size_t size = 0; size <= 4; ++size)
        if (NativeArchetypeEditorFlag(editorBytes.data(), size)) return 1;
    if (NativeArchetypeEditorFlag(nullptr, 8)) return 1;
    alignas(std::max_align_t) std::array<unsigned char,
        3 * NativeArchetypeStorageAlignment> oddLeaves{};
    for (std::size_t offset = 0; offset < NativeArchetypeStorageAlignment; ++offset)
    {
        unsigned char *raw = oddLeaves.data() + offset;
        const std::size_t padding = NativeArchetypeBytePadding(raw);
        if (padding >= NativeArchetypeStorageAlignment ||
            reinterpret_cast<std::uintptr_t>(raw + padding) %
                NativeArchetypeStorageAlignment != 0) return 1;
        // Payload length remains odd; the following typed view needs padding.
        if (NativeArchetypeBytePadding(raw + padding + 5) == 0) return 1;
    }


	if (argc == 2 && std::string(argv[1]) == "--self-test")
		return RunSelfTest();
	if (argc == 4 && std::string(argv[1]) == "--mission-filesystem")
		return InspectMissionFilesystem(argv[2], argv[3]);
	if (argc <= 2)
	{
		const char *environmentPath = std::getenv("FRONTIERX_DATA_ROOT");
		const std::filesystem::path root = FrontierX::Platform::SelectDataRootPath(
			argc == 2 ? std::filesystem::path(argv[1]) : std::filesystem::path(),
			environmentPath != nullptr ? std::filesystem::path(environmentPath) : std::filesystem::path(),
			"Data");
		return Inspect(root);
	}

	std::cerr << "Usage: frontierx_data_root_probe [data-root]|--self-test|"
		"--mission-filesystem data-root mission-name\n";
	return 1;
}
