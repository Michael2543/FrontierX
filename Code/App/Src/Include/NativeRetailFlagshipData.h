#ifndef FRONTIERX_NATIVE_RETAIL_FLAGSHIP_DATA_H
#define FRONTIERX_NATIVE_RETAIL_FLAGSHIP_DATA_H

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace FrontierX
{
// Independently recorded field extents from the external archive's embedded
// schema. This is distinct from the released CQ2 BT_FLAGSHIP_DATA contract.
#pragma pack(push, 4)
struct NativeRetailFlagshipData
{
    std::uint8_t shipData[644];
    float attackRadius;
    std::uint32_t admiralHotkey;
    struct Bonuses
    {
        float damage, shields, supplyUsage, rangeModifier, speed, sensors;
    } general, favored;
    std::uint32_t favoredShips[5];
    std::uint8_t antiSolarian, antiTerran, antiMantis;
    std::uint8_t superFighters, platformKiller, evasiveManuvers;
    std::uint8_t padding[2];
    std::uint32_t toolbarImage, toolbarText, toolbarStatus, toolbarHint;
};
#pragma pack(pop)

static_assert(sizeof(NativeRetailFlagshipData) == 744);
static_assert(offsetof(NativeRetailFlagshipData, attackRadius) == 644);
static_assert(offsetof(NativeRetailFlagshipData, admiralHotkey) == 648);
static_assert(offsetof(NativeRetailFlagshipData, general) == 652);
static_assert(offsetof(NativeRetailFlagshipData, favored) == 676);
static_assert(offsetof(NativeRetailFlagshipData, favoredShips) == 700);
static_assert(offsetof(NativeRetailFlagshipData, antiSolarian) == 720);
static_assert(offsetof(NativeRetailFlagshipData, toolbarImage) == 728);
static_assert(offsetof(NativeRetailFlagshipData, toolbarHint) + sizeof(std::uint32_t) == 744);

// Retail favored-ship bonuses replace general bonuses; they are not the
// additive favored bonuses of the released CQ2 AdmiralBonuses record.
inline const NativeRetailFlagshipData::Bonuses &NativeRetailFlagshipBonusesForShip(
    const NativeRetailFlagshipData &data, std::uint32_t shipClass) noexcept
{
    for (const auto favored : data.favoredShips)
        if (favored == shipClass)
            return data.favored;
    return data.general;
}

inline bool DecodeNativeRetailFlagshipData(const void *bytes, std::size_t size,
                                         NativeRetailFlagshipData &output)
{
    if (!bytes || size != sizeof(NativeRetailFlagshipData)) return false;
    NativeRetailFlagshipData value;
    std::memcpy(&value, bytes, sizeof(value));
    const auto finite = [](const NativeRetailFlagshipData::Bonuses &bonus) {
        return std::isfinite(bonus.damage) && std::isfinite(bonus.shields) &&
            std::isfinite(bonus.supplyUsage) && std::isfinite(bonus.rangeModifier) &&
            std::isfinite(bonus.speed) && std::isfinite(bonus.sensors);
    };
    if (!std::isfinite(value.attackRadius) || !finite(value.general) || !finite(value.favored) ||
        value.admiralHotkey > 6 || value.antiSolarian > 1 || value.antiTerran > 1 ||
        value.antiMantis > 1 || value.superFighters > 1 || value.platformKiller > 1 ||
        value.evasiveManuvers > 1) return false;
    output = value;
    return true;
}
}

#endif
