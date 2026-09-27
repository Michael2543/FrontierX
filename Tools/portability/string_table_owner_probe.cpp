#include "AppStringTableLoad.h"
#include "DACOM.H"
#include "FDump.h"
#include "FrontierX/LegacyLanguageIds.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/StringTableFileText.h"
#include "FrontierX/StringTableRegistration.h"
#include "HeapObj.h"
#include "SaveLoad.h"
#include "StringTable.h"
#include "tinyxml.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace
{
int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }
bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "string table owner probe failed: " << message << '\n';
    return condition;
}

bool WriteFixture(const std::filesystem::path & path, bool malformedSecond = false)
{
    std::vector<unsigned char> data;
    if (!FrontierX::EncodeStringTableFileText(L"Hello \u03a9", data)) return false;
    const std::vector<unsigned char> invalidText = {0x41, 0x00};
    std::string names("/", 2);
    auto addName = [&](const char * name) {
        const U32 offset = static_cast<U32>(names.size());
        names.append(name, std::strlen(name) + 1);
        return offset;
    };
    const U32 table = addName("IStringTable");
    const U32 language = addName("LANGUAGE.LANG_ENGLISH.SUBLANG_ENGLISH_US");
    const U32 key = addName(malformedSecond ? "Added" : "Greeting");
    const U32 broken = malformedSecond ? addName("Broken") : 0;
    std::vector<unsigned char> bytes;
    auto word = [&](U32 value) {
        for (unsigned shift = 0; shift < 32; shift += 8)
            bytes.push_back(static_cast<unsigned char>(value >> shift));
    };
    constexpr U32 directory = 56;
    constexpr U32 record = 44;
    const U32 namesOffset = directory + (malformedSecond ? 5 : 4) * record;
    const U32 dataOffset = namesOffset + static_cast<U32>(names.size());
    for (U32 value : {0x20465455U, 0x101U, directory,
                      (malformedSecond ? 5U : 4U) * record,
                      0U, record, namesOffset, static_cast<U32>(names.size()),
                      static_cast<U32>(names.size()), dataOffset,
                      static_cast<U32>(data.size() +
                          (malformedSecond ? invalidText.size() : 0U)),
                      0U, 0U, 0U}) word(value);
    auto entry = [&](U32 next, U32 name, U32 flags, U32 child, U32 size) {
        for (U32 value : {next, name, flags, 0U, child, size, size, size,
                          0U, 0U, 0U}) word(value);
    };
    entry(0, 0, 0x10, record, 0);
    entry(0, table, 0x10, 2 * record, 0);
    entry(0, language, 0x10, 3 * record, 0);
    entry(malformedSecond ? 4 * record : 0, key, 0, 0,
          static_cast<U32>(data.size()));
    if (malformedSecond)
        entry(0, broken, 0, static_cast<U32>(data.size()),
              static_cast<U32>(invalidText.size()));
    bytes.insert(bytes.end(), names.begin(), names.end());
    bytes.insert(bytes.end(), data.begin(), data.end());
    if (malformedSecond)
        bytes.insert(bytes.end(), invalidText.begin(), invalidText.end());
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    return file.good();
}

