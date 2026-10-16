#include "FrontierX/LegacyFileTypes.h"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wkeyword-macro"
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
#include <DMBaseData.h>
#include <DMovieCamera.h>
#include <DVLaunch.h>
#include <DCloakLauncher.h>
#include <DShipSave.h>
#include "NetVector.h"
#define _ADB
#include <DJumpPlat.h>
#include <DGunPlat.h>
#include <DWeapon.h>
#include <DSpecial.h>
#include <DRepairPlat.h>
#include <DSpaceship.h>
#include <DSupplyShip.h>
#include <DSupplyShipSave.h>
#include <DFighter.h>
#include <DFighterWing.h>
#include <DReconLaunch.h>
#include <DPingLauncher.h>
#include <DTalorianLauncher.h>
#include <DFlagship.h>
#include <DTroopship.h>
#include <DWormholeLauncher.h>
#include <DMultiCloakLauncher.h>
#include <DEndGame.h>
#include <DConfirm.h>
#undef _ADB
#undef mutable
#include <DMenu1.h>
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
#include "Scripts/Script02T/Script01T/NativeScript01Resources.h"
#include "Scripts/Script02T/NativeScript02Resources.h"
#include "Scripts/Script03T/NativeScript03Resources.h"
#include "Scripts/Script04T/NativeScript04Resources.h"
#include "Scripts/Script05T/NativeScript05Resources.h"
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
#include "M3SemanticWriter.h"
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

static_assert(sizeof(BT_SUPPLYSHIP_DATA) == 652 &&
              sizeof(SUPPLYSHIP_SAVELOAD) == 468 &&
              sizeof(BASE_SUPPLYSHIP_SAVELOAD) == 40,
              "Original mobile supply ship archetype/save extents");
static_assert(sizeof(BT_BUILDERSHIP_DATA) == 192,
              "Original repair/build drone archetype disk extent and fields");
static_assert(sizeof(GUNPLAT_SAVELOAD) == 432 && sizeof(BASE_GUNPLAT_SAVELOAD) == 40,
              "Original gun-platform persistent record extent");
static_assert(sizeof(REPAIRPLAT_SAVELOAD) == 476 && sizeof(BASE_REPAIRPLAT_SAVELOAD) == 36,
              "Original repair-platform persistent record extent");
static_assert(sizeof(MASS_DISRUPTOR_SAVELOAD) == 76 && sizeof(BASE_MASS_DISRUPTOR_SAVELOAD) == 36,
              "Original Mass Disruptor persistent record extent");
static_assert(sizeof(BEAM_SAVELOAD) == 108 && sizeof(BASE_BEAM_SAVELOAD) == 68,
              "Original beam persistent record extent");
static_assert(sizeof(JUMPPLAT_SAVELOAD) == 400 && sizeof(BASE_JUMPPLAT_SAVELOAD) == 8,
              "Original jump-platform persistent record extent");
static_assert(sizeof(FIGHTER_SAVELOAD) == 84 && sizeof(BASE_FIGHTER_SAVELOAD) == 44,
              "Original fighter persistent record extent");
static_assert(sizeof(FIGHTERWING_SAVELOAD) == 20 &&
                  offsetof(FIGHTERWING_SAVELOAD, createTime) == 8 &&
                  offsetof(FIGHTERWING_SAVELOAD, bayDoorCloseTime) + sizeof(S32) == 20,
              "Original fighter-wing persistent record extent");

static_assert(sizeof(RECON_LAUNCH_SAVELOAD) == 36 &&
                  offsetof(RECON_LAUNCH_SAVELOAD, probeID) == 24 &&
                  offsetof(RECON_LAUNCH_SAVELOAD, targetID) == 28,
              "Original recon-launch persistent record extent");

static_assert(sizeof(BASE_STASISBOLT_SAVELOAD) == 192 && sizeof(STASISBOLT_SAVELOAD) == 336,
              "Original stasis-bolt persistent record extent");

static_assert(sizeof(BASE_REPELLENTCLOUD_SAVELOAD) == 204 && sizeof(REPELLENTCLOUD_SAVELOAD) == 348,
              "Original repellent-cloud persistent record extent");

static_assert(sizeof(BASE_REPULSORWAVE_SAVELOAD) == 124 && sizeof(REPULSORWAVE_SAVELOAD) == 268,
              "Original repulsor-wave persistent record extent");

static_assert(sizeof(BASE_DESTABILIZER_SAVELOAD) == 44 && sizeof(DESTABILIZER_SAVELOAD) == 188,
              "Original destabilizer persistent record extent");

static_assert(sizeof(BASE_MIMIC_SAVELOAD) == 8 && sizeof(MIMIC_SAVELOAD) == 8 &&
                  offsetof(MIMIC_SAVELOAD, aliasArchetypeID) == 0 &&
                  offsetof(MIMIC_SAVELOAD, aliasPlayerID) == 4,
              "Original mimic persistent record extent and alias fields");

static_assert(sizeof(TRACTOR_SAVELOAD) == 20 &&
                  offsetof(TRACTOR_SAVELOAD, targetID) == 0 &&
                  offsetof(TRACTOR_SAVELOAD, systemID) == 4 &&
                  offsetof(TRACTOR_SAVELOAD, time) == 8 &&
                  offsetof(TRACTOR_SAVELOAD, refireDelay) == 12 &&
                  offsetof(TRACTOR_SAVELOAD, mass) + sizeof(SINGLE) == 20,
              "Original tractor persistent fields and final extent");

static_assert(sizeof(TALORIAN_LAUNCHER_SAVELOAD) == 56 &&
                  offsetof(TALORIAN_LAUNCHER_SAVELOAD, targetIDs) == 4 &&
                  offsetof(TALORIAN_LAUNCHER_SAVELOAD, bShildOn) == 52,
              "Original Talorian launcher data record extent");

