#include "FrontierX/LegacyFileTypes.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wsign-compare"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif
#include "CQTrace.h"
#define MAX_PLAYERS 8
#include <DGlobalData.h>
#include <DMusic.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include "NativeGlobalDataFormat.h"
#include "NativeAppGlobals.h"
#include "NativeMusicDataFormat.h"
#include "NativeAppSchema.h"
#include "NativeSolTrainSchema.h"
#include "NativeSolTrainStrings.h"
#include "Scripts/Sol_T/resource.h"

#include "DACOM.H"
#include "DataParser.h"
#include "FDump.h"
#include "Filesys.h"
#include "FrontierX/DataRoot.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "HeapObj.h"
#include "Symtable.h"
#include "ViewSymbolOperations.h"
#include "ViewCnst.h"
#include "Tokendef.h"

#include <cstdint>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

extern SymbolManager SymbolTable;
extern ICOManager * DACOM;
SYMBOL parse_memory(const char * memory);
void COMAPI CreateDataParser(SYMBOL symbol, IDataParser ** parser);
void RegisterViewConstructor(ICOManager * manager);

namespace
{
static_assert(sizeof(SONG) != 44, "Native music records must not be read as Win32 disk records");

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...)
{
    return 0;
}

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "view parser probe failed: " << message << '\n';
    return condition;
}

// Mission::ParseDefFile consumes the Globals.rc Data.i resource before reload.
// The generated-schema check cannot decide whether that repository source
// parses natively; this result decides whether a native resource binding can
// reuse it or must first repair a parser/restoration incompatibility.
bool ParseAppSchema(IViewConstructor2 * constructor,
                    const std::filesystem::path & path)
{
    std::error_code status;
    const auto length = std::filesystem::file_size(path, status);
    if (status || length == 0 || length > 4U * 1024U * 1024U) return false;
    std::ifstream source(path, std::ios::binary);
    std::vector<char> text(static_cast<std::size_t>(length) + 1U, '\0');
    if (!source.read(text.data(), static_cast<std::streamsize>(length)))
        return false;
    int embeddedLength = 0;
    const void *embedded = FrontierX::GetNativeAppSchema(embeddedLength);
    if (!embedded || embeddedLength != static_cast<int>(length) ||
        std::memcmp(embedded, text.data(), static_cast<std::size_t>(length)) != 0)
        return false;
    std::memcpy(text.data(), embedded, static_cast<std::size_t>(length));
    if (FrontierX::ParseNativeAppSchema(nullptr) ||
        constructor->ParseMemory("static_assert(1);") == GR_OK ||
        !FrontierX::ParseNativeAppSchema(constructor))
        return false;
    const HANDLE malformed = constructor->ParseNewMemory("static_assert(1);");
    if (malformed)
    {
        constructor->DestroySymbols(malformed);
        return false;
    }
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const SYMBOL global = constructor->GetSymbol(symbols, "MT_GlobalData");
    const SYMBOL record = ResolveViewSymbolType(global);
    const bool valid = record && record->size != 0U;
    if (valid)
        std::cout << "App Data.i parsed; MT_GlobalData bytes: "
                  << record->size << '\n';
    constructor->DestroySymbols(symbols);
    return valid;
}