// Mission::LoadBriefing opens a UTF mission inside Maps/Campaign. The existing
// StringTable fixture supplies the nested payload and the real load consumer.
bool WrapMissionFixture(const std::filesystem::path & path)
{
    std::ifstream input(path, std::ios::binary);
    const std::vector<unsigned char> payload((std::istreambuf_iterator<char>(input)), {});
    if (payload.empty()) return false;
    input.close();
    const char names[] = "/\0Campaign\0Training.dmission\0Broken.dmission\0";
    constexpr U32 record = 44;
    constexpr U32 namesOffset = 56 + 4 * record;
    std::vector<unsigned char> bytes;
    auto word = [&](U32 value) {
        for (unsigned shift = 0; shift < 32; shift += 8)
            bytes.push_back(static_cast<unsigned char>(value >> shift));
    };
    for (U32 value : {0x20465455U, 0x101U, 56U, 4 * record, 0U, record,
                      namesOffset, U32(sizeof(names)), U32(sizeof(names)),
                      namesOffset + U32(sizeof(names)), U32(payload.size()) + 4U, 0U, 0U, 0U})
        word(value);
    auto entry = [&](U32 next, U32 name, U32 flags, U32 child, U32 size) {
        for (U32 value : {next, name, flags, 0U, child, size, size, size, 0U, 0U, 0U}) word(value);
    };
    entry(0, 0, 0x10, record, 0);
    entry(0, 2, 0x10, 2 * record, 0);
    entry(3 * record, 11, 0, 0, U32(payload.size()));
    entry(0, 29, 0, U32(payload.size()), 4);
    bytes.insert(bytes.end(), names, names + sizeof(names));
    bytes.insert(bytes.end(), payload.begin(), payload.end());
    bytes.insert(bytes.end(), {'U', 'T', 'F', ' '});
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char *>(bytes.data()), std::streamsize(bytes.size()));
    return output.good();
}

struct Enumeration : IStringEnum
{
    void EnumStringInfo(StringInfo & item) override
    {
        ++count;
        id = item.idString;
    }
    unsigned count = 0;
    U32 id = 0;
};
}

extern "C" { DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic; }