extern SymbolManager SymbolTable;
extern ICOManager * DACOM;
SYMBOL parse_memory(const char * memory);
void COMAPI CreateDataParser(SYMBOL symbol, IDataParser ** parser);
void RegisterViewConstructor(ICOManager * manager);
BOOL32 CopyViewSymbolValue(SYMBOL source, U32 sourceModifiers, const char *sourceData,
                          SYMBOL destination, U32 destinationModifiers, char *destinationData);

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
    // Retail initial Globals lacks only the later lobby metadata nibble.
    // Exercise the real owner-specific provider; generic missing fields fail.
    const auto removeField = [](std::string source, const char *name) {
        const std::size_t at = source.find(name);
        if (at == std::string::npos) return std::string();
        const std::size_t start = source.rfind('\n', at);
        const std::size_t end = source.find(';', at);
        if (end == std::string::npos) return std::string();
        source.erase(start == std::string::npos ? 0U : start + 1U,
                     end + 1U - (start == std::string::npos ? 0U : start + 1U));
        return source;
    };
    const std::string oldGlobalText = removeField(removeField(std::string(text.data()), "maxPlayers"), "compChalange");
    HANDLE oldGlobalSymbols = constructor->ParseNewMemory(oldGlobalText.c_str());
    SYMBOL oldGlobal = ResolveViewSymbolType(constructor->GetSymbol(oldGlobalSymbols, "MT_GlobalData"));
    std::vector<unsigned char> currentGlobals(3236U, 0), oldGlobalData(3236U, 0), restoredGlobals(3236U, 0);
    const auto globalField = [&](const char *name) {
        for (SYMBOL field = record ? record->datatype : nullptr; field; field = field->link)
            if (field->name && !std::strcmp(field->name, name)) return field;
        return SYMBOL(nullptr);
    };
    const auto setBits = [&](const char *name, U32 value) {
        SYMBOL field = globalField(name);
        if (!field || !field->bit_count || field->offset < 0) return false;
        for (unsigned bit = 0; bit < field->bit_count; ++bit)
        {
            const std::size_t index = field->offset + (field->bit_offset + bit) / 8U;
            if (index >= currentGlobals.size()) return false;
            const unsigned char mask = static_cast<unsigned char>(1U << ((field->bit_offset + bit) % 8U));
            if (value & (1U << bit)) currentGlobals[index] |= mask;
            else currentGlobals[index] &= static_cast<unsigned char>(~mask);
        }
        return true;
    };
    const auto setWord = [&](const char *name, U32 value) {
        SYMBOL field = globalField(name);
        if (!field || field->offset < 0 || std::size_t(field->offset) + sizeof(value) > currentGlobals.size()) return false;
        std::memcpy(currentGlobals.data() + field->offset, &value, sizeof(value)); return true;
    };
    if (!setBits("currentPlayer", 3U) || !setBits("maxPlayers", 7U) ||
        !setBits("bGlobalLighting", 1U) || !setBits("bScriptUIControl", 1U) ||
        !setWord("lastPartID", 0x12345678U) || !setWord("missionID", 0x87654321U)) return false;
    const auto namedField = [](SYMBOL type, const char *name) {
        for (SYMBOL field = type ? type->datatype : nullptr; field; field = field->link)
        {
            const char *fieldName = field->name ? field->name : ResolveViewSymbolType(field)->name;
            if (fieldName && !std::strcmp(fieldName, name)) return field;
        }
        return SYMBOL(nullptr);
    };
    const auto slotArray = [&](SYMBOL globals) {
        SYMBOL gameField = namedField(globals, "gameSettings");
        SYMBOL baseField = namedField(ResolveViewSymbolType(gameField), "_CQGAME");
        return namedField(ResolveViewSymbolType(baseField), "slot");
    };
    SYMBOL currentSlots = slotArray(record);
    SYMBOL oldSlots = slotArray(oldGlobal);
    if (!currentSlots || !oldSlots) return false;
    SYMBOL oldArray = ResolveViewSymbolType(oldSlots);
    SYMBOL oldSlot = ResolveViewSymbolType(oldArray->datatype);
    SYMBOL gameField = namedField(record, "gameSettings");
    SYMBOL baseField = namedField(ResolveViewSymbolType(gameField), "_CQGAME");
    SYMBOL optionsField = namedField(ResolveViewSymbolType(gameField), "OPTIONS");
    if (!optionsField) return false;
    const std::size_t optionsOffset = gameField->offset + optionsField->offset;
    const U32 optionsVersion = 0x10203040U, optionsWord = 0xe9abcdefU;
    if (optionsOffset + 8U > currentGlobals.size()) return false;
    std::memcpy(currentGlobals.data() + optionsOffset, &optionsVersion, sizeof(optionsVersion));
    std::memcpy(currentGlobals.data() + optionsOffset + 4U, &optionsWord, sizeof(optionsWord));
    const std::size_t slotsOffset = gameField->offset + baseField->offset + currentSlots->offset;
    const U32 slotWord = 1U | (5U << 3) | (1U << 7) | (1U << 10) | (9U << 14) | (3U << 19) | (2U << 23);
    for (unsigned slot = 0; slot < 8U; ++slot)
    {
        const U32 id = 0x10203040U + slot;
        std::memcpy(currentGlobals.data() + slotsOffset + 8U * slot, &slotWord, sizeof(slotWord));
        std::memcpy(currentGlobals.data() + slotsOffset + 8U * slot + 4U, &id, sizeof(id));
    }
    auto expectedGlobals = currentGlobals;
    expectedGlobals[624] &= 0x0fU;
    for (unsigned slot = 0; slot < 8U; ++slot)
    {
        const U32 withoutChallenge = slotWord & ~(0xfU << 3);
        std::memcpy(expectedGlobals.data() + slotsOffset + 8U * slot, &withoutChallenge, sizeof(withoutChallenge));
    }
    if (!Expect(record && oldGlobalSymbols && oldGlobal && oldGlobal->size == 3236 &&
                record->size == 3236 &&
                CorrelateNativeViewSymbols(constructor, record, currentGlobals.data(), oldGlobal, oldGlobalData.data()) &&
                !CorrelateNativeViewSymbols(constructor, oldGlobal, oldGlobalData.data(), record, restoredGlobals.data()) &&
                CorrelateNativeMissionGlobals(constructor, oldGlobal, oldGlobalData.data(), record, restoredGlobals.data()) &&
                restoredGlobals == expectedGlobals,
                "real Globals migration defaults only absent maxPlayers/challenge and preserves shifted slot fields/IDs")) return false;
    std::vector<unsigned char> canonicalGlobals(3236U, 0);
    if (!Expect(CorrelateNativeMissionGlobals(constructor, record, currentGlobals.data(), record, canonicalGlobals.data()) &&
                canonicalGlobals == currentGlobals,
                "real Globals canonical correlation retains explicit maxPlayers")) return false;
    // TM2's retail initial Globals also predates OPTIONS::commandLimit.
    // Exercise the actual Mission::ParseDefFile migration and retain all other
    // option bits; accepting this permits the original second mission to load.
    std::string oldOptionsText = removeField(oldGlobalText, "commandLimit");
    const auto oldOptionsAt = oldOptionsText.find("struct OPTIONS");
    const auto speedAt = oldOptionsText.find("gameSpeed:5", oldOptionsAt);
    if (oldOptionsAt == std::string::npos || speedAt == std::string::npos) return false;
    oldOptionsText.replace(speedAt, std::strlen("gameSpeed:5"), "gameSpeed:6");
    HANDLE oldOptionsSymbols = constructor->ParseNewMemory(oldOptionsText.c_str());
    SYMBOL oldOptionsGlobal = ResolveViewSymbolType(constructor->GetSymbol(oldOptionsSymbols, "MT_GlobalData"));
    SYMBOL oldOptionsGame = namedField(oldOptionsGlobal, "gameSettings");
    SYMBOL oldOptionsBase = namedField(ResolveViewSymbolType(oldOptionsGame), "OPTIONS");
    SYMBOL oldOptions = ResolveViewSymbolType(oldOptionsBase);
    SYMBOL newOptions = ResolveViewSymbolType(optionsField);
    SYMBOL commandLimit = namedField(newOptions, "commandLimit");
    SYMBOL visibility = namedField(oldOptions, "visibility");
    SYMBOL oldSpeed = namedField(oldOptions, "gameSpeed");
    if (!oldOptionsSymbols || !oldOptionsGlobal || !oldOptions || !commandLimit || !visibility || !oldSpeed)
        return false;
    std::vector<unsigned char> oldOptionsData(3236U, 0), restoredOptions(3236U, 0);
    auto expectedOptions = expectedGlobals;
    const U32 defaultCommandWord = optionsWord & 0x3fffffffU;
    std::memcpy(expectedOptions.data() + optionsOffset + 4U, &defaultCommandWord, sizeof(defaultCommandWord));
    if (!Expect(oldOptionsGlobal->size == 3236 && oldOptions->size == 8 &&
                CorrelateNativeViewSymbols(constructor, record, currentGlobals.data(), oldOptionsGlobal, oldOptionsData.data()) &&
                !CorrelateNativeViewSymbols(constructor, oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data()) &&
                CorrelateNativeMissionGlobals(constructor, oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data()) &&
                restoredOptions == expectedOptions,
                "TM2 Globals defaults only absent commandLimit and retains version/options/slots/IDs")) return false;
    std::string enumOptionsText = oldOptionsText;
    struct LegacyOptionType { const char *field; const char *type; };
    const LegacyOptionType legacyOptionTypes[] = {
        {"gameType", "GAMETYPE"}, {"money", "MONEY"}, {"mapType", "MAPTYPE"},
        {"templateType", "RANDOM_TEMPLATE"}, {"mapSize", "MAPSIZE"},
        {"terrain", "TERRAIN"}, {"units", "STARTING_UNITS"}, {"visibility", "VISIBILITYMODE"}};
    for (const auto &option : legacyOptionTypes)
    {
        const std::string declaration = std::string("int ") + option.field + ':';
        const auto at = enumOptionsText.find(declaration, oldOptionsAt);
        if (at == std::string::npos) return false;
        enumOptionsText.replace(at, 4U, std::string(option.type) + ' ');
    }
    HANDLE enumOptionsSymbols = constructor->ParseNewMemory(enumOptionsText.c_str());
    SYMBOL enumOptionsGlobal = ResolveViewSymbolType(constructor->GetSymbol(enumOptionsSymbols, "MT_GlobalData"));
    if (!enumOptionsGlobal) return false;
    // The original enum-to-int provider parses the symbolic identifier as zero.
    // Use valid -1 enumerators, retaining that Windows behavior and all adjacent
    // non-enum bits instead of treating arbitrary packed values as enum names.
    constexpr U32 enumMask = 0x3fff0007U;
    auto enumInput = currentGlobals, expectedEnums = expectedOptions;
    const U32 enumInputWord = optionsWord | enumMask;
    const U32 enumOutputWord = optionsWord & ~(enumMask | 0xc0000000U);
    std::memcpy(enumInput.data() + optionsOffset + 4U, &enumInputWord, sizeof(enumInputWord));
    std::memcpy(expectedEnums.data() + optionsOffset + 4U, &enumOutputWord, sizeof(enumOutputWord));
    if (!Expect(CorrelateNativeViewSymbols(constructor, record, enumInput.data(), enumOptionsGlobal, oldOptionsData.data()) &&
                CorrelateNativeMissionGlobals(constructor, enumOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data()) &&
                restoredOptions == expectedEnums,
                "TM2 enum options retain original Windows enum-to-int values and surrounding controls")) return false;
    constructor->DestroySymbols(enumOptionsSymbols);
    const int optionsSize = oldOptions->size;
    oldOptions->size = 12;
    const bool wrongOptionsExtentRejected = !CorrelateNativeMissionGlobals(constructor,
        oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data());
    oldOptions->size = optionsSize;
    const unsigned visibilityBit = visibility->bit_offset;
    visibility->bit_offset = visibilityBit + 1U;
    const bool wrongOptionsBitsRejected = !CorrelateNativeMissionGlobals(constructor,
        oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data());
    visibility->bit_offset = visibilityBit;
    const unsigned speedBits = oldSpeed->bit_count;
    oldSpeed->bit_count = 4U;
    const bool wrongOptionsWidthRejected = !CorrelateNativeMissionGlobals(constructor,
        oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data());
    oldSpeed->bit_count = speedBits;
    const unsigned commandBit = commandLimit->bit_offset;
    commandLimit->bit_offset = commandBit - 1U;
    const bool wrongCommandBitsRejected = !CorrelateNativeMissionGlobals(constructor,
        oldOptionsGlobal, oldOptionsData.data(), record, restoredOptions.data());
    commandLimit->bit_offset = commandBit;
    std::string missingOptionText = oldOptionsText;
    const auto optionsAt = missingOptionText.find("struct OPTIONS");
    const auto versionAt = missingOptionText.find("version", optionsAt);
    if (optionsAt == std::string::npos || versionAt == std::string::npos) return false;
    missingOptionText.replace(versionAt, 7U, "MissingRequiredVersion");
    HANDLE missingOptionSymbols = constructor->ParseNewMemory(missingOptionText.c_str());
    SYMBOL missingOptionGlobal = ResolveViewSymbolType(constructor->GetSymbol(missingOptionSymbols, "MT_GlobalData"));
    if (!Expect(missingOptionGlobal && missingOptionGlobal->size == 3236 &&
                !CorrelateNativeMissionGlobals(constructor, missingOptionGlobal, oldOptionsData.data(), record, restoredOptions.data()) &&
                wrongOptionsExtentRejected && wrongOptionsBitsRejected && wrongOptionsWidthRejected && wrongCommandBitsRejected,
                "TM2 optional command limit rejects required version loss, wrong extent and overlapping bits")) return false;
    constructor->DestroySymbols(missingOptionSymbols);
    constructor->DestroySymbols(oldOptionsSymbols);
    std::string badGlobalsText = oldGlobalText;
    const auto requiredAt = badGlobalsText.find("lastPartID");
    if (requiredAt == std::string::npos) return false;
    badGlobalsText.replace(requiredAt, std::strlen("lastPartID"), "MissingRequiredPartID");
    HANDLE badGlobalsSymbols = constructor->ParseNewMemory(badGlobalsText.c_str());
    SYMBOL badGlobals = ResolveViewSymbolType(constructor->GetSymbol(badGlobalsSymbols, "MT_GlobalData"));
    Symbol wrongExtent = *oldGlobal;
    wrongExtent.size += 4;
    Symbol wrongOwner = *oldGlobal;
    wrongOwner.name = const_cast<char *>("OtherGlobalData");
    if (!Expect(badGlobalsSymbols && badGlobals && badGlobals->size == 3236 &&
                !CorrelateNativeMissionGlobals(constructor, badGlobals, oldGlobalData.data(), record, restoredGlobals.data()) &&
                !CorrelateNativeMissionGlobals(constructor, &wrongExtent, oldGlobalData.data(), record, restoredGlobals.data()) &&
                !CorrelateNativeMissionGlobals(constructor, &wrongOwner, oldGlobalData.data(), record, restoredGlobals.data()),
                "Globals metadata compatibility rejects another missing field, extent and owner")) return false;
    std::string missingSlotText = oldGlobalText;
    const auto slotAt = missingSlotText.find("struct SLOT");
    const auto idAt = missingSlotText.find("dpid", slotAt);
    if (slotAt == std::string::npos || idAt == std::string::npos) return false;
    missingSlotText.replace(idAt, 4U, "MissingRequiredPlayerID");
    HANDLE missingSlotSymbols = constructor->ParseNewMemory(missingSlotText.c_str());
    SYMBOL missingSlot = ResolveViewSymbolType(constructor->GetSymbol(missingSlotSymbols, "MT_GlobalData"));
    const int originalCount = oldArray->arrayval;
    oldArray->arrayval = 7;
    const bool wrongCountRejected = !CorrelateNativeMissionGlobals(constructor, oldGlobal, oldGlobalData.data(), record, restoredGlobals.data());
    oldArray->arrayval = originalCount;
    const int originalStride = oldSlot->size;
    oldSlot->size = 12;
    const bool wrongStrideRejected = !CorrelateNativeMissionGlobals(constructor, oldGlobal, oldGlobalData.data(), record, restoredGlobals.data());
    oldSlot->size = originalStride;
    if (!Expect(missingSlotSymbols && missingSlot && missingSlot->size == 3236 &&
                !CorrelateNativeMissionGlobals(constructor, missingSlot, oldGlobalData.data(), record, restoredGlobals.data()) &&
                wrongCountRejected && wrongStrideRejected,
                "legacy slot defaults reject required dpid loss, wrong array count and stride")) return false;
    constructor->DestroySymbols(missingSlotSymbols);
    constructor->DestroySymbols(badGlobalsSymbols);
    constructor->DestroySymbols(oldGlobalSymbols);
    const SYMBOL mission = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "MISSION_SAVELOAD"));
    SYMBOL caps = nullptr;
    for (SYMBOL field = mission ? mission->datatype : nullptr;
         field; field = field->link)
        if (field->name && std::strcmp(field->name, "caps") == 0) caps = field;
    MISSION_SAVELOAD saved = {};
    // Exercise all capability bits and the named padding byte, not only move.
    const unsigned char capabilityBytes[] = {0xA5, 0xFF, 0xFF, 0xFF};
    std::memcpy(&saved.caps, capabilityBytes, sizeof(saved.caps));
    saved.controlGroupID = 3;
    saved.groupIndex = 7;
    saved.groupID = 0x12345678;
    saved.pendingOp = 0x87654321;
    SPACESHIP_SAVELOAD movingShip = {};
    movingShip.tobjmove.moveAgentID = 0x12345678;
    movingShip.tobjmove.jumpAgentID = 0x87654321;
    movingShip.tobjmove.pathLength = 3;
    movingShip.tobjmove.overrideSpeed = 1.25F;
    movingShip.tobjmove.bMoveActive = true;
    movingShip.tobjmove.bCompletionAllowed = true;
    movingShip.tobjmove.bFinalMove = true;
    movingShip.tobjmove.bSyncNeeded = true;
    MISSION_SAVELOAD restored = {};
    bool valid = record && record->size != 0U && mission && caps &&
        mission->size == sizeof(saved) && caps->size == sizeof(saved.caps);
    const SYMBOL trainingMenu = ResolveViewSymbolType(constructor->GetSymbol(symbols, "GT_MENU1"));
    GT_MENU1 trainingSaved{}, trainingRestored{};
    trainingSaved.selectCampaign.buttonSolarian.xOrigin = 300;
    if (!trainingMenu || trainingMenu->size != sizeof(trainingSaved))
    {
        std::cerr << "Original training-menu layout mismatch; schema="
                  << (trainingMenu ? trainingMenu->size : 0) << " native=" << sizeof(trainingSaved) << '\n';
        valid = false;
    }
    else
    {
        constructor->CorrelateSymbol(trainingMenu, &trainingSaved, trainingMenu, &trainingRestored);
        valid = valid && trainingRestored.selectCampaign.buttonSolarian.xOrigin == 300;
    }
    const SYMBOL endMenu = ResolveViewSymbolType(constructor->GetSymbol(symbols, "GT_ENDGAME"));
    const SYMBOL messageBox = ResolveViewSymbolType(constructor->GetSymbol(symbols, "GT_MESSAGEBOX"));
    GT_ENDGAME endSaved{}, endRestored{};
    GT_MESSAGEBOX messageSaved{}, messageRestored{};
    endSaved.screenRect.right = 1024;
    endSaved.cont.xOrigin = 700;
    endSaved.staticDescription.width = 400;
    endSaved.overviewTab.staticOverviewTitles[TITLES_O - 1].width = 321;
    endSaved.overviewTab.staticOverviewUnits[0].width = 123;
    messageSaved.okAlone.xOrigin = 300;
    messageSaved.message.width = 500;
    if (!endMenu || endMenu->size != sizeof(endSaved) ||
        !messageBox || messageBox->size != sizeof(messageSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(endMenu, &endSaved, endMenu, &endRestored);
        constructor->CorrelateSymbol(messageBox, &messageSaved, messageBox, &messageRestored);
        valid = valid && endRestored.screenRect.right == 1024 &&
            endRestored.cont.xOrigin == 700 && endRestored.staticDescription.width == 400 &&
            endRestored.overviewTab.staticOverviewTitles[TITLES_O - 1].width == 321 &&
            endRestored.overviewTab.staticOverviewUnits[0].width == 123 &&
            messageRestored.okAlone.xOrigin == 300 && messageRestored.message.width == 500;
    }
    const SYMBOL jump = ResolveViewSymbolType(constructor->GetSymbol(symbols, "JUMPPLAT_SAVELOAD"));
    JUMPPLAT_SAVELOAD jumpSaved{}, jumpRestored{};
    jumpSaved.jumpPlatSaveload.jumpGateID = 37;
    jumpSaved.jumpPlatSaveload.bOwnGate = true;
    jumpSaved.jumpPlatSaveload.bLockGate = true;
    if (!jump || jump->size != sizeof(jumpSaved) ||
        reinterpret_cast<const unsigned char *>(&jumpSaved.jumpPlatSaveload) -
            reinterpret_cast<const unsigned char *>(&jumpSaved) != 392) valid = false;
    else
    {
        constructor->CorrelateSymbol(jump, &jumpSaved, jump, &jumpRestored);
        valid = valid && jumpRestored.jumpPlatSaveload.jumpGateID == 37 &&
            jumpRestored.jumpPlatSaveload.bOwnGate && jumpRestored.jumpPlatSaveload.bLockGate;
    }
    // TM2's real ChangeCamera(MissionCamStart) caller requires the original
    // MovieCamera object. Its existing Mission save/load must retain both
    // object transform and authored camera view rather than a placeholder.
    const SYMBOL movieCamera = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MOVIE_CAMERA_SAVELOAD"));
    const SYMBOL movieCameraArchetype = ResolveViewSymbolType(constructor->GetSymbol(symbols, "BT_MOVIE_CAMERA_DATA"));
    MOVIE_CAMERA_SAVELOAD cameraSaved{}, cameraRestored{};
    cameraSaved.trans_SL.position = Vector(1, 2, 3);
    cameraSaved.baseSave.dwMissionID = 0x12345678U;
    cameraSaved.baseSave.systemID = 2;
    std::strcpy(cameraSaved.baseSave.partName.string, "MissionCamStart");
    cameraSaved.baseSave.cam_lookAt = Vector(4, 5, 6);
    cameraSaved.baseSave.cam_position = Vector(7, 8, 9);
    cameraSaved.baseSave.cam_FOV_x = 0.75F;
    cameraSaved.baseSave.cam_FOV_y = 0.5F;
    if (!movieCamera || movieCamera->size != 176 || sizeof(cameraSaved) != 176 ||
        !movieCameraArchetype || movieCameraArchetype->size != 112 ||
        sizeof(BT_MOVIE_CAMERA_DATA) != 112 ||
        reinterpret_cast<const unsigned char *>(&cameraSaved.baseSave) -
            reinterpret_cast<const unsigned char *>(&cameraSaved) != 40 ||
        reinterpret_cast<const unsigned char *>(&cameraSaved.baseSave.cam_lookAt) -
            reinterpret_cast<const unsigned char *>(&cameraSaved) != 144 ||
        reinterpret_cast<const unsigned char *>(&cameraSaved.baseSave.cam_FOV_y) -
            reinterpret_cast<const unsigned char *>(&cameraSaved) + sizeof(SINGLE) != 176)
        valid = false;
    else
    {
        valid = valid && CorrelateNativeViewSymbols(constructor, movieCamera, &cameraSaved,
                                                   movieCamera, &cameraRestored) &&
            cameraRestored.trans_SL.position.x == 1 && cameraRestored.trans_SL.position.y == 2 &&
            cameraRestored.trans_SL.position.z == 3 && cameraRestored.baseSave.dwMissionID == 0x12345678U &&
            cameraRestored.baseSave.systemID == 2 &&
            std::strcmp(cameraRestored.baseSave.partName.string, "MissionCamStart") == 0 &&
            cameraRestored.baseSave.cam_lookAt.x == 4 && cameraRestored.baseSave.cam_lookAt.y == 5 &&
            cameraRestored.baseSave.cam_lookAt.z == 6 && cameraRestored.baseSave.cam_position.x == 7 &&
            cameraRestored.baseSave.cam_position.y == 8 && cameraRestored.baseSave.cam_position.z == 9 &&
            cameraRestored.baseSave.cam_FOV_x == 0.75F && cameraRestored.baseSave.cam_FOV_y == 0.5F;
    }
    // BuildPlat -> MGlobals::CreateInstance -> Gunboat needs both original
    // Missile Cruiser launcher owners. Protect their real schema extents and
    // Cloak launcher's persisted flags without manufacturing a created ship.
    const SYMBOL verticalType = ResolveViewSymbolType(constructor->GetSymbol(symbols, "BT_VERTICAL_LAUNCH"));
    const SYMBOL cloakType = ResolveViewSymbolType(constructor->GetSymbol(symbols, "BT_CLOAK_LAUNCHER"));
    const SYMBOL cloakRecord = ResolveViewSymbolType(constructor->GetSymbol(symbols, "CLOAK_LAUNCHER_SAVELOAD"));
    CLOAK_LAUNCHER_SAVELOAD cloakSaved{}, cloakRestored{};
    cloakSaved.bCloakEnabled = true;
    cloakSaved.bPrepareToggle = true;
    cloakSaved.cloakSupplyCount = 0.375F;
    valid = valid && verticalType && verticalType->size == 796 &&
        sizeof(BT_VERTICAL_LAUNCH) == 796 && cloakType && cloakType->size == 84 &&
        sizeof(BT_CLOAK_LAUNCHER) == 84 && cloakRecord && cloakRecord->size == 8 &&
        sizeof(cloakSaved) == 8 &&
        CorrelateNativeViewSymbols(constructor, cloakRecord, &cloakSaved, cloakRecord, &cloakRestored) &&
        cloakRestored.bCloakEnabled && cloakRestored.bPrepareToggle &&
        cloakRestored.cloakSupplyCount == 0.375F;
    // Catalogue integration must retain the actual ship/launcher saved records;
    // these correlations exercise schema ownership without creating game units.
    const auto schemaExtent = [&](const char *name, std::size_t size)
    {
        const SYMBOL symbol = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        return symbol && symbol->size >= 0 && static_cast<std::size_t>(symbol->size) == size;
    };
    valid = valid && schemaExtent("BT_FLAGSHIP_DATA", sizeof(BT_FLAGSHIP_DATA)) &&
        schemaExtent("BT_RECONPROBE_DATA", sizeof(BT_RECONPROBE_DATA)) &&
        schemaExtent("BT_WORMHOLE_LAUNCHER", sizeof(BT_WORMHOLE_LAUNCHER)) &&
        schemaExtent("BT_MULTICLOAK_LAUNCHER", sizeof(BT_MULTICLOAK_LAUNCHER)) &&
        schemaExtent("BT_SYNTHESIS_DATA", sizeof(BT_SYNTHESIS_DATA));
    const auto correlateRecord = [&](const char *name, const auto &source, auto &destination)
    {
        const SYMBOL symbol = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        return symbol && symbol->size == sizeof(source) &&
            CorrelateNativeViewSymbols(constructor, symbol, &source, symbol, &destination);
    };
    FLAGSHIP_SAVELOAD flagshipSaved{}, flagshipRestored{};
    flagshipSaved.flagshipSaveLoad.knownFormations[MAX_FORMATIONS - 1] = 0x12345678U;
    flagshipSaved.flagshipSaveLoad.commandKitsArchID[MAX_KNOWN_KITS - 1] = 0x23456789U;
    flagshipSaved.flagshipSaveLoad.bRovingAllowed = true;
    flagshipSaved.flagshipSaveLoad.buildTime = 0.375F;
    valid = valid && correlateRecord("FLAGSHIP_SAVELOAD", flagshipSaved, flagshipRestored) &&
        flagshipRestored.flagshipSaveLoad.knownFormations[MAX_FORMATIONS - 1] == 0x12345678U &&
        flagshipRestored.flagshipSaveLoad.commandKitsArchID[MAX_KNOWN_KITS - 1] == 0x23456789U &&
        flagshipRestored.flagshipSaveLoad.bRovingAllowed &&
        flagshipRestored.flagshipSaveLoad.buildTime == 0.375F;
    TROOPSHIP_SAVELOAD troopSaved{}, troopRestored{};
    troopSaved.troopSaveLoad.dwTargetID = 0x12345678U;
    troopSaved.troopSaveLoad.attackAgentID = 37;
    troopSaved.troopSaveLoad.bTakeoverApproved = true;
    TROOPPOD_SAVELOAD podSaved{0x23456789U}, podRestored{};
    valid = valid && correlateRecord("TROOPSHIP_SAVELOAD", troopSaved, troopRestored) &&
        troopRestored.troopSaveLoad.dwTargetID == 0x12345678U &&
        troopRestored.troopSaveLoad.attackAgentID == 37 && troopRestored.troopSaveLoad.bTakeoverApproved &&
        correlateRecord("TROOPPOD_SAVELOAD", podSaved, podRestored) && podRestored.dwTargetID == 0x23456789U;
    RECONPROBE_SAVELOAD probeSaved{}, probeRestored{};
    probeSaved.baseSaveLoad.probeTimer = 0.375F;
    probeSaved.baseSaveLoad.goal = Vector(1, 2, 3);
    probeSaved.baseSaveLoad.workingID = 37;
    probeSaved.baseSaveLoad.bLauncherDelete = true;
    valid = valid && correlateRecord("RECONPROBE_SAVELOAD", probeSaved, probeRestored) &&
        probeRestored.baseSaveLoad.probeTimer == 0.375F && probeRestored.baseSaveLoad.goal.z == 3 &&
        probeRestored.baseSaveLoad.workingID == 37 && probeRestored.baseSaveLoad.bLauncherDelete;
    WORMHOLE_LAUNCHER_SAVELOAD wormSaved{}, wormRestored{};
    wormSaved.numTargets = MAX_WORM_VICTIMS;
    wormSaved.targetIDs[MAX_WORM_VICTIMS - 1] = 0x12345678U;
    wormSaved.mode = WORMHOLE_LAUNCHER_SAVELOAD::WORM_WORKING;
    valid = valid && correlateRecord("WORMHOLE_LAUNCHER_SAVELOAD", wormSaved, wormRestored) &&
        wormRestored.numTargets == MAX_WORM_VICTIMS &&
        wormRestored.targetIDs[MAX_WORM_VICTIMS - 1] == 0x12345678U &&
        wormRestored.mode == WORMHOLE_LAUNCHER_SAVELOAD::WORM_WORKING;
    MULTICLOAK_LAUNCHER_SAVELOAD multiSaved{}, multiRestored{};
    multiSaved.cloakTargetID = 0x23456789U;
    multiSaved.decloakTime = 37;
    multiSaved.bCloakingTarget = multiSaved.bPrepareToggle = true;
    valid = valid && correlateRecord("MULTICLOAK_LAUNCHER_SAVELOAD", multiSaved, multiRestored) &&
        multiRestored.cloakTargetID == 0x23456789U && multiRestored.decloakTime == 37 &&
        multiRestored.bCloakingTarget && multiRestored.bPrepareToggle;
    SYNTHESIS_SAVELOAD synthesisSaved{}, synthesisRestored{};
    synthesisSaved.stage = SYN_ABSORB;
    synthesisSaved.hullPointsPer = 0.375F;
    synthesisSaved.targetID = 0x12345678U;
    synthesisSaved.trans_SL.position = Vector(1, 2, 3);
    valid = valid && correlateRecord("SYNTHESIS_SAVELOAD", synthesisSaved, synthesisRestored) &&
        synthesisRestored.stage == SYN_ABSORB && synthesisRestored.hullPointsPer == 0.375F &&
        synthesisRestored.targetID == 0x12345678U && synthesisRestored.trans_SL.position.z == 3;
    const SYMBOL gun = ResolveViewSymbolType(constructor->GetSymbol(symbols, "GUNPLAT_SAVELOAD"));
    GUNPLAT_SAVELOAD gunSaved{}, gunRestored{};
    gunSaved.gunplatSaveload.dwTargetID = 0x12345672;
    gunSaved.gunplatSaveload.attackAgentID = 123;
    gunSaved.gunplatSaveload.launcherAgentID = 456;
    gunSaved.gunplatSaveload.bUserGenerated = true;
    gunSaved.gunplatSaveload.bSpecialAttack = true;
    if (!gun || gun->size != sizeof(gunSaved) ||
        reinterpret_cast<const unsigned char *>(&gunSaved.gunplatSaveload) -
            reinterpret_cast<const unsigned char *>(&gunSaved) != 392) valid = false;
    else
    {
        constructor->CorrelateSymbol(gun, &gunSaved, gun, &gunRestored);
        valid = valid && gunRestored.gunplatSaveload.dwTargetID == 0x12345672 &&
            gunRestored.gunplatSaveload.attackAgentID == 123 &&
            gunRestored.gunplatSaveload.launcherAgentID == 456 &&
            gunRestored.gunplatSaveload.bUserGenerated && gunRestored.gunplatSaveload.bSpecialAttack;
    }
    const SYMBOL beam = ResolveViewSymbolType(constructor->GetSymbol(symbols, "BEAM_SAVELOAD"));
    BEAM_SAVELOAD beamSaved{}, beamRestored{};
    beamSaved.targetID = 0x12345672;
    beamSaved.ownerID = 0x23456781;
    beamSaved.systemID = 2;
    beamSaved.startOffset = Vector(1, 2, 3);
    beamSaved.target1 = Vector(4, 5, 6);
    beamSaved.target2 = Vector(7, 8, 9);
    beamSaved.end = Vector(10, 11, 12);
    beamSaved.time = 0.5f;
    beamSaved.bHit = true;
    beamSaved.trans_SL.position = Vector(13, 14, 15);
    if (!beam || beam->size != sizeof(beamSaved) ||
        reinterpret_cast<const unsigned char *>(&beamSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&beamSaved) != 68) valid = false;
    else
    {
        constructor->CorrelateSymbol(beam, &beamSaved, beam, &beamRestored);
        valid = valid && beamRestored.targetID == beamSaved.targetID &&
            beamRestored.ownerID == beamSaved.ownerID && beamRestored.systemID == 2 &&
            beamRestored.startOffset.x == 1 && beamRestored.target1.y == 5 &&
            beamRestored.target2.z == 9 && beamRestored.end.z == 12 &&
            beamRestored.time == 0.5f && beamRestored.bHit &&
            beamRestored.trans_SL.position.z == 15;
    }
    const SYMBOL disruptor = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MASS_DISRUPTOR_SAVELOAD"));
    MASS_DISRUPTOR_SAVELOAD disruptorSaved{}, disruptorRestored{};
    disruptorSaved.ownerID = 0x23456781;
    disruptorSaved.systemID = 2;
    disruptorSaved.targetID = 0x12345672;
    disruptorSaved.damageDealt = 25;
    disruptorSaved.targetDir = Vector(1, 2, 3);
    disruptorSaved.dist = 0.5f;
    disruptorSaved.trans_SL.position = Vector(4, 5, 6);
    if (!disruptor || disruptor->size != sizeof(disruptorSaved) ||
        reinterpret_cast<const unsigned char *>(&disruptorSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&disruptorSaved) != 36) valid = false;
    else
    {
        constructor->CorrelateSymbol(disruptor, &disruptorSaved, disruptor, &disruptorRestored);
        valid = valid && disruptorRestored.ownerID == disruptorSaved.ownerID &&
            disruptorRestored.systemID == 2 && disruptorRestored.targetID == disruptorSaved.targetID &&
            disruptorRestored.damageDealt == 25 && disruptorRestored.targetDir.z == 3 &&
            disruptorRestored.dist == 0.5f && disruptorRestored.trans_SL.position.z == 6;
    }
    const SYMBOL repair = ResolveViewSymbolType(constructor->GetSymbol(symbols, "REPAIRPLAT_SAVELOAD"));
    REPAIRPLAT_SAVELOAD repairSaved{}, repairRestored{};
    repairSaved.repairPlatSaveload.workingID = 123;
    repairSaved.repairPlatSaveload.repairTargetID = 0x12345671;
    repairSaved.repairPlatSaveload.mode = BASE_REPAIRPLAT_SAVELOAD::REP_REPAIRING;
    repairSaved.repairPlatSaveload.oldHullPoints = 25;
    repairSaved.repairPlatSaveload.supplyTimer = 0.5f;
    repairSaved.repairPlatSaveload.bDockLocked = true;
    repairSaved.repairPlatSaveload.bTakenCost = true;
    repairSaved.repairTransform.set_identity();
    repairSaved.repairTransform.translation = Vector(4, 5, 6);
    if (!repair || repair->size != sizeof(repairSaved) ||
        reinterpret_cast<const unsigned char *>(&repairSaved.repairPlatSaveload) -
            reinterpret_cast<const unsigned char *>(&repairSaved) != 392 ||
        reinterpret_cast<const unsigned char *>(&repairSaved.repairTransform) -
            reinterpret_cast<const unsigned char *>(&repairSaved) != 428) valid = false;
    else
    {
        constructor->CorrelateSymbol(repair, &repairSaved, repair, &repairRestored);
        valid = valid && repairRestored.repairPlatSaveload.workingID == 123 &&
            repairRestored.repairPlatSaveload.repairTargetID == 0x12345671 &&
            repairRestored.repairPlatSaveload.mode == BASE_REPAIRPLAT_SAVELOAD::REP_REPAIRING &&
            repairRestored.repairPlatSaveload.oldHullPoints == 25 &&
            repairRestored.repairPlatSaveload.supplyTimer == 0.5f &&
            repairRestored.repairPlatSaveload.bDockLocked && repairRestored.repairPlatSaveload.bTakenCost &&
            repairRestored.repairTransform.translation.z == 6;
    }
    const SYMBOL fighter = ResolveViewSymbolType(constructor->GetSymbol(symbols, "FIGHTER_SAVELOAD"));
    FIGHTER_SAVELOAD fighterSaved{}, fighterRestored{};
    fighterSaved.ownerID = 0x12345672;
    fighterSaved.dwMissionID = 0x23456782;
    fighterSaved.systemID = 1;
    fighterSaved.playerID = 2;
    fighterSaved.supplies = -1;
    fighterSaved.parentIndex = -1;
    fighterSaved.childIndex = 5;
    fighterSaved.refireTime = -3;
    fighterSaved.patrolTime = 12;
    fighterSaved.kamikaziTarget = Vector(1, 2, 3);
    fighterSaved.state = 4;
    fighterSaved.formationType = 2;
    fighterSaved.race = 1;
    fighterSaved.bCirclePatrol = true;
    fighterSaved.bKamikaziTargetSelected = true;
    fighterSaved.trans_SL.position = Vector(4, 5, 6);
    if (!fighter || fighter->size != sizeof(fighterSaved) ||
        reinterpret_cast<const unsigned char *>(&fighterSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&fighterSaved) != 44) valid = false;
    else
    {
        constructor->CorrelateSymbol(fighter, &fighterSaved, fighter, &fighterRestored);
        valid = valid && fighterRestored.ownerID == fighterSaved.ownerID &&
            fighterRestored.dwMissionID == fighterSaved.dwMissionID &&
            fighterRestored.systemID == 1 && fighterRestored.playerID == 2 &&
            fighterRestored.supplies == -1 && fighterRestored.parentIndex == -1 &&
            fighterRestored.childIndex == 5 && fighterRestored.refireTime == -3 &&
            fighterRestored.patrolTime == 12 && fighterRestored.kamikaziTarget.z == 3 &&
            fighterRestored.state == 4 && fighterRestored.formationType == 2 &&
            fighterRestored.race == 1 && fighterRestored.bCirclePatrol &&
            fighterRestored.bKamikaziTargetSelected && fighterRestored.trans_SL.position.z == 6;
    }
    const SYMBOL stasis = ResolveViewSymbolType(constructor->GetSymbol(symbols, "STASISBOLT_SAVELOAD"));
    STASISBOLT_SAVELOAD stasisSaved{}, stasisRestored{};
    stasisSaved.time = 0.5f;
    stasisSaved.targetPos.init(12, 24);
    stasisSaved.ownerID = 0x12345672;
    stasisSaved.numSquares = MAX_STASIS_SQUARES;
    stasisSaved.gvec[MAX_STASIS_SQUARES-1].init(36, 48);
    stasisSaved.numTargets = MAX_STASIS_TARGETS;
    stasisSaved.targets[MAX_STASIS_TARGETS-1] = 0x12345681;
    stasisSaved.lastSent = 2;
    stasisSaved.targetID = 0x12345691;
    stasisSaved.targetsHeld = 1;
    stasisSaved.bDeleteRequested = true;
    stasisSaved.bFreeTargets = true;
    stasisSaved.bNoMoreSync = true;
    stasisSaved.trans_SL.position = Vector(4, 5, 6);
    stasisSaved.mission.dwMissionID = 0x12345682;
    stasisSaved.mission.systemID = 1;
    if (!stasis || stasis->size != sizeof(stasisSaved) ||
        reinterpret_cast<const unsigned char *>(&stasisSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&stasisSaved) != 192 ||
        reinterpret_cast<const unsigned char *>(&stasisSaved.mission) -
            reinterpret_cast<const unsigned char *>(&stasisSaved) + sizeof(MISSION_SAVELOAD) != 336) valid = false;
    else
    {
        constructor->CorrelateSymbol(stasis, &stasisSaved, stasis, &stasisRestored);
        valid = valid && stasisRestored.time == 0.5f && stasisRestored.targetPos.getX() == 12 &&
            stasisRestored.targetPos.getY() == 24 && stasisRestored.ownerID == stasisSaved.ownerID &&
            stasisRestored.numSquares == MAX_STASIS_SQUARES && stasisRestored.gvec[MAX_STASIS_SQUARES-1].getX() == 36 &&
            stasisRestored.numTargets == MAX_STASIS_TARGETS &&
            stasisRestored.targets[MAX_STASIS_TARGETS-1] == stasisSaved.targets[MAX_STASIS_TARGETS-1] &&
            stasisRestored.lastSent == 2 && stasisRestored.targetID == stasisSaved.targetID &&
            stasisRestored.targetsHeld == 1 && stasisRestored.bDeleteRequested &&
            stasisRestored.bFreeTargets && stasisRestored.bNoMoreSync &&
            stasisRestored.trans_SL.position.z == 6 &&
            stasisRestored.mission.dwMissionID == stasisSaved.mission.dwMissionID && stasisRestored.mission.systemID == 1;
    }
    const SYMBOL repellent = ResolveViewSymbolType(constructor->GetSymbol(symbols, "REPELLENTCLOUD_SAVELOAD"));
    REPELLENTCLOUD_SAVELOAD repellentSaved{}, repellentRestored{};
    repellentSaved.time = 0.5f;
    repellentSaved.lastTime = -1;
    repellentSaved.targetPos.init(12, 24);
    repellentSaved.ownerID = 0x12345672;
    repellentSaved.numSquares = MAX_REPEL_SQUARES;
    repellentSaved.gvec[MAX_REPEL_SQUARES-1].init(36, 48);
    repellentSaved.numTargets = MAX_REPEL_TARGETS;
    repellentSaved.targets[MAX_REPEL_TARGETS-1] = 0x12345681;
    repellentSaved.targetsHeld = 1;
    repellentSaved.lastSent = 2;
    repellentSaved.sprayDir = Vector(1, 2, 3);
    repellentSaved.sideDir = Vector(4, 5, 6);
    repellentSaved.bDeleteRequested = true;
    repellentSaved.bFreeTargets = true;
    repellentSaved.bNoMoreSync = true;
    repellentSaved.trans_SL.position = Vector(4, 5, 6);
    repellentSaved.mission.dwMissionID = 0x12345682;
    repellentSaved.mission.systemID = 1;
    if (!repellent || repellent->size != sizeof(repellentSaved) ||
        reinterpret_cast<const unsigned char *>(&repellentSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&repellentSaved) != 204 ||
        reinterpret_cast<const unsigned char *>(&repellentSaved.mission) -
            reinterpret_cast<const unsigned char *>(&repellentSaved) + sizeof(MISSION_SAVELOAD) != 348) valid = false;
    else
    {
        constructor->CorrelateSymbol(repellent, &repellentSaved, repellent, &repellentRestored);
        valid = valid && repellentRestored.time == 0.5f && repellentRestored.lastTime == -1 &&
            repellentRestored.targetPos.getX() == 12 && repellentRestored.targetPos.getY() == 24 &&
            repellentRestored.ownerID == repellentSaved.ownerID && repellentRestored.numSquares == MAX_REPEL_SQUARES &&
            repellentRestored.gvec[MAX_REPEL_SQUARES-1].getX() == 36 &&
            repellentRestored.numTargets == MAX_REPEL_TARGETS &&
            repellentRestored.targets[MAX_REPEL_TARGETS-1] == repellentSaved.targets[MAX_REPEL_TARGETS-1] &&
            repellentRestored.targetsHeld == 1 && repellentRestored.lastSent == 2 &&
            repellentRestored.sprayDir.z == 3 && repellentRestored.sideDir.z == 6 &&
            repellentRestored.bDeleteRequested && repellentRestored.bFreeTargets && repellentRestored.bNoMoreSync &&
            repellentRestored.trans_SL.position.z == 6 &&
            repellentRestored.mission.dwMissionID == repellentSaved.mission.dwMissionID &&
            repellentRestored.mission.systemID == 1;
    }
    const SYMBOL repulsor = ResolveViewSymbolType(constructor->GetSymbol(symbols, "REPULSORWAVE_SAVELOAD"));
    REPULSORWAVE_SAVELOAD repulsorSaved{}, repulsorRestored{};
    repulsorSaved.targetPos.init(12, 24);
    repulsorSaved.ownerID = 0x12345672;
    repulsorSaved.time = 0.5f;
    repulsorSaved.numTargets = MAX_REPULSORWAVE_TARGETS;
    repulsorSaved.targets[MAX_REPULSORWAVE_TARGETS-1] = 0x12345681;
    repulsorSaved.targetsHeld = 1;
    repulsorSaved.lastSent = 2;
    repulsorSaved.bDeleteRequested = true;
    repulsorSaved.bFreeTargets = true;
    repulsorSaved.bNoMoreSync = true;
    repulsorSaved.bHasBeenInit = true;
    repulsorSaved.trans_SL.position = Vector(4, 5, 6);
    repulsorSaved.mission.dwMissionID = 0x12345682;
    repulsorSaved.mission.systemID = 1;
    if (!repulsor || repulsor->size != sizeof(repulsorSaved) ||
        reinterpret_cast<const unsigned char *>(&repulsorSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&repulsorSaved) != 124 ||
        reinterpret_cast<const unsigned char *>(&repulsorSaved.mission) -
            reinterpret_cast<const unsigned char *>(&repulsorSaved) + sizeof(MISSION_SAVELOAD) != 268) valid = false;
    else
    {
        constructor->CorrelateSymbol(repulsor, &repulsorSaved, repulsor, &repulsorRestored);
        valid = valid && repulsorRestored.targetPos.getX() == 12 && repulsorRestored.targetPos.getY() == 24 &&
            repulsorRestored.ownerID == repulsorSaved.ownerID && repulsorRestored.time == 0.5f &&
            repulsorRestored.numTargets == MAX_REPULSORWAVE_TARGETS &&
            repulsorRestored.targets[MAX_REPULSORWAVE_TARGETS-1] == repulsorSaved.targets[MAX_REPULSORWAVE_TARGETS-1] &&
            repulsorRestored.targetsHeld == 1 && repulsorRestored.lastSent == 2 &&
            repulsorRestored.bDeleteRequested && repulsorRestored.bFreeTargets &&
            repulsorRestored.bNoMoreSync && repulsorRestored.bHasBeenInit &&
            repulsorRestored.trans_SL.position.z == 6 &&
            repulsorRestored.mission.dwMissionID == repulsorSaved.mission.dwMissionID &&
            repulsorRestored.mission.systemID == 1;
    }
    const SYMBOL destabilizer = ResolveViewSymbolType(constructor->GetSymbol(symbols, "DESTABILIZER_SAVELOAD"));
    DESTABILIZER_SAVELOAD destabilizerSaved{}, destabilizerRestored{};
    destabilizerSaved.time = 0.5f;
    destabilizerSaved.targetPos.init(12, 24);
    destabilizerSaved.ownerID = 0x12345672;
    destabilizerSaved.numTargets = MAX_DESTABILIZER_TARGETS;
    destabilizerSaved.targetIDs[MAX_DESTABILIZER_TARGETS-1] = 0x12345681;
    destabilizerSaved.lastSent = 2;
    destabilizerSaved.targetID = 0x12345682;
    destabilizerSaved.targetsHeld = 1;
    destabilizerSaved.bDeleteRequested = true;
    destabilizerSaved.bFreeTargets = true;
    destabilizerSaved.bNoMoreSync = true;
    destabilizerSaved.trans_SL.position = Vector(4, 5, 6);
    destabilizerSaved.mission.dwMissionID = 0x12345683;
    destabilizerSaved.mission.systemID = 1;
    if (!destabilizer || destabilizer->size != sizeof(destabilizerSaved) ||
        reinterpret_cast<const unsigned char *>(&destabilizerSaved.trans_SL) -
            reinterpret_cast<const unsigned char *>(&destabilizerSaved) != 44 ||
        reinterpret_cast<const unsigned char *>(&destabilizerSaved.mission) -
            reinterpret_cast<const unsigned char *>(&destabilizerSaved) + sizeof(MISSION_SAVELOAD) != 188) valid = false;
    else
    {
        constructor->CorrelateSymbol(destabilizer, &destabilizerSaved, destabilizer, &destabilizerRestored);
        valid = valid && destabilizerRestored.time == 0.5f &&
            destabilizerRestored.targetPos.getX() == 12 && destabilizerRestored.targetPos.getY() == 24 &&
            destabilizerRestored.ownerID == destabilizerSaved.ownerID &&
            destabilizerRestored.numTargets == MAX_DESTABILIZER_TARGETS &&
            destabilizerRestored.targetIDs[MAX_DESTABILIZER_TARGETS-1] == destabilizerSaved.targetIDs[MAX_DESTABILIZER_TARGETS-1] &&
            destabilizerRestored.lastSent == 2 && destabilizerRestored.targetsHeld == 1 &&
            destabilizerRestored.targetID == destabilizerSaved.targetID &&
            destabilizerRestored.bDeleteRequested && destabilizerRestored.bFreeTargets && destabilizerRestored.bNoMoreSync &&
            destabilizerRestored.trans_SL.position.z == 6 &&
            destabilizerRestored.mission.dwMissionID == destabilizerSaved.mission.dwMissionID &&
            destabilizerRestored.mission.systemID == 1;
    }
    const SYMBOL tractor = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TRACTOR_SAVELOAD"));
    TRACTOR_SAVELOAD tractorSaved{}, tractorRestored{};
    tractorSaved.targetID = 0x12345681;
    tractorSaved.systemID = 2;
    tractorSaved.time = 1.5f;
    tractorSaved.refireDelay = 0.25f;
    tractorSaved.mass = 9120.0f;
    if (!tractor || tractor->size != sizeof(tractorSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(tractor, &tractorSaved, tractor, &tractorRestored);
        valid = valid && tractorRestored.targetID == tractorSaved.targetID &&
            tractorRestored.systemID == 2 && tractorRestored.time == 1.5f &&
            tractorRestored.refireDelay == 0.25f && tractorRestored.mass == 9120.0f;
    }
    const SYMBOL mimic = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MIMIC_SAVELOAD"));
    MIMIC_SAVELOAD mimicSaved{}, mimicRestored{};
    mimicSaved.aliasArchetypeID = 0x12345681;
    mimicSaved.aliasPlayerID = 2;
    mimicSaved.bCloakEnabled = true;
    if (!mimic || mimic->size != sizeof(mimicSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(mimic, &mimicSaved, mimic, &mimicRestored);
        valid = valid && mimicRestored.aliasArchetypeID == mimicSaved.aliasArchetypeID &&
            mimicRestored.aliasPlayerID == 2 && mimicRestored.bCloakEnabled;
    }
    const SYMBOL recon = ResolveViewSymbolType(constructor->GetSymbol(symbols, "RECON_LAUNCH_SAVELOAD"));
    RECON_LAUNCH_SAVELOAD reconSaved{}, reconRestored{};
    reconSaved.targetPos = Vector(1, 2, 3);
    reconSaved.targetSystemID = 1;
    reconSaved.attacking = 2;
    reconSaved.refireDelay = -1;
    reconSaved.probeID = 0x12345672;
    reconSaved.targetID = 0x12345681;
    reconSaved.bKillProbe = true;
    if (!recon || recon->size != sizeof(reconSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(recon, &reconSaved, recon, &reconRestored);
        valid = valid && reconRestored.targetPos.x == 1 && reconRestored.targetPos.y == 2 &&
            reconRestored.targetPos.z == 3 && reconRestored.targetSystemID == 1 &&
            reconRestored.attacking == 2 && reconRestored.refireDelay == -1 &&
            reconRestored.probeID == reconSaved.probeID && reconRestored.targetID == reconSaved.targetID &&
            reconRestored.bKillProbe;
    }
    const SYMBOL supplyShip = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "SUPPLYSHIP_SAVELOAD"));
    SUPPLYSHIP_SAVELOAD supplySaved{}, supplyRestored{};
    supplySaved.mission.dwMissionID = 0x12345671;
    supplySaved.baseSaveLoad.mode = SUP_MOVING_TARGETED_SUPPLY;
    supplySaved.baseSaveLoad.workingID = 123;
    supplySaved.baseSaveLoad.targetedTargetID = 0x12345681;
    supplySaved.baseSaveLoad.supplyTimer = 0.5f;
    supplySaved.baseSaveLoad.supplyEscortTargetID = 0x12345691;
    supplySaved.baseSaveLoad.supplyPlatformTargetID = 0x123456a1;
    supplySaved.baseSaveLoad.supplyStance = SUP_STANCE_RESUPPLY;
    supplySaved.baseSaveLoad.bNeedToSendMoveToTarget = true;
    supplySaved.baseSaveLoad.bStoreAgentID = true;
    std::cout << "Supply ship schema/native extent: "
        << (supplyShip ? supplyShip->size : -1) << '/' << sizeof(supplySaved) << '\n';
    // Original Save writes sizeof(SUPPLYSHIP_SAVELOAD); the empty trailing
    // sup_SL has no schema fields, so correlation covers the first 464 bytes.
    const SYMBOL emptySupply = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "SUP_SAVELOAD"));
    if (!supplyShip || supplyShip->size != 464 ||
        !emptySupply || emptySupply->size != 0 || sizeof(SUP_SAVELOAD) != 1 ||
        reinterpret_cast<const unsigned char *>(&supplySaved.baseSaveLoad) -
            reinterpret_cast<const unsigned char *>(&supplySaved) != 424 ||
        reinterpret_cast<const unsigned char *>(&supplySaved.sup_SL) -
            reinterpret_cast<const unsigned char *>(&supplySaved) != 464) valid = false;
    else
    {
        constructor->CorrelateSymbol(supplyShip, &supplySaved, supplyShip, &supplyRestored);
        valid = valid && supplyRestored.mission.dwMissionID == supplySaved.mission.dwMissionID &&
            supplyRestored.baseSaveLoad.mode == SUP_MOVING_TARGETED_SUPPLY &&
            supplyRestored.baseSaveLoad.workingID == 123 &&
            supplyRestored.baseSaveLoad.targetedTargetID == supplySaved.baseSaveLoad.targetedTargetID &&
            supplyRestored.baseSaveLoad.supplyTimer == 0.5f &&
            supplyRestored.baseSaveLoad.supplyEscortTargetID == supplySaved.baseSaveLoad.supplyEscortTargetID &&
            supplyRestored.baseSaveLoad.supplyPlatformTargetID == supplySaved.baseSaveLoad.supplyPlatformTargetID &&
            supplyRestored.baseSaveLoad.supplyStance == SUP_STANCE_RESUPPLY &&
            supplyRestored.baseSaveLoad.bNeedToSendMoveToTarget &&
            supplyRestored.baseSaveLoad.bStoreAgentID;
    }
    const SYMBOL pingArchetype = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "BT_PING_LAUNCHER"));
    const SYMBOL zealotArchetype = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "BT_ZEALOT_DATA"));
    const SYMBOL zealotRecord = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "ZEALOT_SAVELOAD"));
    const SYMBOL emptyPing = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "PING_LAUNCHER_SAVELOAD"));
    BT_PING_LAUNCHER pingSaved{}, pingRestored{};
    BT_ZEALOT_DATA zealotDataSaved{}, zealotDataRestored{};
    ZEALOT_SAVELOAD zealotSaved{}, zealotRestored{};
    pingSaved.techNode.common_extra = static_cast<TECHTREE::COMMON_EXTRA>(0x43);
    zealotDataSaved.kamikazeSpeed = 18.5F;
    zealotDataSaved.damageAmount[2] = 90.25F;
    std::strcpy(zealotDataSaved.impactBlastType, "GeneratedImpact");
    zealotDataSaved.neededTech.common_extra = static_cast<TECHTREE::COMMON_EXTRA>(0x21);
    zealotSaved.dwMissionID = 0x11223344U;
    zealotSaved.targetID = 0x55667788U;
    zealotSaved.zealotArchetypeID = 0x12345678U;
    zealotSaved.systemID = 5U;
    zealotSaved.stage = Z_THRUST;
    zealotSaved.trans_SL.position.x = 1.25F;
    zealotSaved.trans_SL.velocity.z = 9.5F;
    zealotSaved.trans_SL.ang_velocity.y = 0.75F;
    zealotSaved.visibilityFlags = 0xA5U;
    if (!pingArchetype || pingArchetype->size != sizeof(pingSaved) ||
        !zealotArchetype || zealotArchetype->size != sizeof(zealotDataSaved) ||
        !zealotRecord || zealotRecord->size != sizeof(zealotSaved) ||
        !emptyPing || emptyPing->size != 0U) valid = false;
    else
    {
        constructor->CorrelateSymbol(pingArchetype, &pingSaved, pingArchetype, &pingRestored);
        constructor->CorrelateSymbol(zealotArchetype, &zealotDataSaved, zealotArchetype, &zealotDataRestored);
        constructor->CorrelateSymbol(zealotRecord, &zealotSaved, zealotRecord, &zealotRestored);
        valid = valid && pingRestored.techNode.common_extra == pingSaved.techNode.common_extra &&
            zealotDataRestored.kamikazeSpeed == 18.5F && zealotDataRestored.damageAmount[2] == 90.25F &&
            std::strcmp(zealotDataRestored.impactBlastType, "GeneratedImpact") == 0 &&
            zealotDataRestored.neededTech.common_extra == zealotDataSaved.neededTech.common_extra &&
            zealotRestored.dwMissionID == zealotSaved.dwMissionID && zealotRestored.targetID == zealotSaved.targetID &&
            zealotRestored.zealotArchetypeID == zealotSaved.zealotArchetypeID && zealotRestored.systemID == 5U &&
            zealotRestored.stage == Z_THRUST && zealotRestored.trans_SL.position.x == 1.25F &&
            zealotRestored.trans_SL.velocity.z == 9.5F && zealotRestored.trans_SL.ang_velocity.y == 0.75F &&
            zealotRestored.visibilityFlags == 0xA5U;
    }
    const SYMBOL buildDrone = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "BT_BUILDERSHIP_DATA"));
    BT_BUILDERSHIP_DATA droneSaved{}, droneRestored{};
    const auto droneOffset = [&droneSaved](const void *field) {
        return static_cast<const unsigned char *>(field) -
            reinterpret_cast<const unsigned char *>(&droneSaved);
    };
    valid = valid && droneOffset(&droneSaved.type) == 8 &&
        droneOffset(droneSaved.fileName) == 12 &&
        droneOffset(&droneSaved.sparkDelay) + sizeof(SINGLE) == 192;
    droneSaved.objClass = OC_SPACESHIP;
    droneSaved.type = SSC_BUILDSHIP;
    std::strcpy(droneSaved.fileName, "repair-drone-fixture");
    droneSaved.dynamicsData.maxLinearVelocity = 123;
    droneSaved.sparkWidth = 12;
    droneSaved.sparkDelay = 34;
    if (!buildDrone || buildDrone->size != sizeof(droneSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(buildDrone, &droneSaved, buildDrone, &droneRestored);
        valid = valid && droneRestored.objClass == OC_SPACESHIP &&
            droneRestored.type == SSC_BUILDSHIP &&
            std::strcmp(droneRestored.fileName, droneSaved.fileName) == 0 &&
            droneRestored.dynamicsData.maxLinearVelocity == 123 &&
            droneRestored.sparkWidth == 12 && droneRestored.sparkDelay == 34;
    }
    const SYMBOL wing = ResolveViewSymbolType(constructor->GetSymbol(symbols, "FIGHTERWING_SAVELOAD"));
    FIGHTERWING_SAVELOAD wingSaved{}, wingRestored{};
    wingSaved.firstFighterID = 0x12345672;
    wingSaved.bBayDoorOpen = true;
    wingSaved.bAutoAttack = true;
    wingSaved.bSpecialWeapon = true;
    wingSaved.createTime = -1;
    wingSaved.minLaunchTime = 12;
    wingSaved.bayDoorCloseTime = 24;
    if (!wing || wing->size != sizeof(wingSaved)) valid = false;
    else
    {
        constructor->CorrelateSymbol(wing, &wingSaved, wing, &wingRestored);
        valid = valid && wingRestored.firstFighterID == wingSaved.firstFighterID &&
            wingRestored.bBayDoorOpen && wingRestored.bAutoAttack && wingRestored.bSpecialWeapon &&
            wingRestored.createTime == -1 && wingRestored.minLaunchTime == 12 &&
            wingRestored.bayDoorCloseTime == 24;
    }
    if (valid)
    {
        constructor->CorrelateSymbol(mission, &saved, mission, &restored);
        valid = caps->offset == offsetof(MISSION_SAVELOAD, caps) &&
            std::memcmp(&saved.caps, &restored.caps, sizeof(saved.caps)) == 0 &&
            saved.controlGroupID == restored.controlGroupID &&
            saved.groupIndex == restored.groupIndex &&
            saved.groupID == restored.groupID &&
            saved.pendingOp == restored.pendingOp;
        std::cout << "Mission capabilities schema/native offsets: "
                  << caps->offset << '/' << offsetof(MISSION_SAVELOAD, caps)
                  << "; correlated move/attack: " << restored.caps.moveOk
                  << '/' << restored.caps.attackOk << '\n';
    }
    for (const char * name : {"GUNBOAT_SAVELOAD", "FABRICATOR_SAVELOAD",
                              "HARVEST_SAVELOAD"})
    {
        const SYMBOL ship = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        const auto findMissionOffset = [](auto && self, SYMBOL type, int depth) -> int {
            type = ResolveViewSymbolType(type);
            if (!type || type->kind != RECORDSYM || depth > 4) return -1;
            for (SYMBOL field = type->datatype; field; field = field->link)
            {
                if (field->name && std::strcmp(field->name, "mission") == 0)
                    return field->offset;
                if (!field->name)
                {
                    const int inherited = self(self, field, depth + 1);
                    if (inherited >= 0) return field->offset + inherited;
                }
            }
            return -1;
        };
        const int missionOffset = findMissionOffset(findMissionOffset, ship, 0);
        std::cout << "Nested mission field " << name << ": " << missionOffset << '\n';
        if (!ship || ship->size <= 0 || missionOffset < 0 ||
            static_cast<std::size_t>(missionOffset) != offsetof(SPACESHIP_SAVELOAD, mission) ||
            static_cast<std::size_t>(missionOffset) + sizeof(saved) >
                static_cast<std::size_t>(ship->size))
        {
            valid = false;
            break;
        }
        std::vector<unsigned char> input(ship->size, 0), output(ship->size, 0);
        std::memcpy(input.data(), &movingShip, sizeof(movingShip));
        std::memcpy(input.data() + missionOffset, &saved, sizeof(saved));
        valid = valid && CorrelateNativeViewSymbols(constructor, ship, input.data(), ship, output.data());
        MISSION_SAVELOAD nested = {};
        std::memcpy(&nested, output.data() + missionOffset, sizeof(nested));
        valid = valid && std::memcmp(&saved.caps, &nested.caps, sizeof(saved.caps)) == 0 &&
            nested.controlGroupID == saved.controlGroupID && nested.groupID == saved.groupID &&
            nested.pendingOp == saved.pendingOp;
        SPACESHIP_SAVELOAD moved = {};
        std::memcpy(&moved, output.data(), sizeof(moved));
        valid = valid && moved.tobjmove.moveAgentID == movingShip.tobjmove.moveAgentID &&
            moved.tobjmove.jumpAgentID == movingShip.tobjmove.jumpAgentID &&
            moved.tobjmove.pathLength == movingShip.tobjmove.pathLength &&
            moved.tobjmove.overrideSpeed == movingShip.tobjmove.overrideSpeed &&
            moved.tobjmove.bMoveActive && moved.tobjmove.bCompletionAllowed &&
            moved.tobjmove.bFinalMove && moved.tobjmove.bSyncNeeded;
    }
    if (valid)
        std::cout << "App Data.i parsed; MT_GlobalData bytes: "
                  << record->size << '\n';
    constructor->DestroySymbols(symbols);
    return valid;
}

