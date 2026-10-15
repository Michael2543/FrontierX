#include <DMissionDataLayout.h>
#include <DMissionEnum.h>
#include <MPartRef.h>

#include "Include/NativeAppGlobals.h"
#include "Include/NativeRetailFlagshipData.h"
#include "CQTrace.h"
#include "resource.h"
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
#include <DataDef.h>
#include <DMBaseData.h>
#include <DMovieCamera.h>
#include <DVLaunch.h>
#include <DCloakLauncher.h>
#include <DFlagship.h>
#include <DTroopship.h>
#include <DWormholeLauncher.h>
#include <DMultiCloakLauncher.h>
#include <DButton.h>
#include <DPingLauncher.h>
#include <DSpecial.h>
#include "Scripts/Include/ScriptDef.h"
#include <TComponent.h>
// Source API returns const scalars; top-level return qualifiers have no ABI effect.
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif
#include "SPlayerAI.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <array>
#include <limits>
// Script01T DLL-local records use the same fixed-width storage contract as Sol_T.
#undef DATADEF_H
namespace Script01Layout
{
#include "Scripts/Script02T/Script01T/DataDef.h"
}
static_assert(sizeof(Script01Layout::MissionData) == 376);
static_assert(offsetof(Script01Layout::MissionData, WayPoint1) == 72);
static_assert(offsetof(Script01Layout::MissionData, Blackwell) == 152);
static_assert(offsetof(Script01Layout::MissionData, failureID) + sizeof(U32) == 376);
static_assert(sizeof(Script01Layout::TM1_Briefing_Save) == 32);
static_assert(sizeof(Script01Layout::TM1_MissionStart_Save) == 4);

// TM2 MScript saves these source records through its selected script schema.
// Keep the Win32 MPartRef slots even when native debugger pointers are wider.
#undef DATADEF_H
namespace Script02Layout
{
#include "Scripts/Script02T/DataDef.h"
}
static_assert(sizeof(Script02Layout::MissionData) == 408);
static_assert(offsetof(Script02Layout::MissionData, corvette1) == 36);
static_assert(offsetof(Script02Layout::MissionData, Blackwell) == 132);
static_assert(offsetof(Script02Layout::MissionData, failureID) + sizeof(U32) == 408);
static_assert(sizeof(Script02Layout::TM2_Briefing_Data) == 24);
static_assert(sizeof(Script02Layout::TM2_InGameScene_Save) == 120);
static_assert(offsetof(Script02Layout::TM2_InGameScene_Save, timer) == 112);
static_assert(sizeof(Script02Layout::TM2_MantisAttack_Data) == 24);
static_assert(sizeof(Script02Layout::TM2_ScoutAlkaid_Data) == 8);
static_assert(sizeof(Script02Layout::TM2_ForbiddenJump_Data) == 32);

