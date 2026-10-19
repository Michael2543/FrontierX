// Explicit asset/factory diagnostic. Never run inside a playable simulation.
#include "Include/NativeAppGlobals.h"
#include "Include/NativeRetailFlagshipData.h"
#include "ObjList.h"
#include "Include/CQTrace.h"
#include <DSpaceship.h>
#include <DPlatform.h>
#include <DFlagship.h>
#include <DFabricator.h>
#include <DSupplyShip.h>
#include <DBuildPlat.h>
#include <DBuildSupPlat.h>
#include <DGunPlat.h>
#include <DJumpPlat.h>
#include <DRefinePlat.h>
#include <DRepairPlat.h>
#include <DSellPlat.h>
#include <cstring>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

namespace
{
// EnumerateArchetypeData exposes raw archive bytes. It performs no schema
// correlation; a base-record check cannot protect a factory's derived reads.
// Keep unsupported extents as failures, before any factory is invoked.
std::size_t UnitDataSize(bool ship, U32 subtype)
{
    if (ship)
    {
        switch (subtype)
        {
        case SSC_GUNBOAT: return sizeof(BT_GUNBOAT_DATA);
        case SSC_HARVESTSHIP: return sizeof(BT_HARVESTSHIP_DATA);
        case SSC_FABRICATOR: return sizeof(BT_FABRICATOR_DATA);
        case SSC_MINELAYER: return sizeof(BT_MINELAYER_DATA);
        case SSC_RECONPROBE: return sizeof(BT_RECONPROBE_DATA);
        case SSC_FLAGSHIP: return sizeof(BT_FLAGSHIP_DATA);
        case SSC_SUPPLYSHIP: return sizeof(BT_SUPPLYSHIP_DATA);
        case SSC_TROOPSHIP: return sizeof(BT_TROOPSHIP_DATA);
        default: return 0;
        }
    }
    switch (subtype)
    {
    case PC_BUILDPLAT: return sizeof(BT_PLAT_BUILD_DATA);
    case PC_BUILDSUPPLAT: return sizeof(BT_PLAT_BUILDSUP_DATA);
    case PC_GUNPLAT: return sizeof(BT_PLAT_GUN);
    case PC_JUMPPLAT: return sizeof(BT_PLAT_JUMP_DATA);
    case PC_REFINERY: return sizeof(BT_PLAT_REFINE_DATA);
    case PC_REPAIRPLAT: return sizeof(BT_PLAT_REPAIR_DATA);
    case PC_SELL: return sizeof(BT_PLAT_SELL_DATA);
    default: return 0;
    }
}

struct UnitRecord
{
    std::string name;
    U32 race;
    bool ship;
    bool validExtent;
};

struct UnitEnumerator final : IArchetypeEnum
{
    std::vector<UnitRecord> records;
    bool valid = true;
    BOOL32 __stdcall ArchetypeEnum(const char *name, void *data, U32 size) override
    {
        if (!name || !data || size < sizeof(BASIC_DATA))
        {
            std::cerr << "Native unit catalog invalid metadata: " << (name ? name : "<null>")
                      << "; bytes " << size << '\n';
            return false;
        }
        // The archive also contains non-archetype leaves. Inspect its discriminator
        // as bytes before forming any typed enum view.
        U32 objectClass;
        std::memcpy(&objectClass, data, sizeof(objectClass));
        U32 race;
        U32 subtype;
        const bool ship = objectClass == OC_SPACESHIP;
        if (ship)
        {
            // Construction shuttles share OC_SPACESHIP but have their own
            // compact record and no missionData.race; their factories are
            // exercised as dependencies of the selected construction owners.
            if (size >= sizeof(BASIC_DATA) + sizeof(U32))
            {
                std::memcpy(&subtype, static_cast<const char *>(data) + sizeof(BASIC_DATA), sizeof(subtype));
                if ((subtype == SSC_BUILDSHIP || subtype == SSC_TERRANDRONE || subtype == SSC_SPIDERDRONE))
                {
                    if (size == sizeof(BT_BUILDERSHIP_DATA)) return true;
                    std::cerr << "Native unit catalog invalid construction shuttle: " << name << '\n';
                    return false;
                }
            }
            if (size < sizeof(BASE_SPACESHIP_DATA))
            {
                std::cerr << "Native unit catalog truncated ship: " << name << "; bytes " << size << '\n';
                return false;
            }
            race = static_cast<const BASE_SPACESHIP_DATA *>(data)->missionData.race;
        }
        else if (objectClass == OC_PLATFORM)
        {
            if (size < sizeof(BASE_PLATFORM_DATA))
            {
                std::cerr << "Native unit catalog truncated platform: " << name << "; bytes " << size << '\n';
                return false;
            }
            race = static_cast<const BASE_PLATFORM_DATA *>(data)->missionData.race;
            std::memcpy(&subtype, static_cast<const char *>(data) + sizeof(BASIC_DATA), sizeof(subtype));
        }
        else return true;
        if (race == M_TERRAN || race == M_MANTIS || race == M_SOLARIAN)
        {
            const std::size_t expected = UnitDataSize(ship, subtype);
            bool validExtent = expected && size == expected;
            if (ship && subtype == SSC_FLAGSHIP &&
                size == sizeof(FrontierX::NativeRetailFlagshipData))
            {
                FrontierX::NativeRetailFlagshipData decoded{};
                validExtent = FrontierX::DecodeNativeRetailFlagshipData(data, size, decoded);
            }
            records.push_back({name, race, ship, validExtent});
            if (!validExtent)
            {
                std::cerr << "Native unit catalog invalid raw extent: " << name
                          << "; subtype " << subtype << "; bytes " << size
                          << "; expected " << expected << '\n';
                valid = false;
            }
        }
        return true;
    }
};
}