// First Terran MScript binding needs its own schema/resource rather than Sol_T's.
// This existing owner probe gates that binding, not full mission acceptance.
bool ParseTerranSchema(IViewConstructor2 *constructor, const std::filesystem::path &path)
{
    std::ifstream source(path, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(source)), {});
    std::size_t length = 0;
    const void *embedded = FrontierX::GetNativeScript01Schema(length);
    if (bytes.empty() || !embedded || length != bytes.size() ||
        std::memcmp(embedded, bytes.data(), length)) return false;
    std::string text = bytes;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const auto global = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MissionData"));
    const auto briefing = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TM1_Briefing_Save"));
    const auto start = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TM1_MissionStart_Save"));
    bool waypoint = false, blackwell = false, end = false;
    for (auto field = global ? global->datatype : nullptr; field; field = field->link)
    {
        if (!field->name) continue;
        if (!std::strcmp(field->name, "WayPoint1")) waypoint = field->offset == 72;
        if (!std::strcmp(field->name, "Blackwell")) blackwell = field->offset == 152;
        if (!std::strcmp(field->name, "failureID")) end = field->offset + field->size == 376;
    }
    const auto name = FrontierX::GetNativeScript01CountedString(1055);
    const bool valid = global && global->size == 376 && waypoint && blackwell && end &&
        briefing && briefing->size == 32 && start && start->size == 4 &&
        name && name[0] == 20 && std::wmemcmp(name+1, L"UNCHARTED TERRITORY.", 20) == 0 && !FrontierX::GetNativeScript01CountedString(0);
    constructor->DestroySymbols(symbols);
    if (valid) std::cout << "Script01T source schema/resource and fixed-width records checked\n";
    return valid;
}