// TM2 Continue reaches Script03's distinct Win32-shaped actor/program records.
#undef DATADEF_H
namespace Script03Layout
{
#include "Scripts/Script03T/DataDef.h"
}
static_assert(sizeof(Script03Layout::MissionData) == 492);
static_assert(offsetof(Script03Layout::MissionData, blackwellsBogeysTrigger) == 68);
static_assert(offsetof(Script03Layout::MissionData, blackwellsShip) == 148);
static_assert(offsetof(Script03Layout::MissionData, mantisBuildBin) == 372);
static_assert(offsetof(Script03Layout::MissionData, mantisBuildBinIndex) == 436);
static_assert(offsetof(Script03Layout::MissionData, failureID) + sizeof(U32) == 492);
static_assert(sizeof(Script03Layout::TM3_BriefingData) == 12);
static_assert(sizeof(Script03Layout::TM3_MissionIntroData) == 216);
static_assert(offsetof(Script03Layout::TM3_MissionIntroData, mantisFrigates) == 8);
static_assert(offsetof(Script03Layout::TM3_MissionIntroData, terranRepair) == 184);
static_assert(sizeof(Script03Layout::TM3_InitialMantisAttackData) == 1);
static_assert(sizeof(Script03Layout::TM3_VisibilityHandlerData) == 24);
static_assert(sizeof(Script03Layout::TM3_BlackwellIsDeadData) == 8);
static_assert(sizeof(Script03Layout::TM3_CheckForCapturedPrisonData) == 12);
static_assert(sizeof(Script03Layout::TM3_DisplayObjectiveHandlerData) == 8);
// TM2_MissionStart changes to the authored MissionCamStart object; its normal
// Mission save/load uses these original fixed-width camera records.
static_assert(sizeof(BASE_MOVIE_CAMERA_SAVELOAD) == 136);
static_assert(sizeof(MOVIE_CAMERA_SAVELOAD) == 176);
static_assert(sizeof(BT_MOVIE_CAMERA_DATA) == 112);
static_assert(sizeof(MOVIE_CAMERA_VIEW) == 64);
// BuildPlat creates the Terran Missile Cruiser through Gunboat's real vertical
// and cloak launcher factories. Their data remains the original Win32 schema.
static_assert(sizeof(BT_VERTICAL_LAUNCH) == 796);
static_assert(sizeof(BT_VERTICAL_LAUNCH::_salvo) == 24);
static_assert(sizeof(VLAUNCH_SAVELOAD) == 36);
static_assert(offsetof(VLAUNCH_SAVELOAD, salvo) + sizeof(S32) == 36);
static_assert(sizeof(BT_CLOAK_LAUNCHER) == 84);
static_assert(sizeof(CLOAK_LAUNCHER_SAVELOAD) == 8);
static_assert(offsetof(CLOAK_LAUNCHER_SAVELOAD, cloakSupplyCount) == 4);

// Original all-race catalogue owners retain their Win32 data/save extents.
// Actual factory admission is separate from this storage contract.
static_assert(sizeof(BT_FLAGSHIP_DATA) == 1384);
static_assert(sizeof(BASE_FLAGSHIP_SAVELOAD) == 104);
static_assert(sizeof(FLAGSHIP_SAVELOAD) == 616);
static_assert(sizeof(BASE_TROOPSHIP_SAVELOAD) == 12);
static_assert(sizeof(TROOPSHIP_SAVELOAD) == 436);
static_assert(sizeof(TROOPPOD_SAVELOAD) == 4);
static_assert(offsetof(TROOPPOD_SAVELOAD, dwTargetID) == 0);
static_assert(sizeof(BT_RECONPROBE_DATA) == 680);
static_assert(sizeof(BASE_RECONPROBE_SAVELOAD) == 28);
static_assert(sizeof(RECONPROBE_SAVELOAD) == 452);
static_assert(sizeof(BT_WORMHOLE_LAUNCHER) == 80);
static_assert(sizeof(WORMHOLE_LAUNCHER_SAVELOAD) == 112);
static_assert(offsetof(WORMHOLE_LAUNCHER_SAVELOAD, mode) == 108);
static_assert(sizeof(BT_MULTICLOAK_LAUNCHER) == 88);
static_assert(sizeof(MULTICLOAK_LAUNCHER_SAVELOAD) == 16);
static_assert(sizeof(BT_SYNTHESIS_DATA) == 64);
static_assert(sizeof(BASE_SYNTHESIS_SAVELOAD) == 16);
static_assert(sizeof(SYNTHESIS_SAVELOAD) == 56);


// The source AI saves assignment records, never native linked-list pointers.
static_assert(sizeof(PLAYERAI_ASSIGNMENT_RECORD) == 40 + 4 * MAX_SELECTED_UNITS);
static_assert(offsetof(PLAYERAI_ASSIGNMENT_RECORD, type) == 4);
static_assert(offsetof(PLAYERAI_ASSIGNMENT_RECORD, set) == 16);
static_assert(offsetof(PLAYERAI_ASSIGNMENT_RECORD, supships) + sizeof(U32) ==
    sizeof(PLAYERAI_ASSIGNMENT_RECORD));