// MScript::SetScriptData consumes the selected script's generated schema.
// This check decides whether native preprocessing can supply that contract.
bool ParseScriptSchema(IViewConstructor2 * constructor,
                       const std::filesystem::path & path)
{
    std::error_code status;
    const auto length = std::filesystem::file_size(path, status);
    if (status || length == 0 || length > 4U * 1024U * 1024U) return false;
    std::ifstream source(path, std::ios::binary);
    std::vector<char> text(static_cast<std::size_t>(length) + 1U, '\0');
    if (!source.read(text.data(), static_cast<std::streamsize>(length)))
        return false;
    std::size_t embeddedLength = 0;
    const void *embedded = FrontierX::GetNativeSolTrainSchema(embeddedLength);
    if (!embedded || embeddedLength != length ||
        std::memcmp(embedded, text.data(), static_cast<std::size_t>(length)) != 0)
        return false;
    std::memcpy(text.data(), embedded, static_cast<std::size_t>(length));
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const SYMBOL global = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "MissionData"));
    const SYMBOL movement = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "SolTrain_SelectAndMoveForger_Save"));
    const auto fieldOffset = [](SYMBOL record, const char * name) -> int {
        for (SYMBOL field = record ? record->datatype : nullptr;
             field; field = field->link)
            if (field->name && std::strcmp(field->name, name) == 0)
                return field->offset;
        return -1;
    };
    const bool valid = global && global->size == 292U &&
        fieldOffset(global, "Forger") == 168 &&
        fieldOffset(global, "Taos1") == 184 &&
        fieldOffset(global, "Taos2") == 200 &&
        fieldOffset(global, "Acropolis") == 216 &&
        movement && movement->size == 4U &&
        fieldOffset(movement, "state") == 0;
    const wchar_t *select = FrontierX::GetNativeSolTrainCountedString(
        IDS_SOLTRAIN_OBJECTIVE1);
    const wchar_t *move = FrontierX::GetNativeSolTrainCountedString(
        IDS_SOLTRAIN_OBJECTIVE2);
    const wchar_t *subtitle = FrontierX::GetNativeSolTrainCountedString(
        IDS_SOL_SUB_STMG03);
    const bool stringsValid = select && select[0] == 14 &&
        std::wmemcmp(select + 1, L"Select Forger.", 14) == 0 &&
        select[15] == 0 && move && move[0] == 53 &&
        std::wmemcmp(move + 1, L"Right-click", 11) == 0 &&
        subtitle && subtitle[0] == 219 &&
        std::wmemcmp(subtitle + 1, L"Elan:", 5) == 0 &&
        !FrontierX::GetNativeSolTrainCountedString(0);
    if (valid)
        std::cout << "Sol_T script schema parsed; MissionData bytes: "
                  << global->size << '\n';
    constructor->DestroySymbols(symbols);
    return valid && stringsValid;
}