// TM1 Continue selects TM2's independent MScript schema/resource. Extend the
// existing parser gate: a Sol_T/TM1 schema cannot restore TM2's saved actors.
bool ParseSecondTerranSchema(IViewConstructor2 *constructor, const std::filesystem::path &path)
{
    std::ifstream source(path, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(source)), {});
    std::size_t length = 0;
    const void *embedded = FrontierX::GetNativeScript02Schema(length);
    if (bytes.empty() || !embedded || length != bytes.size() ||
        std::memcmp(embedded, bytes.data(), length)) return false;
    std::string text = bytes;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const auto global = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MissionData"));
    const auto briefing = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TM2_Briefing_Data"));
    const auto scene = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TM2_InGameScene_Save"));
    const auto jump = ResolveViewSymbolType(constructor->GetSymbol(symbols, "TM2_ForbiddenJump_Data"));
    bool corvette = false, blackwell = false, end = false;
    for (auto field = global ? global->datatype : nullptr; field; field = field->link)
    {
        if (!field->name) continue;
        if (!std::strcmp(field->name, "corvette1")) corvette = field->offset == 36;
        if (!std::strcmp(field->name, "Blackwell")) blackwell = field->offset == 132;
        if (!std::strcmp(field->name, "failureID")) end = field->offset + field->size == 408;
    }
    const auto name = FrontierX::GetNativeScript02CountedString(2014);
    bool valid = global && global->size == 408 && corvette && blackwell && end &&
        briefing && briefing->size == 24 && scene && scene->size == 120 &&
        jump && jump->size == 32 && name && name[0] == 20 &&
        std::wmemcmp(name + 1, L"Rescue Captured Crew", 20) == 0 &&
        !FrontierX::GetNativeScript02CountedString(0);
    if (valid)
    {
        std::vector<unsigned char> saved(408, 0), restored(408, 0);
        const U32 actorID = 0x12345671U, failureID = 42U;
        std::memcpy(saved.data() + 136, &actorID, sizeof(actorID));
        std::memcpy(saved.data() + 404, &failureID, sizeof(failureID));
        valid = CorrelateNativeViewSymbols(constructor, global, saved.data(), global, restored.data()) &&
            std::memcmp(saved.data() + 136, restored.data() + 136, sizeof(actorID)) == 0 &&
            std::memcmp(saved.data() + 404, restored.data() + 404, sizeof(failureID)) == 0;
    }
    constructor->DestroySymbols(symbols);
    if (valid) std::cout << "Script02T source schema/resource and saved actor IDs checked\n";
    return valid;
}