// PLAYERAI_SAVELOAD uses legacy value-type inheritance; measure its storage
// extent explicitly rather than assume standard-layout. Native Windows remains
// required to independently confirm this source-derived fixed-width record.
static_assert(sizeof(PLAYERAI_SAVELOAD) == 47608);
static_assert(alignof(PLAYERAI_SAVELOAD) == 8);
static_assert(sizeof(BuildPriority) == 32);
static_assert(sizeof(ResearchPriority) == 24);
static_assert(sizeof(ShipPriority) == 16);
static_assert(sizeof(EnemyStatus) == 16);
static_assert(sizeof(ScoutRoute) == 28);

// Actual TM1 Seeker/Khamir launchers and the original kamikaze save caller.
static_assert(sizeof(SINGLE_TECHNODE) == 20);
static_assert(sizeof(BT_PING_LAUNCHER) == 76);
static_assert(sizeof(BT_ZEALOT_DATA) == 80);
static_assert(sizeof(BT_ZEALOT_DATA::impactBlastType) == 32);
static_assert(sizeof(BASE_ZEALOT_SAVELOAD) == 20);
static_assert(offsetof(BASE_ZEALOT_SAVELOAD, dwMissionID) == 0);
static_assert(offsetof(BASE_ZEALOT_SAVELOAD, targetID) == 4);
static_assert(offsetof(BASE_ZEALOT_SAVELOAD, zealotArchetypeID) == 8);
static_assert(offsetof(BASE_ZEALOT_SAVELOAD, systemID) == 12);
static_assert(offsetof(BASE_ZEALOT_SAVELOAD, stage) + sizeof(ZSTAGE) == 20);
static_assert(sizeof(TRANS_SAVELOAD) == 40);
static_assert(sizeof(ZEALOT_SAVELOAD) == 64 && alignof(ZEALOT_SAVELOAD) == 4);

static_assert(sizeof(void *) == 8, "The Linux port requires a 64-bit process");
static_assert(sizeof(long) == 8, "The Linux x86_64 probe requires the LP64 data model");
static_assert(sizeof(U8) == 1, "U8 must remain an 8-bit storage type");
static_assert(sizeof(GT_BUTTON) == 84,
    "Original button archetypes retain their Win32 schema extent and text/image offsets");
static_assert(sizeof(S8) == 1, "S8 must remain an 8-bit storage type");
static_assert(sizeof(U16) == 2, "U16 must remain a 16-bit storage type");
static_assert(sizeof(S16) == 2, "S16 must remain a 16-bit storage type");
static_assert(sizeof(U32) == 4, "U32 must remain a 32-bit storage type");
static_assert(sizeof(S32) == 4, "S32 must remain a 32-bit storage type");
static_assert(sizeof(BOOL32) == 4, "BOOL32 must remain a 32-bit storage type");
static_assert(sizeof(U64) == 8, "U64 must remain a 64-bit storage type");
static_assert(sizeof(S64) == 8, "S64 must remain a 64-bit storage type");
static_assert(sizeof(MISSION_SAVELOAD) == 104,
    "Mission save records retain their original extent");
static_assert(offsetof(MISSION_SAVELOAD, caps) == 70,
    "Mission capabilities retain their original byte-aligned offset");
static_assert(offsetof(MISSION_SAVELOAD, groupID) == 76 &&
    offsetof(MISSION_SAVELOAD, pendingOp) + sizeof(U32) == 104,
    "Mission fields after capabilities retain their original offsets");
static_assert(sizeof(MPartRef) == 16,
	"MPartRef in script mission data must retain its Win32 save stride");
static_assert(alignof(MPartRef) == 4,
	"MPartRef in script mission data must retain four-byte alignment");
static_assert(sizeof(MissionData) == 292,
	"Sol_T MissionData must retain its source-derived Win32 save size");