// Protect the observed UBSan regression: ARCHLIST also enumerates non-archetype
// leaves whose first word is not an OBJCLASS value. This metadata-only check runs
// in the existing shell lifecycle test, never as a replacement mission/factory.
bool CheckNativeUnitCatalogMetadata()
{
    UnitEnumerator catalog;
    U32 nonArchetype[2] = {0x72746574U, 0};
    if (!catalog.ArchetypeEnum("non-archetype", nonArchetype, sizeof(nonArchetype)) ||
        !catalog.records.empty()) return false;
    for (SPACESHIPCLASS type : {SSC_BUILDSHIP, SSC_TERRANDRONE, SSC_SPIDERDRONE})
    {
        BT_BUILDERSHIP_DATA shuttle = {};
        shuttle.objClass = OC_SPACESHIP;
        shuttle.type = type;
        if (!catalog.ArchetypeEnum("construction-shuttle", &shuttle, sizeof(shuttle)) ||
            !catalog.records.empty()) return false;
    }
    BT_GUNBOAT_DATA ship = {};
    ship.objClass = OC_SPACESHIP;
    ship.type = SSC_GUNBOAT;
    ship.missionData.race = M_TERRAN;
    // Recognized object classes can likewise carry an invalid raw subtype.
    // Reject it as metadata without reading an out-of-range enum object.
    UnitEnumerator badSubtypes;
    const U32 invalidSubtype = 0x72746574U;
    std::memcpy(reinterpret_cast<char *>(&ship) + sizeof(BASIC_DATA), &invalidSubtype, sizeof(invalidSubtype));
    BT_PLAT_GUN platform = {};
    platform.objClass = OC_PLATFORM;
    platform.missionData.race = M_SOLARIAN;
    std::memcpy(reinterpret_cast<char *>(&platform) + sizeof(BASIC_DATA), &invalidSubtype, sizeof(invalidSubtype));
    if (!badSubtypes.ArchetypeEnum("invalid-ship-subtype", &ship, sizeof(ship)) ||
        !badSubtypes.ArchetypeEnum("invalid-platform-subtype", &platform, sizeof(platform)) ||
        badSubtypes.valid || badSubtypes.records.size() != 2 ||
        badSubtypes.records[0].validExtent || badSubtypes.records[1].validExtent) return false;
    ship.type = SSC_GUNBOAT;
    return catalog.ArchetypeEnum("race-tagged-ship", &ship, sizeof(ship)) &&
        catalog.valid && catalog.records.size() == 1 && catalog.records[0].validExtent;
}

bool CheckNativeUnitCatalog(IArchetypeList *objectList)
{
    UnitEnumerator catalog;
    if (!objectList || !objectList->EnumerateArchetypeData(&catalog) || catalog.records.empty())
    {
        std::cerr << "Native unit catalog: invalid or empty source unit metadata\n";
        return false;
    }
    U32 selected[3][2] = {}, loaded[3][2] = {}, invalid[3][2] = {};
    // Validate the complete selected set before recursive LoadArchetype calls.
    // Invalid records remain selected failures; valid records still exercise
    // their original factories. Recursive dependencies require their own owner
    // validation; this unit preflight does not substitute for that boundary.
    for (const auto &unit : catalog.records)
    {
        const U32 race = unit.race - M_TERRAN;
        const U32 kind = unit.ship ? 0 : 1;
        ++selected[race][kind];
        if (!unit.validExtent)
        {
            ++invalid[race][kind];
            continue;
        }
        std::cout << "Native unit catalog checking: " << unit.name << std::endl;
        // LoadArchetype caches through the original owner, including dependent
        // launchers/effects. Global teardown releases that cache on diagnostic exit.
        // No ship, operation, objective or progress value is created here.
        try
        {
            if (objectList->LoadArchetype(unit.name.c_str())) ++loaded[race][kind];
            else std::cerr << "Native unit catalog failed: " << unit.name << '\n';
        }
        catch (const std::runtime_error &error)
        {
            std::cerr << "Native unit catalog failed: " << unit.name << "; " << error.what() << '\n';
        }
    }
    bool complete = catalog.valid;
    for (U32 race = 0; race != 3; ++race)
    {
        std::cout << "Native unit catalog race " << race + M_TERRAN
                  << ": ships " << loaded[race][0] << '/' << selected[race][0]
                  << " (invalid raw " << invalid[race][0] << ')'
                  << "; platforms " << loaded[race][1] << '/' << selected[race][1]
                  << " (invalid raw " << invalid[race][1] << ")\n";
        complete = complete && selected[race][0] && selected[race][1] &&
            loaded[race][0] == selected[race][0] && loaded[race][1] == selected[race][1];
    }
    std::cout << "Native unit catalog: factory/assets only; gameplay untested\n";
    return complete;
}