// Original TM2 Continue loads Script03, not the previous module's schema.
bool ParseThirdTerranSchema(IViewConstructor2 *constructor, const std::filesystem::path &path)
{
    std::ifstream source(path, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(source)), {});
    std::size_t length = 0;
    const void *embedded = FrontierX::GetNativeScript03Schema(length);
    if (bytes.empty() || !embedded || length != bytes.size() ||
        std::memcmp(embedded, bytes.data(), length)) return false;
    std::string text = bytes;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const auto global = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MissionData"));
    const auto extent = [&](const char *name, int size) {
        const auto symbol = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        return symbol && symbol->size == size;
    };
    bool blackwell = false, bin = false, end = false;
    for (auto field = global ? global->datatype : nullptr; field; field = field->link)
    {
        if (!field->name) continue;
        if (!std::strcmp(field->name, "blackwellsShip")) blackwell = field->offset == 148;
        if (!std::strcmp(field->name, "mantisBuildBin")) bin = field->offset == 372 && field->size == 64;
        if (!std::strcmp(field->name, "failureID")) end = field->offset + field->size == 492;
    }
    const auto objective = FrontierX::GetNativeScript03CountedString(3088);
    constexpr wchar_t objectiveText[] = L"Troopship Hawkes' prison.";
    bool valid = global && global->size == 492 && blackwell && bin && end &&
        extent("TM3_BriefingData", 12) && extent("TM3_MissionIntroData", 216) &&
        extent("TM3_InitialMantisAttackData", 1) && extent("TM3_VisibilityHandlerData", 24) &&
        extent("TM3_BlackwellIsDeadData", 8) && extent("TM3_CheckForCapturedPrisonData", 12) &&
        extent("TM3_DisplayObjectiveHandlerData", 8) && extent("TM3_MissionStartData", 0) &&
        extent("TM3_MissionSuccessData", 0) && objective &&
        objective[0] == std::size(objectiveText) - 1 &&
        std::wmemcmp(objective + 1, objectiveText, std::size(objectiveText) - 1) == 0 &&
        !FrontierX::GetNativeScript03CountedString(0);
    if (valid)
    {
        std::vector<unsigned char> saved(492, 0), restored(492, 0);
        const U32 actorID = 0x12345671U, lastBinActor = 0x23456781U, failureID = 42U;
        std::memcpy(saved.data() + 152, &actorID, sizeof(actorID));
        std::memcpy(saved.data() + 424, &lastBinActor, sizeof(lastBinActor));
        std::memcpy(saved.data() + 488, &failureID, sizeof(failureID));
        valid = CorrelateNativeViewSymbols(constructor, global, saved.data(), global, restored.data()) &&
            std::memcmp(saved.data(), restored.data(), saved.size()) == 0;
    }
    constructor->DestroySymbols(symbols);
    if (valid) std::cout << "Script03T source schema/resource, actor array and empty programs checked\n";
    return valid;
}