static_assert(sizeof(SolTrain_SelectAndMoveForger_Save) == 4,
	"Sol_T movement checkpoint must retain its four-byte saved enum");
static_assert(std::is_unsigned<U32>::value, "U32 must remain unsigned");
static_assert(std::is_signed<S32>::value, "S32 must remain signed");
static_assert(sizeof(HYBRID) == sizeof(void *), "HYBRID must retain native pointer width");
static_assert(alignof(HYBRID) == alignof(void *), "HYBRID must retain native pointer alignment");
static_assert(sizeof(M_RACE) == 4, "M_RACE must retain its retail representation");
static_assert(sizeof(M_OBJCLASS) == 4, "M_OBJCLASS must retain its retail representation");
static_assert(sizeof(UNIT_STANCE) == 4, "UNIT_STANCE must retain its retail ABI width");
static_assert(M_FRIGATE == 73, "M_OBJCLASS must retain retail numeric IDs");
static_assert(M_ENDOBJCLASS == 110, "M_OBJCLASS must retain the retail sentinel");
static_assert(std::is_standard_layout<MISSION_DATA_BIN>::value,
	"MISSION_DATA_BIN must remain a standard-layout storage type");
static_assert(alignof(ResourceCost) == 4, "ResourceCost must retain 4-byte alignment");
static_assert(alignof(MISSION_DATA_BIN::M_CAPS) == 1, "M_CAPS must retain byte alignment");
static_assert(offsetof(MISSION_DATA_BIN, caps) == 4, "MISSION_DATA_BIN caps offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, hullPointsMax) == 8,
	"MISSION_DATA_BIN hullPointsMax offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, supplyPointsMax) == 10,
	"MISSION_DATA_BIN supplyPointsMax offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, scrapValue) == 12,
	"MISSION_DATA_BIN scrapValue offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, buildTime) == 14,
	"MISSION_DATA_BIN buildTime offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, resourceCost) == 16,
	"MISSION_DATA_BIN resourceCost offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, sensorRadius) == 20,
	"MISSION_DATA_BIN sensorRadius offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, cloakedSensorRadius) == 24,
	"MISSION_DATA_BIN cloakedSensorRadius offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, maxVelocity) == 28,
	"MISSION_DATA_BIN maxVelocity offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, baseWeaponAccuracy) == 32,
	"MISSION_DATA_BIN baseWeaponAccuracy offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, baseShieldLevel) == 36,
	"MISSION_DATA_BIN baseShieldLevel offset mismatch");
static_assert(offsetof(MISSION_DATA_BIN, baseShieldLevel) +
		sizeof(MISSION_DATA_BIN::baseShieldLevel) == sizeof(MISSION_DATA_BIN),
	"MISSION_DATA_BIN final-field extent mismatch");