bool ParseMissionSchema(IViewConstructor2 * constructor,
                        const std::filesystem::path & root, const char * missionName)
{
    using FileOwner = std::unique_ptr<IFileSystem, void (*)(IFileSystem *)>;
    const auto release = [](IFileSystem *file) { if (file) file->Release(); };
    FileOwner campaign(nullptr, release);
    FileOwner mission(nullptr, release);
    for (const auto & entry : FrontierX::Platform::InspectDataRoot(root))
    {
        if (entry.expectedPath != "Maps/Campaign") continue;
        IFileSystem * opened = nullptr;
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
        break;
    }
    if (!campaign) return false;
    DAFILEDESC missionDescriptor(missionName);
    IFileSystem * opened = nullptr;
    if (campaign->CreateInstance(&missionDescriptor,
            reinterpret_cast<void **>(&opened)) != GR_OK || !opened) return false;
    mission.reset(opened);
    DAFILEDESC parseDescriptor("\\ParseData\\Data.h");
    const HANDLE parseFile = mission->OpenChild(&parseDescriptor);
    if (parseFile == INVALID_HANDLE_VALUE) return false;
    const DWORD length = mission->GetFileSize(parseFile, nullptr);
    if (length == 0 || length > 4U * 1024U * 1024U)
    {
        mission->CloseHandle(parseFile);
        return false;
    }
    std::vector<char> text(length + 1U, '\0');
    DWORD read = 0;
    const bool loaded = mission->ReadFile(parseFile, text.data(), length,
        &read, nullptr) && read == length && mission->CloseHandle(parseFile);
    if (!loaded) return false;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const SYMBOL global = constructor->GetSymbol(symbols, "MT_GlobalData");
    const SYMBOL record = ResolveViewSymbolType(global);
    const SYMBOL music = constructor->GetSymbol(symbols, "MT_MUSIC_DATA");
    const SYMBOL musicRecord = ResolveViewSymbolType(music);
    const SYMBOL song = ResolveViewSymbolType(constructor->GetSymbol(symbols, "SONG"));
    const auto fieldOffset = [record](const char * name) -> int {
        for (SYMBOL field = record ? record->datatype : nullptr;
             field; field = field->link)
            if (field->name && std::strcmp(field->name, name) == 0)
                return field->offset;
        return -1;
    };
    bool hasBaseTerrainMap = false;
    U32 missionBuildVersion = 0;
    const auto songOffset = [song](const char *name) -> int {
        for (SYMBOL field = song ? song->datatype : nullptr; field; field = field->link)
            if (field->name && std::strcmp(field->name, name) == 0)
                return field->offset;
        return -1;
    };
    bool valid = record && record->size == FrontierX::GlobalDataDiskSize &&
        musicRecord && musicRecord->size == 176 &&
        song && song->size == 44 && songOffset("handle") == 4 &&
        songOffset("volume") == 8 && songOffset("filename") == 12 &&
        fieldOffset("gameDescription") == 0 &&
        fieldOffset("playerNames") == 64 &&
        fieldOffset("playerAssignments") == 576 &&
        fieldOffset("gameSettings") == 3160;
    if (valid && mission->SetCurrentDirectory("\\MT_MUSIC_DATA"))
    {
        DAFILEDESC musicDescriptor("Music");
        const HANDLE musicFile = mission->OpenChild(&musicDescriptor);
        if (musicFile != INVALID_HANDLE_VALUE)
        {
            FrontierX::MusicDataDiskBytes bytes = {};
            DWORD count = 0;
            valid = mission->GetFileSize(musicFile, nullptr) == bytes.size() &&
                mission->ReadFile(musicFile, bytes.data(), bytes.size(), &count, nullptr) &&
                count == bytes.size();
            valid = mission->CloseHandle(musicFile) && valid;
            if (valid)
            {
                MT_MUSIC_DATA songs = {};
                FrontierX::MusicDataDiskBytes correlated = {};
                FrontierX::MusicDataDiskBytes roundTrip = {};
                constructor->CorrelateSymbol(music, bytes.data(), music,
                                             correlated.data());
                valid = FrontierX::DecodeMusicData(correlated, songs) &&
                    FrontierX::EncodeMusicData(songs, roundTrip);
                for (std::size_t index = 0; index < NUM_SONGS && valid; ++index)
                {
                    valid = songs[index].handle == nullptr &&
                        (correlated[index * FrontierX::MusicSongDiskSize] & 3U) ==
                        (roundTrip[index * FrontierX::MusicSongDiskSize] & 3U) &&
                        std::memcmp(roundTrip.data() + index * FrontierX::MusicSongDiskSize + 4,
                                    "\0\0\0\0", 4) == 0 &&
                        std::memcmp(correlated.data() + index * FrontierX::MusicSongDiskSize + 8,
                                    roundTrip.data() + index * FrontierX::MusicSongDiskSize + 8,
                                    36) == 0;
                }
                if (valid)
                {
                    auto malformed = correlated;
                    std::memset(malformed.data() + 12, 'x', 32);
                    valid = !FrontierX::DecodeMusicData(malformed, songs);
                }
            }
        }
    }
    if (valid && mission->SetCurrentDirectory("\\MT_GlobalData"))
    {
        DAFILEDESC globalDescriptor("Globals");
        const HANDLE globalFile = mission->OpenChild(&globalDescriptor);
        if (globalFile != INVALID_HANDLE_VALUE)
        {
            FrontierX::GlobalDataDiskBytes bytes = {};
            DWORD count = 0;
            valid = mission->GetFileSize(globalFile, nullptr) == bytes.size() &&
                mission->ReadFile(globalFile, bytes.data(), bytes.size(), &count, nullptr) &&
                count == bytes.size();
            valid = mission->CloseHandle(globalFile) && valid;
            if (valid)
            {
                MT_GlobalData runtime = {};
                FrontierX::GlobalDataDiskBytes roundTrip = {};
                FrontierX::GlobalDataDiskBytes correlated = {};
                constructor->CorrelateSymbol(global, bytes.data(), global,
                                             correlated.data());
                FrontierX::DecodeGlobalData(correlated, runtime);
                hasBaseTerrainMap = runtime.baseTerrainMap.string[0] != '\0';
                missionBuildVersion = runtime.gameSettings.version;
                valid = FrontierX::EncodeGlobalData(runtime, roundTrip) &&
                    correlated == roundTrip;
                valid = valid &&
                    std::strncmp(runtime.scriptLibrary.string, "Sol_T.dll",
                                 sizeof(runtime.scriptLibrary.string)) == 0 &&
                    runtime.updateCount == 0;
                if (valid)
                {
                    runtime.gameDescription.string[0] = static_cast<wchar_t>(0x10000);
                    valid = !FrontierX::EncodeGlobalData(runtime, roundTrip) &&
                        correlated == roundTrip;
                }
            }
        }
        else valid = false;
    }
    else valid = false;
    if (valid)
        std::cout << "Mission ParseData/global record round-tripped; bytes: "
                  << global->size << "; terrain source: "
                  << (hasBaseTerrainMap ? "separate map" : "mission file")
                  << "; saved build version: " << missionBuildVersion << '\n';
    constructor->DestroySymbols(symbols);
    return valid;
}