// TM3 Continue needs TM4's own actor references and 64-entry retreat list.
// Existing schema correlation protects the actual MScript persistent contract;
// this does not exercise mission objectives or imply gameplay acceptance.
bool ParseFourthTerranSchema(IViewConstructor2 *constructor, const std::filesystem::path &path)
{
    std::ifstream source(path, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(source)), {});
    std::size_t length = 0;
    const void *embedded = FrontierX::GetNativeScript04Schema(length);
    if (bytes.empty() || !embedded || length != bytes.size() ||
        std::memcmp(embedded, bytes.data(), length)) return false;
    std::string text = bytes;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const auto global = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MissionData"));
    const auto extent = [&](const char *name, int size) {
        const auto symbol = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        return symbol && symbol->size == size;
    };
    bool blackwell = false, hawkes = false, exits = false, end = false;
    for (auto field = global ? global->datatype : nullptr; field; field = field->link)
    {
        if (!field->name) continue;
        if (!std::strcmp(field->name, "Blackwell")) blackwell = field->offset == 32;
        if (!std::strcmp(field->name, "Hawkes")) hawkes = field->offset == 48;
        if (!std::strcmp(field->name, "exitID")) exits = field->offset == 176 && field->size == 256;
        if (!std::strcmp(field->name, "failureID")) end = field->offset + field->size == 436;
    }
    const auto objective = FrontierX::GetNativeScript04CountedString(4073);
    constexpr wchar_t objectiveText[] = L"Blackwell and Hawkes must survive";
    bool valid = global && global->size == 436 && blackwell && hawkes && exits && end &&
        extent("TM4_Briefing_Data", 24) && extent("TM4_MissionStart_Data", 8) &&
        extent("TM4_ControlProgram_Data", 1) && extent("TM4_SendLuxorWave_Data", 8) &&
        extent("TM4_InvestigateAlto_Data", 8) && extent("TM4_RetreatToCapella_Data", 8) &&
        extent("TM4_DisplayObjectiveHandlerData", 8) && extent("TM4_Start_Data", 0) &&
        extent("TM4_ProtectRebels_Data", 0) && objective &&
        objective[0] == std::size(objectiveText) - 1 &&
        std::wmemcmp(objective + 1, objectiveText, std::size(objectiveText) - 1) == 0 &&
        !FrontierX::GetNativeScript04CountedString(0);
    if (valid)
    {
        std::vector<unsigned char> saved(436, 0), restored(436, 0);
        const U32 blackwellID = 0x12345671U, hawkesID = 0x23456781U;
        const U32 exitCount = 64, lastExitID = 0x34567891U, failureID = 42;
        std::memcpy(saved.data() + 36, &blackwellID, sizeof(blackwellID));
        std::memcpy(saved.data() + 52, &hawkesID, sizeof(hawkesID));
        std::memcpy(saved.data() + 172, &exitCount, sizeof(exitCount));
        std::memcpy(saved.data() + 428, &lastExitID, sizeof(lastExitID));
        std::memcpy(saved.data() + 432, &failureID, sizeof(failureID));
        valid = CorrelateNativeViewSymbols(constructor, global, saved.data(), global, restored.data()) &&
            std::memcmp(saved.data(), restored.data(), saved.size()) == 0;
    }
    constructor->DestroySymbols(symbols);
    if (valid) std::cout << "Script04T source schema/resource, saved admirals and retreat list checked\n";
    return valid;
}