namespace
{
bool checkMissionHeaderBits()
{
	MISSION_DATA_BIN missionData = {};
	missionData.mObjClass = 0xA5;
	missionData.race = 0xB;
	missionData.displayName = 0x2AAAA;

	std::uint32_t encoded = 0;
	std::memcpy(&encoded, &missionData, sizeof(encoded));
	const std::uint32_t expected = 0xA5u | (0xBu << 8) | (0x2AAAAu << 12);
	return encoded == expected;
}

bool checkScriptReferenceBytes()
{
	MPartRef reference;
	unsigned char bytes[sizeof(reference)] = {};
	std::memcpy(bytes, &reference, sizeof(bytes));
	for (unsigned char byte : bytes)
		if (byte != 0) return false;
	return true;
}

bool checkEmptyScriptSaveStorage()
{
    struct EmptyProgram : CQBaseProgram, CQ_SCRIPT_SAVE_BASE(SolTrain_STowerFinished_Save)
    {
        void Initialize(U32, const MPartRef &) override {}
        bool Update() override { return true; }
        void Destroy() override {}
    };
    EmptyProgram instance;
    CQBaseProgram *program = &instance;
    auto *save = static_cast<SolTrain_STowerFinished_Save *>(&instance);
    const auto offset = reinterpret_cast<const char *>(save) -
        reinterpret_cast<const char *>(program);
    if (offset < static_cast<std::ptrdiff_t>(sizeof(CQBaseProgram)) ||
        offset != static_cast<std::ptrdiff_t>(
            scriptoffsetofclass(SolTrain_STowerFinished_Save, EmptyProgram)))
        return false;
    static_assert(sizeof(SolTrain_STowerFinished_Save) == 1,
        "Empty original script save records retain their one-byte extent");
    const U8 savedByte = 0xa5;
    std::memcpy(save, &savedByte, sizeof(savedByte));
    return program->Update();
}

bool checkSolTrainMissionOffsets()
{
	MissionData data = {};
	const auto *start = reinterpret_cast<const unsigned char *>(&data);
	return reinterpret_cast<const unsigned char *>(&data.Forger) - start == 168 &&
		reinterpret_cast<const unsigned char *>(&data.Taos1) - start == 184 &&
		reinterpret_cast<const unsigned char *>(&data.Taos2) - start == 200 &&
		reinterpret_cast<const unsigned char *>(&data.Acropolis) - start == 216 &&
		reinterpret_cast<const unsigned char *>(
			&data.displayObjectiveHandlerParams_teletypeHandle) - start == 288;
}

bool checkResourceCostBits()
{
	ResourceCost resourceCost = {};
	resourceCost.gas = 0x11;
	resourceCost.metal = 0x22;
	resourceCost.crew = 0x33;
	resourceCost.commandPt = 0x44;

	std::uint32_t encoded = 0;
	std::memcpy(&encoded, &resourceCost, sizeof(encoded));
	return encoded == 0x44332211u;
}

bool checkCapabilityBits()
{
	MISSION_DATA_BIN::M_CAPS capabilities = {};
	capabilities.padding = 0xA5;
	capabilities.moveOk = true;
	capabilities.supplyOk = true;
	capabilities.harvestOk = true;
	capabilities.probeOk = true;
	capabilities.mimicOk = true;
	capabilities.specialTargetPlanetOk = true;

	const unsigned char expected[] = {0xA5, 0x81, 0x81, 0x81};
	unsigned char encoded[sizeof(capabilities)] = {};
	std::memcpy(encoded, &capabilities, sizeof(encoded));
	return std::memcmp(encoded, expected, sizeof(expected)) == 0;
}
}

// Raw744 flagship records are not CQ2 BT_FLAGSHIP_DATA. Validate only the
// bounded interoperability decoder; this does not create a working admiral.
static bool checkRetailFlagshipDecoder()
{
    using FrontierX::NativeRetailFlagshipData;
    std::array<unsigned char, 745> bytes{};
    const auto put = [&](std::size_t offset, const auto &value) {
        std::memcpy(bytes.data() + offset, &value, sizeof(value));
    };
    put(644, 1.5F);
    put(648, U32{6});
    for (std::size_t offset = 652; offset != 700; offset += 4) put(offset, 0.375F);
    put(716, U32{0x12345678U});
    bytes[720] = bytes[723] = bytes[725] = 1;
    put(740, U32{0x23456789U});
    NativeRetailFlagshipData decoded{};
    if (!FrontierX::DecodeNativeRetailFlagshipData(bytes.data(), 744, decoded) ||
        decoded.attackRadius != 1.5F || decoded.admiralHotkey != 6 ||
        decoded.general.damage != 0.375F || decoded.favored.sensors != 0.375F ||
        decoded.favoredShips[4] != 0x12345678U || !decoded.antiSolarian ||
        decoded.antiTerran || !decoded.superFighters || !decoded.evasiveManuvers ||
        decoded.toolbarHint != 0x23456789U) return false;
    const auto rejected = [&](const void *data, std::size_t size) {
        const NativeRetailFlagshipData before = decoded;
        return !FrontierX::DecodeNativeRetailFlagshipData(data, size, decoded) &&
            std::memcmp(&before, &decoded, sizeof(decoded)) == 0;
    };
    if (!rejected(nullptr, 744) || !rejected(bytes.data(), 743) ||
        !rejected(bytes.data(), 745)) return false;
    put(648, U32{7});
    if (!rejected(bytes.data(), 744)) return false;
    put(648, U32{6});
    for (std::size_t offset = 720; offset != 726; ++offset)
    {
        const auto previous = bytes[offset];
        bytes[offset] = 2;
        if (!rejected(bytes.data(), 744)) return false;
        bytes[offset] = previous;
    }
    for (std::size_t index = 0; index != 13; ++index)
    {
        const std::size_t offset = index ? 648 + index * 4 : 644;
        const float previous = index ? 0.375F : 1.5F;
        put(offset, std::numeric_limits<float>::quiet_NaN());
        if (!rejected(bytes.data(), 744)) return false;
        put(offset, std::numeric_limits<float>::infinity());
        if (!rejected(bytes.data(), 744)) return false;
        put(offset, previous);
    }
    return true;
}

