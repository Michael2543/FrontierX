#include <DMissionDataLayout.h>
#include <DMissionEnum.h>
#include <MPartRef.h>

#include "Include/NativeAppGlobals.h"
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
#endif
#include <DataDef.h>
#include <DMBaseData.h>
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

int main()
{
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