// TM4 Continue must bind the next original briefing's independent schema.
// This is technical transition preparation, not TM5 gameplay readiness.
bool ParseFifthTerranSchema(IViewConstructor2 *constructor, const std::filesystem::path &path)
{
    std::ifstream source(path, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(source)), {});
    std::size_t length = 0;
    const void *embedded = FrontierX::GetNativeScript05Schema(length);
    if (bytes.empty() || !embedded || length != bytes.size() ||
        std::memcmp(embedded, bytes.data(), length)) return false;
    std::string text = bytes;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const auto global = ResolveViewSymbolType(constructor->GetSymbol(symbols, "MissionData"));
    const auto extent = [&](const char *name, int size) {
        const auto symbol = ResolveViewSymbolType(constructor->GetSymbol(symbols, name));
        return symbol && symbol->size == size;
    };
    bool blackwell = false, hawkes = false, kertak = false, end = false;
    for (auto field = global ? global->datatype : nullptr; field; field = field->link)
    {
        if (!field->name) continue;
        if (!std::strcmp(field->name, "Blackwell")) blackwell = field->offset == 248;
        if (!std::strcmp(field->name, "Hawkes")) hawkes = field->offset == 264;
        if (!std::strcmp(field->name, "KerTak")) kertak = field->offset == 456;
        if (!std::strcmp(field->name, "failureID")) end = field->offset + field->size == 604;
    }
    const auto name = FrontierX::GetNativeScript05CountedString(5020);
    constexpr wchar_t missionName[] = L"Terrans strike back";
    bool valid = global && global->size == 604 && blackwell && hawkes && kertak && end &&
        extent("TM5_Briefing_Save", 4) && extent("TM5_MissionStart_Save", 4) &&
        extent("TM5_AIManager_Save", 1) && extent("TM5_PerilonCleared_Save", 68) &&
        extent("TM5_SendMantisWave1_Save", 36) && extent("TM5_SendMantisWave2_Save", 36) &&
        extent("TM5_IntroduceKerTak_Save", 36) && extent("TM5_ClearOutLuxor_Save", 24) &&
        extent("TM5_Start_Save", 0) && name && name[0] == std::size(missionName) - 1 &&
        std::wmemcmp(name + 1, missionName, std::size(missionName) - 1) == 0 &&
        !FrontierX::GetNativeScript05CountedString(0);
    if (valid)
    {
        std::vector<unsigned char> saved(604, 0), restored(604, 0);
        const U32 admiralID = 0x12345671U, rebelID = 0x23456781U, gateID = 0x34567891U, failureID = 42;
        std::memcpy(saved.data() + 252, &admiralID, sizeof(admiralID));
        std::memcpy(saved.data() + 460, &rebelID, sizeof(rebelID));
        std::memcpy(saved.data() + 588, &gateID, sizeof(gateID));
        std::memcpy(saved.data() + 600, &failureID, sizeof(failureID));
        valid = CorrelateNativeViewSymbols(constructor, global, saved.data(), global, restored.data()) &&
            std::memcmp(saved.data(), restored.data(), saved.size()) == 0;
    }
    constructor->DestroySymbols(symbols);
    if (valid) std::cout << "Script05T transition schema/resource and actor references checked; gameplay untested\n";
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
    if (!FrontierX::ParseNativeAppSchema(constructor)) return false;
    const HANDLE symbols = constructor->ParseNewMemory(text.data());
    if (!symbols) return false;
    const SYMBOL missionRecord = ResolveViewSymbolType(
        constructor->GetSymbol(symbols, "MISSION_SAVELOAD"));
    for (SYMBOL field = missionRecord ? missionRecord->datatype : nullptr;
         field; field = field->link)
        if (field->name && std::strcmp(field->name, "caps") == 0)
            std::cout << "External mission capability schema offset/size: "
                      << field->offset << '/' << field->size
                      << "; alignment/kind/type: " << field->alignment << '/'
                      << field->kind << '/' << ResolveViewSymbolType(field)->kind << '\n';
    // Mission::CorrelateSymbol converts external saved records into the App
    // resource schema. Preserve capabilities in both directions, without
    // changing the retail record layout or inventing gameplay state.
    const SYMBOL nativeMission = ResolveViewSymbolType(
        constructor->GetSymbol(0, "MISSION_SAVELOAD"));
    MISSION_SAVELOAD sourceMission = {};
    // Retail schema predates the final two CQ2 capabilities. Test all of its
    // declared bits; newer undeclared bits are outside that correlation.
    const unsigned char allCaps[] = {0xA5, 0xFF, 0xFF, 0x3F};
    std::memcpy(&sourceMission.caps, allCaps, sizeof(sourceMission.caps));
    sourceMission.controlGroupID = 3;
    sourceMission.groupID = 0x12345678;
    MISSION_SAVELOAD nativeRecord = {}, externalRecord = {};
    if (!missionRecord || !nativeMission ||
        missionRecord->size != sizeof(sourceMission) ||
        nativeMission->size != sizeof(sourceMission)) return false;
    constructor->CorrelateSymbol(missionRecord, &sourceMission,
                                 nativeMission, &nativeRecord);
    constructor->CorrelateSymbol(nativeMission, &nativeRecord,
                                 missionRecord, &externalRecord);
    if (std::memcmp(&sourceMission.caps, &nativeRecord.caps,
                    sizeof(sourceMission.caps)) != 0 ||
        std::memcmp(&sourceMission.caps, &externalRecord.caps,
                    sizeof(sourceMission.caps)) != 0 ||
        externalRecord.controlGroupID != sourceMission.controlGroupID ||
        externalRecord.groupID != sourceMission.groupID) return false;
    std::cout << "External/App mission capabilities preserved in both directions\n";
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

void DACOM_CDECL CaptureSemanticText(const char *bytes, U32 length, void *context)
{
    static_cast<std::string *>(context)->append(bytes, length);
}

