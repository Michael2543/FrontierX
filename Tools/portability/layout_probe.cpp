#include <DMissionDataLayout.h>
#include <DMissionEnum.h>
#include <MPartRef.h>

#include "Include/NativeAppGlobals.h"
#include "CQTrace.h"
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
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>

static_assert(sizeof(void *) == 8, "The Linux port requires a 64-bit process");
static_assert(sizeof(long) == 8, "The Linux x86_64 probe requires the LP64 data model");
static_assert(sizeof(U8) == 1, "U8 must remain an 8-bit storage type");
static_assert(sizeof(S8) == 1, "S8 must remain an 8-bit storage type");
static_assert(sizeof(U16) == 2, "U16 must remain a 16-bit storage type");
static_assert(sizeof(S16) == 2, "S16 must remain a 16-bit storage type");
static_assert(sizeof(U32) == 4, "U32 must remain a 32-bit storage type");
static_assert(sizeof(S32) == 4, "S32 must remain a 32-bit storage type");
static_assert(sizeof(BOOL32) == 4, "BOOL32 must remain a 32-bit storage type");
static_assert(sizeof(U64) == 8, "U64 must remain a 64-bit storage type");
static_assert(sizeof(S64) == 8, "S64 must remain a 64-bit storage type");
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
	if (!checkScriptReferenceBytes())
	{
		std::cerr << "MPartRef retained pointer bytes in a new script reference\n";
		return 4;
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