BOOL32 COMAPI ReportBlock(IHeap * heap, void * block, U32 flags, void *)
{
    if (flags & DAHEAPFLAG_ALLOCATED_BLOCK)
        std::cerr << "remaining parser allocation: " << heap->GetBlockSize(block)
                  << " bytes; " << heap->GetBlockMessage(block) << '\n';
    return TRUE;
}

class FieldCapture final : public IParserCallback
{
public:
    BOOL32 COMAPI VarInstance(IDataParser * nested,
                              const VARIABLEDESC & field) override
    {
        constexpr const char * names[]{"first", "pointer", "second"};
        constexpr U32 offsets[]{0U, 4U, 8U};
        if (count >= 3U || !field.varName ||
            std::strcmp(field.varName, names[count]) != 0 ||
            field.offset != offsets[count] || field.size != 4U || nested)
            valid = false;
        ++count;
        return TRUE;
    }

    U32 count = 0U;
    bool valid = true;
};

class NestedCapture final : public IParserCallback
{
public:
    BOOL32 COMAPI VarInstance(IDataParser * nested,
                              const VARIABLEDESC & field) override
    {
        if (count == 0U)
        {
            FieldCapture fields;
            valid = field.varName && std::strcmp(field.varName, "item") == 0 &&
                field.offset == 0U && field.size == 12U && nested &&
                nested->Enumerate(&fields) == TRUE && fields.valid &&
                fields.count == 3U;
        }
        else if (count == 1U)
            valid = valid && field.varName &&
                std::strcmp(field.varName, "tail") == 0 &&
                field.offset == 12U && field.size == 4U && !nested;
        else
            valid = false;
        ++count;
        return TRUE;
    }

    U32 count = 0U;
    bool valid = true;
};

class MigrationCapture final : public IParserCallback
{
public:
    BOOL32 COMAPI VarInstance(IDataParser * nested,
                              const VARIABLEDESC & field) override
    {
        constexpr const char * names[]{"beta", "alpha"};
        if (count >= 2U || !field.varName ||
            std::strcmp(field.varName, names[count]) != 0 ||
            field.offset != count * 4U || field.size != 4U || nested)
            valid = false;
        ++count;
        return TRUE;
    }

    U32 count = 0U;
    bool valid = true;
};
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

ICOManager * DACOM = nullptr;