bool CheckScriptSemanticFields(IViewConstructor2 *constructor)
{
    const HANDLE symbols = constructor->ParseNewMemory(
        "struct MPartRef { unsigned int index; unsigned int dwMissionID; void *pSave; void *pInit; };"
        "struct Observed { unsigned char marker; MPartRef ref; float value; unsigned int low:3; unsigned int high:5; int items[2]; enum { Idle, Active } state; };"
        "struct Empty {}; struct Unsafe { void *pointer; };");
    auto observe = [&](const char *name, const void *bytes, U32 length, std::string &text)
    {
        DPARSERDESC descriptor;
        descriptor.symbol = constructor->GetSymbol(symbols, name);
        IDataParser *parser = nullptr;
        if (!descriptor.symbol || constructor->CreateInstance(&descriptor,
                reinterpret_cast<void **>(&parser)) != GR_OK || !parser) return false;
        FrontierXM3::SemanticWriter writer(CaptureSemanticText, &text);
        const bool valid = writer.Fields(parser, bytes, length, false, 0, descriptor.symbol);
        parser->Release();
        return valid;
    };
    unsigned char bytes[40];
    std::memset(bytes, 0xa5, sizeof(bytes));
    bytes[0] = 7;
    auto put = [&](U32 offset, U32 value) { std::memcpy(bytes + offset, &value, sizeof(value)); };
    put(8, 0x1234); put(20, 0x80000000); put(24, 0xaa); put(28, 1); put(32, 2); put(36, 1);
    std::string first, normalized, changed, empty, unsafe;
    bool valid = symbols && observe("Observed", bytes, sizeof(bytes), first) &&
        first.find("<anonymous enum>") != std::string::npos;
    bytes[1] = 1; bytes[2] = 2; bytes[3] = 3;
    put(4, 9); put(12, 0x87654321); put(16, 0x11223344);
    valid = valid && observe("Observed", bytes, sizeof(bytes), normalized) && first == normalized;
    put(8, 0x4321);
    valid = valid && observe("Observed", bytes, sizeof(bytes), changed) && first != changed;
    valid = valid && observe("Empty", bytes, 1, empty) && empty == "[]" &&
        !observe("Unsafe", bytes, 4, unsafe);
    if (symbols) constructor->DestroySymbols(symbols);
    return Expect(valid, "script semantic fields preserve IDs, reject pointers and omit only padding/cache slots");
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
    const bool terranSchema = argc == 3 && std::strcmp(argv[1], "--terran-script-schema") == 0;
    const bool secondTerranSchema = argc == 3 && std::strcmp(argv[1], "--second-terran-script-schema") == 0;
    const bool thirdTerranSchema = argc == 3 && std::strcmp(argv[1], "--third-terran-script-schema") == 0;
    const bool fourthTerranSchema = argc == 3 && std::strcmp(argv[1], "--fourth-terran-script-schema") == 0;
    const bool fifthTerranSchema = argc == 3 && std::strcmp(argv[1], "--fifth-terran-script-schema") == 0;
    const bool missionSchema = argc == 4 &&
        std::strcmp(argv[1], "--mission-schema") == 0 &&
        !std::strchr(argv[3], '/') && !std::strchr(argv[3], '\\');
    if (argc != 1 && !appSchema && !scriptSchema && !terranSchema && !secondTerranSchema && !thirdTerranSchema && !fourthTerranSchema && !fifthTerranSchema && !missionSchema)
    {
        std::cerr << "Usage: frontierx_view_parser_probe [--app-schema data.i | "
                     "--script-schema DataDef.i | "
                     "--terran-script-schema DataDef.i | "
                     "--second-terran-script-schema DataDef.i | "
                     "--third-terran-script-schema DataDef.i | "
                     "--fourth-terran-script-schema DataDef.i | "
                     "--fifth-terran-script-schema DataDef.i | "
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
    if (terranSchema && !Expect(ParseTerranSchema(view2, argv[2]),
                                "selected Script01T schema/resource")) return 1;
    if (secondTerranSchema && !Expect(ParseSecondTerranSchema(view2, argv[2]),
                                "selected Script02T schema/resource")) return 1;
    if (thirdTerranSchema && !Expect(ParseThirdTerranSchema(view2, argv[2]),
                                "selected Script03T schema/resource")) return 1;
    if (fourthTerranSchema && !Expect(ParseFourthTerranSchema(view2, argv[2]),
                                "selected Script04T schema/resource")) return 1;
    if (fifthTerranSchema && !Expect(ParseFifthTerranSchema(view2, argv[2]),
                                "selected Script05T transition schema/resource")) return 1;
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
    const bool reorderedCorrelated = CorrelateNativeViewSymbols(view2, oldRecord, oldData, newRecord, newData);
    if (!Expect(reorderedCorrelated && newData[0] == oldData[1] && newData[1] == oldData[0],
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
    Symbol basicInteger;
    basicInteger.kind = BASICTYPE; basicInteger.basicdt = INTEGER; basicInteger.size = 4;
    std::uint32_t basicValue = 0xdeadbeefU;
    if (!Expect(CorrelateNativeViewSymbols(view2, &oldEnum, &oldEnumValue, &basicInteger, &basicValue) && basicValue == 0U,
                "known enum identifier to basic integer follows Windows text conversion to zero")) return 1;
    Symbol packedEnum = oldEnum;
    packedEnum.bit_count = 3; packedEnum.bit_offset = 7;
    Symbol packedInteger = basicInteger;
    packedInteger.bit_count = 3; packedInteger.bit_offset = 12;
    std::uint32_t packedSource = (0xa5a5a5a5U & ~(7U << 7)) | (1U << 7);
    std::uint32_t packedDestination = 0xf0f0f0f0U;
    const std::uint32_t preservedNeighbors = packedDestination & ~(7U << 12);
    if (!Expect(CopyViewSymbolValue(&packedEnum, 0, reinterpret_cast<const char *>(&packedSource),
                    &packedInteger, 0, reinterpret_cast<char *>(&packedDestination)) &&
                packedDestination == preservedNeighbors,
                "enum identifier to packed integer clears only destination bits and preserves neighbors")) return 1;
    const auto rejectsEnumCopy = [&](Symbol source, Symbol destination) {
        std::uint32_t unchanged = 0xdeadbeefU;
        const BOOL32 result = CopyViewSymbolValue(&source, 0, reinterpret_cast<const char *>(&oldEnumValue),
                    &destination, 0, reinterpret_cast<char *>(&unchanged));
        if (result || unchanged != 0xdeadbeefU) std::cerr << "enum reject type=" << destination.basicdt << " size=" << destination.size << " bits=" << destination.bit_offset << '/' << destination.bit_count << " result=" << result << " value=" << unchanged << '\n';
        return !result && unchanged == 0xdeadbeefU;
    };
    Symbol realDestination = basicInteger; realDestination.basicdt = REAL;
    Symbol stringDestination = basicInteger; stringDestination.basicdt = STRINGTYPE;
    Symbol shortDestination = basicInteger; shortDestination.size = 2;
    Symbol invalidBits = packedInteger; invalidBits.bit_offset = 63;
    Symbol invalidName = oldEntry; invalidName.name = const_cast<char *>("123");
    Symbol invalidEnum = oldEnum; invalidEnum.datatype = &invalidName;
    if (!Expect(rejectsEnumCopy(oldEnum, realDestination) && rejectsEnumCopy(oldEnum, stringDestination) &&
                rejectsEnumCopy(oldEnum, shortDestination) && rejectsEnumCopy(oldEnum, invalidBits) &&
                rejectsEnumCopy(invalidEnum, basicInteger),
                "enum text conversion rejects unsupported type, width, bits and nonidentifier")) return 1;
    std::uint32_t unknownPackedSource = (0xa5a5a5a5U & ~(7U << 7)) | (7U << 7);
    std::uint32_t unknownPackedDestination = 0xf0f0f0f0U;
    if (!Expect(!CopyViewSymbolValue(&packedEnum, 0, reinterpret_cast<const char *>(&unknownPackedSource),
                    &packedInteger, 0, reinterpret_cast<char *>(&unknownPackedDestination)) &&
                unknownPackedDestination == 0xf0f0f0f0U,
                "unknown packed enum rejects without successful untouched placeholder or neighbor mutation")) return 1;
    std::uint32_t unknownWhole = 99U, unknownWholeCopy = 0U;
    if (!Expect(CopyViewSymbolValue(&oldEnum, 0, reinterpret_cast<const char *>(&unknownWhole),
                    &basicInteger, 0, reinterpret_cast<char *>(&unknownWholeCopy)) && unknownWholeCopy == 99U,
                "unknown nonpacked enum retains original Windows raw numeric copy")) return 1;
    const char *anonymousSchema = "struct AnonymousRecord { int marker; union { float overrideSpeed; float overrideYaw; }; int tail; };";
    HANDLE oldAnonymousSymbols = view2->ParseNewMemory(anonymousSchema);
    HANDLE newAnonymousSymbols = view2->ParseNewMemory(anonymousSchema);
    SYMBOL oldAnonymous = view2->GetSymbol(oldAnonymousSymbols, "AnonymousRecord");
    SYMBOL newAnonymous = view2->GetSymbol(newAnonymousSymbols, "AnonymousRecord");
    const std::uint32_t anonymousData[3] = {0x12345678U, 0x3fa00000U, 0x87654321U};
    std::uint32_t anonymousCopy[3]{};
    if (!Expect(oldAnonymousSymbols && newAnonymousSymbols && oldAnonymous && newAnonymous &&
                oldAnonymous->size == 12 && newAnonymous->size == 12 &&
                CorrelateNativeViewSymbols(view2, oldAnonymous, anonymousData, newAnonymous, anonymousCopy) &&
                std::memcmp(anonymousData, anonymousCopy, sizeof(anonymousData)) == 0,
                "genuine provider matches unique unnamed union and copies nonzero original payload")) return 1;
    SYMBOL unnamedField = nullptr;
    for (SYMBOL field = oldAnonymous->datatype; field; field = field->link)
        if (!field->name && !ResolveViewSymbolType(field)->name) unnamedField = field;
    if (!Expect(unnamedField && unnamedField->offset == 4 && unnamedField->size == 4,
                "production parser exposes expected nameless union field extent")) return 1;
    SYMBOL unnamedType = ResolveViewSymbolType(unnamedField);
    SYMBOL firstMember = unnamedType->datatype;
    const auto rejectsAnonymous = [&]() {
        return !CorrelateNativeViewSymbols(view2, oldAnonymous, anonymousData, newAnonymous, anonymousCopy);
    };
    char *originalMemberName = firstMember->name;
    firstMember->name = const_cast<char *>("MissingOverride");
    const bool memberNameRejected = rejectsAnonymous(); firstMember->name = originalMemberName;
    const int originalOffset = unnamedField->offset;
    unnamedField->offset = 8; const bool offsetRejected = rejectsAnonymous(); unnamedField->offset = originalOffset;
    const int originalSize = unnamedField->size;
    unnamedField->size = 8; const bool sizeRejected = rejectsAnonymous(); unnamedField->size = originalSize;
    SYMBOL originalMemberType = firstMember->datatype;
    Symbol incompatibleMemberType = *ResolveViewSymbolType(firstMember);
    incompatibleMemberType.basicdt = INTEGER;
    firstMember->datatype = &incompatibleMemberType;
    const bool typeRejected = rejectsAnonymous(); firstMember->datatype = originalMemberType;
    Symbol duplicateUnion = *unnamedField;
    duplicateUnion.link = unnamedField->link;
    unnamedField->link = &duplicateUnion;
    const bool ambiguityRejected = rejectsAnonymous(); unnamedField->link = duplicateUnion.link;
    std::cerr << "Anonymous negative predicates member/offset/size/type/ambiguity: "
              << memberNameRejected << '/' << offsetRejected << '/' << sizeRejected << '/'
              << typeRejected << '/' << ambiguityRejected << '\n';
    if (!Expect(memberNameRejected && offsetRejected && sizeRejected && typeRejected && ambiguityRejected,
                "nameless union structural match rejects member/type/size/offset mismatch and duplicate ambiguity")) return 1;
    view2->DestroySymbols(newAnonymousSymbols); view2->DestroySymbols(oldAnonymousSymbols);
    HANDLE sameSymbols = view2->ParseNewMemory(
        "struct MigrationRecord { int alpha; int beta; };");
    SYMBOL sameRecord = view2->GetSymbol(sameSymbols, "MigrationRecord");
    if (!Expect(sameSymbols && sameRecord &&
                view2->IsEqual(oldRecord, sameRecord),
                "same schema direct-copy decision across parsed tables")) return 1;
    std::uint32_t sameData[2]{};
    if (!Expect(CorrelateNativeViewSymbols(view2, oldRecord, oldData, sameRecord, sameData) &&
                std::memcmp(sameData, oldData, sizeof(oldData)) == 0,
                "native real provider returns successful canonical correlation")) return 1;
    HANDLE incompleteSymbols = view2->ParseNewMemory("struct MigrationRecord { int alpha; int absent; };");
    SYMBOL incompleteRecord = view2->GetSymbol(incompleteSymbols, "MigrationRecord");
    std::uint32_t incompleteData[2]{0xdeadbeefU, 0xdeadbeefU};
    if (!Expect(incompleteSymbols && incompleteRecord &&
                !CorrelateNativeViewSymbols(view2, oldRecord, oldData, incompleteRecord, incompleteData) &&
                incompleteData[0] == oldData[0] && incompleteData[1] == 0U &&
                !CorrelateNativeViewSymbols(view2, nullptr, oldData, newRecord, newData) &&
                !CorrelateNativeViewSymbols(nullptr, oldRecord, oldData, newRecord, newData),
                "native real provider rejects missing fields and unknown provider/symbol")) return 1;
    view2->DestroySymbols(incompleteSymbols);
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
    const bool nestedCorrelated = CorrelateNativeViewSymbols(view2, oldArrayRecord, oldArrayData,
                           newArrayRecord, newArrayData);
    constexpr std::uint32_t expectedArrayData[]{15U, 12U, 11U, 14U, 13U, 10U};
    if (!Expect(nestedCorrelated && std::memcmp(newArrayData, expectedArrayData,
                            sizeof(expectedArrayData)) == 0,
                "production nested array and record correlation")) return 1;
    view2->DestroySymbols(newNestedSymbols);
    view2->DestroySymbols(oldNestedSymbols);
    view2->DestroySymbols(newSymbols);
    view2->DestroySymbols(oldSymbols);
    if (!CheckScriptSemanticFields(view2)) return 1;
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