int main()
{
    if (!checkRetailFlagshipDecoder())
    {
        std::cerr << "Retail flagship decoder accepted invalid data or changed failed output\n";
        return 1;
    }
    // These original value-type records use inheritance and are not standard
    // layout. Measure real subobject addresses rather than apply offsetof to a
    // conditionally supported type or suppress its compiler diagnostic.
    BT_PING_LAUNCHER ping{};
    BT_ZEALOT_DATA zealot{};
    ZEALOT_SAVELOAD zealotSave{};
    const auto offset = [](const void *record, const void *field) {
        return static_cast<const unsigned char *>(field) -
               static_cast<const unsigned char *>(record);
    };
    if (offset(&ping, &ping.techNode) != 56 ||
        offset(&ping, &ping.techNode) + sizeof(ping.techNode) != sizeof(ping) ||
        offset(&zealot, &zealot.impactBlastType) != 28 ||
        offset(&zealot, &zealot.neededTech) != 60 ||
        offset(&zealot, &zealot.neededTech) + sizeof(zealot.neededTech) != sizeof(zealot) ||
        offset(&zealotSave, &zealotSave.trans_SL) != 20 ||
        offset(&zealotSave, &zealotSave.trans_SL) + sizeof(TRANS_SAVELOAD) + sizeof(U32) != sizeof(zealotSave))
    {
        std::cerr << "Original Ping/Zealot archetype/save subobject layout mismatch\n";
        return 1;
    }
    GT_BUTTON button{};
    const auto *buttonBytes = reinterpret_cast<const unsigned char *>(&button);
    if (reinterpret_cast<const unsigned char *>(button.fontName) - buttonBytes != 8 ||
        reinterpret_cast<const unsigned char *>(button.shapeFile) - buttonBytes != 49)
    {
        std::cerr << "Original button schema offsets differ\n";
        return 1;
    }
	if (!checkScriptReferenceBytes())
	{
		std::cerr << "MPartRef retained pointer bytes in a new script reference\n";
		return 4;
	}
    if (!checkEmptyScriptSaveStorage())
    {
        std::cerr << "Empty script save overlaps polymorphic program storage\n";
        return 1;
    }
	if (!checkSolTrainMissionOffsets())
	{
		std::cerr << "Sol_T mission reference offsets changed\n";
		return 5;
	}
	if (!checkMissionHeaderBits())
	{
		std::cerr << "MISSION_DATA_BIN header bitfield layout mismatch\n";
		return 1;
	}
	if (!checkResourceCostBits())
	{
		std::cerr << "ResourceCost bitfield layout mismatch\n";
		return 2;
	}
	if (!checkCapabilityBits())
	{
		std::cerr << "MISSION_DATA_BIN capability bitfield layout mismatch\n";
		return 3;
	}
	return 0;
}