int main(int argc, char ** argv)
{
    const bool appSchema = argc == 3 && std::strcmp(argv[1], "--app-schema") == 0;
    const bool scriptSchema = argc == 3 &&
        std::strcmp(argv[1], "--script-schema") == 0;
    const bool missionSchema = argc == 4 &&
        std::strcmp(argv[1], "--mission-schema") == 0 &&
        !std::strchr(argv[3], '/') && !std::strchr(argv[3], '\\');
    if (argc != 1 && !appSchema && !scriptSchema && !missionSchema)
    {
        std::cerr << "Usage: frontierx_view_parser_probe [--app-schema data.i | "
                     "--script-schema DataDef.i | "
                     "--mission-schema data-root mission-name]\n";
        return 1;
    }
    char buildVersionText[32] = {};
    if (!Expect(GetBuildVersion() == 0x020005c5U &&
                GetBuildVersion(buildVersionText, sizeof(buildVersionText)) == 13U &&
                std::strcmp(buildVersionText, "02.00.05.2501") == 0,
                "source VERSIONINFO build-number conversion")) return 1;
    if (!Expect(InitializeDAHeap(1024 * 1024, 0, DAHEAPFLAG_NOMSGS) == TRUE,
                "native heap initialization")) return 1;
    DACOM = DACOM_Acquire();
    SYMBOL reserved = SymbolTable.set_reserved();
    if (!Expect(reserved != nullptr, "reserved parser types")) return 1;
    reserved->AddRef();

    SYMBOL longType = SymbolTable.get_symbol(const_cast<char *>("long"));
    if (!Expect(longType && longType->size == 4 && longType->alignment == 4,
                "Win32 long parser width and alignment")) return 1;

    SYMBOL parsed = parse_memory(
        "struct NativeLayout { long first; void *pointer; int second; };"
        "struct NestedLayout { NativeLayout item; int tail; };"
        "struct VariableLayout { int count; char data[0]; };");
    if (!Expect(parsed != nullptr, "production record parser")) return 1;
    SYMBOL record = parsed;
    while (record && (!record->name ||
                      std::strcmp(record->name, "NativeLayout") != 0))
        record = record->link;
    if (!Expect(record && record->size == 12 && record->alignment == 4,
                "Win32 record size and alignment")) return 1;
    SYMBOL first = record->datatype;
    SYMBOL pointer = first ? first->link : nullptr;
    SYMBOL second = pointer ? pointer->link : nullptr;
    if (!Expect(first && pointer && second && first->offset == 0 &&
                first->size == 4 && pointer->offset == 4 &&
                pointer->size == 4 && second->offset == 8 && second->size == 4,
                "Win32 long, pointer and following field offsets")) return 1;

    IDataParser * dataParser = nullptr;
    CreateDataParser(nullptr, &dataParser);
    if (!Expect(!dataParser, "null parser symbol rejected")) return 1;
    CreateDataParser(record, &dataParser);
    FieldCapture fields;
    if (!Expect(dataParser && dataParser->Enumerate(&fields) == TRUE &&
                fields.valid && fields.count == 3U,
                "production IDataParser field enumeration")) return 1;
    dataParser->Release();

    SYMBOL nestedRecord = parsed;
    while (nestedRecord && (!nestedRecord->name ||
                            std::strcmp(nestedRecord->name, "NestedLayout") != 0))
        nestedRecord = nestedRecord->link;
    IDataParser * nestedParser = nullptr;
    if (!Expect(nestedRecord && nestedRecord->size == 16 &&
                nestedRecord->alignment == 4,
                "nested Win32 record layout")) return 1;
    if (!Expect(EqualViewSymbols(record, record) &&
                !EqualViewSymbols(record, nestedRecord) &&
                !HasVariableViewSymbolSize(record) &&
                !HasVariableViewSymbolSize(nestedRecord),
                "viewer schema comparison and fixed-size decisions")) return 1;
    SYMBOL variableRecord = parsed;
    while (variableRecord && (!variableRecord->name ||
                              std::strcmp(variableRecord->name,
                                          "VariableLayout") != 0))
        variableRecord = variableRecord->link;
    if (!Expect(variableRecord && HasVariableViewSymbolSize(variableRecord),
                "variable trailing array schema decision")) return 1;
    CreateDataParser(nestedRecord, &nestedParser);
    NestedCapture nestedFields;
    if (!Expect(nestedParser && nestedParser->Enumerate(&nestedFields) == TRUE &&
                nestedFields.valid && nestedFields.count == 2U,
                "production nested IDataParser enumeration")) return 1;
    nestedParser->Release();

    while (parsed)
    {
        SYMBOL next = parsed->link;
        parsed->Release();
        parsed = next;
    }
    while (reserved)
    {
        SYMBOL next = reserved->link;
        reserved->Release();
        reserved = next;
    }
    SymbolTable.free();
    if (HEAP->GetAvailableMemory() != HEAP->GetHeapSize())
    {
        HEAP->EnumerateBlocks(ReportBlock, nullptr);
        if (!Expect(false, "parser heap balanced after release")) return 1;
    }
    RegisterViewConstructor(DACOM);
    DACOMDESC viewDescriptor("IViewConstructor");
    IViewConstructor * view = nullptr;
    if (!Expect(DACOM->CreateInstance(&viewDescriptor,
                    reinterpret_cast<void **>(&view)) == GR_OK && view,
                "production view constructor factory")) return 1;
    IViewConstructor2 * view2 = nullptr;
    if (!Expect(view->QueryInterface("IViewConstructor2",
                    reinterpret_cast<void **>(&view2)) == GR_OK && view2,
                "production IViewConstructor2 query")) return 1;
    if (appSchema && !Expect(ParseAppSchema(view2, argv[2]),
                             "repository App Data.i schema")) return 1;
    if (scriptSchema && !Expect(ParseScriptSchema(view2, argv[2]),
                                "selected Sol_T script schema")) return 1;
    if (missionSchema && !Expect(ParseMissionSchema(view2, argv[2], argv[3]),
                            "external mission ParseData schema")) return 1;
    HANDLE oldSymbols = view2->ParseNewMemory(
        "struct MigrationRecord { int alpha; int beta; };");
    HANDLE newSymbols = view2->ParseNewMemory(
        "struct MigrationRecord { int beta; int alpha; };");
    SYMBOL oldRecord = view2->GetSymbol(oldSymbols, "MigrationRecord");
    SYMBOL newRecord = view2->GetSymbol(newSymbols, "MigrationRecord");
    if (!Expect(oldSymbols && newSymbols && oldRecord && newRecord &&
                oldRecord->size == 8 && newRecord->size == 8 &&
                view2->IsEqual(oldRecord, oldRecord) &&
                !view2->IsEqual(oldRecord, newRecord),
                "changed schema from production IViewConstructor2")) return 1;
    DPARSERDESC parserDescriptor;
    parserDescriptor.symbol = newRecord;
    IDataParser * factoryParser = nullptr;
    MigrationCapture migrationFields;
    if (!Expect(view->CreateInstance(&parserDescriptor,
                    reinterpret_cast<void **>(&factoryParser)) == GR_OK &&
                factoryParser &&
                factoryParser->Enumerate(&migrationFields) == TRUE &&
                migrationFields.valid && migrationFields.count == 2U,
                "production constructor IDataParser factory")) return 1;
    factoryParser->Release();
    DACOMDESC unavailableViewer("IViewer");
    void * viewer = nullptr;
    if (!Expect(view->CreateInstance(&unavailableViewer, &viewer) ==
                    GR_INTERFACE_UNSUPPORTED && !viewer,
                "native GUI factory rejection")) return 1;
    std::uint32_t oldData[]{0x11111111U, 0x22222222U};
    std::uint32_t newData[]{0xdeadbeefU, 0xdeadbeefU};
    view2->CorrelateSymbol(oldRecord, oldData, newRecord, newData);
    if (!Expect(newData[0] == oldData[1] && newData[1] == oldData[0],
                "production field correlation by name")) return 1;
    Symbol oldBit;
    oldBit.kind = BASICTYPE;
    oldBit.basicdt = INTEGER;
    oldBit.size = 4;
    oldBit.bit_offset = 29;
    oldBit.bit_count = 5;
    oldBit.modifiers = MODIFIER_UNSIGNED;
    Symbol newBit = oldBit;
    newBit.bit_offset = 3;
    std::uint64_t oldBits = std::uint64_t(0x15U) << 29;
    std::uint64_t newBits = ~std::uint64_t(0);
    view2->CorrelateSymbol(&oldBit, &oldBits, &newBit, &newBits);
    if (!Expect((newBits & 0xffU) == (0x15U << 3),
                "production unsigned cross-word bitfield correlation")) return 1;
    oldBit.modifiers = 0;
    oldBit.bit_offset = 0;
    newBit.bit_offset = 0;
    newBit.bit_count = 8;
    oldBits = 0x1dU;
    newBits = 0;
    view2->CorrelateSymbol(&oldBit, &oldBits, &newBit, &newBits);
    if (!Expect((newBits & 0xffU) == 0xfdU,
                "production signed bitfield correlation")) return 1;
    Symbol oldEntry;
    oldEntry.kind = CONSTSYM;
    oldEntry.name = const_cast<char *>("ready");
    oldEntry.constval.intnum = 1;
    Symbol newEntry = oldEntry;
    newEntry.constval.intnum = 7;
    Symbol oldEnum;
    oldEnum.kind = ENUMSYM;
    oldEnum.name = const_cast<char *>("State");
    oldEnum.size = 4;
    oldEnum.datatype = &oldEntry;
    Symbol newEnum = oldEnum;
    newEnum.datatype = &newEntry;
    std::uint32_t oldEnumValue = 1;
    std::uint32_t newEnumValue = 0;
    view2->CorrelateSymbol(&oldEnum, &oldEnumValue,
                           &newEnum, &newEnumValue);
    if (!Expect(newEnumValue == 7,
                "production enum correlation by value name")) return 1;
    HANDLE sameSymbols = view2->ParseNewMemory(
        "struct MigrationRecord { int alpha; int beta; };");
    SYMBOL sameRecord = view2->GetSymbol(sameSymbols, "MigrationRecord");
    if (!Expect(sameSymbols && sameRecord &&
                view2->IsEqual(oldRecord, sameRecord),
                "same schema direct-copy decision across parsed tables")) return 1;
    view2->DestroySymbols(sameSymbols);
    HANDLE oldNestedSymbols = view2->ParseNewMemory(
        "struct Inner { int left; int right; };"
        "struct ArrayRecord { int marker; Inner inner[2]; int tail; };");
    HANDLE newNestedSymbols = view2->ParseNewMemory(
        "struct Inner { int right; int left; };"
        "struct ArrayRecord { int tail; Inner inner[2]; int marker; };");
    SYMBOL oldArrayRecord = view2->GetSymbol(oldNestedSymbols, "ArrayRecord");
    SYMBOL newArrayRecord = view2->GetSymbol(newNestedSymbols, "ArrayRecord");
    if (!Expect(oldNestedSymbols && newNestedSymbols && oldArrayRecord &&
                newArrayRecord && oldArrayRecord->size == 24 &&
                newArrayRecord->size == 24 &&
                !view2->IsEqual(oldArrayRecord, newArrayRecord),
                "nested array schema from production parser")) return 1;
    std::uint32_t oldArrayData[]{10U, 11U, 12U, 13U, 14U, 15U};
    std::uint32_t newArrayData[6]{};
    view2->CorrelateSymbol(oldArrayRecord, oldArrayData,
                           newArrayRecord, newArrayData);
    constexpr std::uint32_t expectedArrayData[]{15U, 12U, 11U, 14U, 13U, 10U};
    if (!Expect(std::memcmp(newArrayData, expectedArrayData,
                            sizeof(expectedArrayData)) == 0,
                "production nested array and record correlation")) return 1;
    view2->DestroySymbols(newNestedSymbols);
    view2->DestroySymbols(oldNestedSymbols);
    view2->DestroySymbols(newSymbols);
    view2->DestroySymbols(oldSymbols);
    view2->Release();
    if (!Expect(DACOM->UnregisterComponent(view, "IViewConstructor") == GR_OK,
                "production view constructor unregister")) return 1;
    view->Release();
    SymbolTable.free();
    if (HEAP->GetAvailableMemory() != HEAP->GetHeapSize())
    {
        HEAP->EnumerateBlocks(ReportBlock, nullptr);
        if (!Expect(false, "constructor heap balanced after release")) return 1;
    }
    std::cout << "production view parser preserved Win32 long record layout\n";
    return 0;
}