int main(int argc, char ** argv)
{
    if (!Expect(argc == 2, "fixture path argument") ||
        !Expect(InitializeDAHeap(1024 * 1024, 0, DAHEAPFLAG_NOMSGS) == TRUE,
                "heap initialization")) return 1;
    ICOManager * manager = DACOM_Acquire();
    if (!Expect(manager && RegisterTheStringTable(manager), "factory registration")) return 1;
    AGGDESC descriptor("IStringTable");
    IStringTable * table = nullptr;
    if (!Expect(manager->CreateInstance(&descriptor,
                    reinterpret_cast<void **>(&table)) == GR_OK && table,
                "production aggregate creation")) return 1;
    ISaverLoader * loader = nullptr;
    if (!Expect(table->QueryInterface(IID_ISaverLoader,
                    reinterpret_cast<void **>(&loader)) == GR_OK && loader,
                "production saver/loader interface")) return 1;
    if (!Expect(table->GetLanguage() == 0x409, "native default language")) return 1;
    wchar_t local[] = L"Local";
    const U32 localId = table->NewString(local, "Local", 0x409);
    if (!Expect(localId == 0x10000 &&
                std::wcscmp(table->GetStringByTag("Local"), L"Local") == 0 &&
                table->GetStringIdByTag("Local") == localId &&
                std::strcmp(table->GetStringTag(localId), "Local") == 0,
                "production lookup by tag and ID")) return 1;
    Enumeration enumeration;
    table->EnumerateStringInfo(enumeration);
    if (!Expect(enumeration.count == 1 && enumeration.id == localId,
                "production enumeration ID width")) return 1;

    wchar_t french[] = L"Bonjour";
    table->NewString(french, "Greeting", 0x40c);
    if (!Expect(table->SetLanguage(0x40c) &&
                std::wcscmp(table->GetStringByTag("Greeting"), L"Bonjour") == 0,
                "second language selection")) return 1;

    TiXmlDocument document;
    if (!Expect(loader->Save(document), "XML save")) return 1;
    table->Reset();
    if (!Expect(loader->Load(document) && table->SetLanguage(0x40c) &&
                std::wcscmp(table->GetStringByTag("Greeting"), L"Bonjour") == 0 &&
                table->SetLanguage(0x409) &&
                std::wcscmp(table->GetStringByTag("Local"), L"Local") == 0,
                "XML language parsing and two-dictionary load")) return 1;
    const wchar_t * originalLocal = table->GetStringByTag("Local");

    const std::filesystem::path path(argv[1]);
    if (!Expect(WriteFixture(path) && WrapMissionFixture(path),
                "synthetic nested mission UTF fixture")) return 1;
    IFileSystem * archive = nullptr;
    std::string error;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(path, &archive, error) && archive,
                "native UTF archive open")) return 1;
    IFileSystem * campaign = nullptr;
    DAFILEDESC campaignDescriptor("Campaign");
    if (!Expect(archive->CreateInstance(&campaignDescriptor,
                    reinterpret_cast<void **>(&campaign)) == GR_OK && campaign,
                "production campaign directory contract")) return 1;
    DAFILEDESC badMission("Broken.dmission");
    void * bad = nullptr;
    if (!Expect(campaign->CreateInstance(&badMission, &bad) != GR_OK && !bad,
                "malformed nested UTF is not accepted as a plain file")) return 1;
    DAFILEDESC missionDescriptor("Training.dmission");
    missionDescriptor.lpImplementation = "DOS";
    IFileSystem * rawMission = nullptr;
    if (!Expect(campaign->CreateInstance(&missionDescriptor,
                    reinterpret_cast<void **>(&rawMission)) == GR_OK && rawMission &&
                rawMission->GetFileSize(0, nullptr) > 56 &&
                !rawMission->SetCurrentDirectory("IStringTable"),
                "explicit DOS returns raw mission bytes")) return 1;
    rawMission->Release();
    missionDescriptor.lpImplementation = nullptr;
    IFileSystem * mission = nullptr;
    if (!Expect(campaign->CreateInstance(&missionDescriptor,
                    reinterpret_cast<void **>(&mission)) == GR_OK && mission,
                "production mission factory opens nested UTF")) return 1;
    campaign->Release();
    archive->Release();
    archive = mission;
    std::filesystem::remove(path);
    DAFILEDESC rootDescriptor = "\\";
    rootDescriptor.lpImplementation = "DOS";
    rootDescriptor.dwDesiredAccess = GENERIC_READ;
    rootDescriptor.dwShareMode = 0;
    rootDescriptor.dwCreationDistribution = OPEN_EXISTING;
    rootDescriptor.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    void * rejected = nullptr;
    if (!Expect(archive->CreateInstance(&rootDescriptor, &rejected) != GR_OK &&
                rejected == nullptr, "read-only DOS child contract")) return 1;
    const bool loaded = FrontierX::LoadScriptStringTable(*archive, *table);
    archive->Release();
    std::filesystem::remove(path);
    if (!Expect(loaded && table->GetStringByTag("Local") == originalLocal &&
                table->GetStringByTag("Greeting") &&
                std::wcscmp(table->GetStringByTag("Greeting"), L"Hello \u03a9") == 0,
                "production UTF archive load and Unicode text")) return 1;
    const wchar_t * originalEnglish = table->GetStringByTag("Greeting");
    if (!Expect(table->SetLanguage(0x40c), "active language before failed load")) return 1;
    const wchar_t * originalFrench = table->GetStringByTag("Greeting");
    const std::filesystem::path invalidPath = path.string() + ".invalid";
    if (!Expect(WriteFixture(invalidPath, true), "malformed UTF archive fixture")) return 1;
    IFileSystem * invalidArchive = nullptr;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(invalidPath, &invalidArchive,
                    error) && invalidArchive,
                "malformed UTF archive open")) return 1;
    const bool rejectedLoad =
        !FrontierX::LoadScriptStringTable(*invalidArchive, *table);
    invalidArchive->Release();
    std::filesystem::remove(invalidPath);
    if (!Expect(rejectedLoad && table->GetLanguage() == 0x40c &&
                table->GetStringByTag("Greeting") == originalFrench &&
                std::wcscmp(table->GetStringByTag("Greeting"), L"Bonjour") == 0 &&
                table->SetLanguage(0x409) && !table->GetStringByTag("Added") &&
                !table->GetStringByTag("Broken") &&
                table->GetStringByTag("Greeting") == originalEnglish &&
                std::wcscmp(table->GetStringByTag("Greeting"), L"Hello \u03a9") == 0,
                "failed archive load preserves the existing dictionary")) return 1;
    if (!Expect(!table->SetLanguage(0x407) && table->SetLanguage(0x409),
                "dictionary selection")) return 1;
    loader->Release();
    table->Release();
    std::cout << "production StringTable XML and UTF archive paths passed\n";
    return 0;
}
