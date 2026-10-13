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
#include <DTalorianLauncher.h>
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
        constructor->CorrelateSymbol(ship, input.data(), ship, output.data());
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
            moved.tobjmove.bMoveActive && moved.tobjmove.bCompletionAllowed &&
            moved.tobjmove.bFinalMove && moved.tobjmove.bSyncNeeded;
    }
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
