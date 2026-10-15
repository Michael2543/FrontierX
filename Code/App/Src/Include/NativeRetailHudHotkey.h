#ifndef FRONTIERX_NATIVE_RETAIL_HUD_HOTKEY_H
#define FRONTIERX_NATIVE_RETAIL_HUD_HOTKEY_H

#if !defined(_WIN32)
#include "Hotkeys.h"
#include <EventSys.h>
#include <HKEvent.h>
#include <cstdint>

namespace FrontierX
{
// Shipped UI records retain their retail HOTKEYS::HOTKEY enum values.
// Convert once at resource initialization, before mutable tooltip/state changes.
// Physical keyboard events already carry current IDs and must never enter here.
inline U32 NativeRetailHudHotkey(U32 resourceHotkey)
{
    switch (resourceHotkey)
    {
    case 1: return IDH_ADMIRAL_TACTIC_DEFEND;
    case 2: return IDH_ADMIRAL_TACTIC_HOLD;
    case 3: return IDH_ADMIRAL_TACTIC_PEACE;
    case 4: return IDH_ADMIRAL_TACTIC_SEEK;
    case 5: return IDH_BUILD_PLATS;
    case 6: return IDH_CANCEL_COMMAND;
    case 7: return IDH_CENTER_SCREEN;
    case 8: return IDH_CHATALL;
    case 9: return IDH_CHATTEAM;
    case 10: return IDH_CHAT_MENU;
    case 11: return IDH_CLOAK;
    case 12: return IDH_CONTEXT_A;
    case 13: return IDH_CONTEXT_B;
    case 14: return IDH_CONTEXT_C;
    case 15: return IDH_CONTEXT_D;
    case 16: return IDH_CONTEXT_E;
    case 17: return IDH_CONTEXT_F;
    case 18: return IDH_CONTEXT_G;
    case 19: return IDH_CONTEXT_H;
    case 20: return IDH_CONTEXT_I;
    case 21: return IDH_CONTEXT_J;
    case 22: return IDH_CONTEXT_K;
    case 23: return IDH_CONTEXT_L;
    case 24: return IDH_CONTEXT_M;
    case 25: return IDH_CONTEXT_N;
    case 26: return IDH_CONTEXT_O;
    case 27: return IDH_CONTEXT_P;
    case 28: return IDH_CONTEXT_Q;
    case 29: return IDH_CONTEXT_R;
    case 30: return IDH_CONTEXT_S;
    case 31: return IDH_CONTEXT_T;
    case 32: return IDH_CONTEXT_U;
    case 33: return IDH_CONTEXT_V;
    case 34: return IDH_CONTEXT_W;
    case 35: return IDH_CONTEXT_X;
    case 36: return IDH_CONTEXT_Y;
    case 37: return IDH_CONTEXT_Z;
    case 38: return IDH_CYCLE_ADMIRALS;
    case 39: return IDH_CYCLE_FLEETS;
    case 40: return IDH_DEBUG_PRINT;
    case 41: return IDH_DEFEND;
    case 42: return IDH_DESTROY;
    case 43: return IDH_DIPLOMACY;
    case 44: return IDH_DISBAND_FLEET;
    case 45: return IDH_FABRICATOR;
    case 46: return IDH_FIGHTER_NORMAL;
    case 47: return IDH_FIGHTER_PATROL;
    case 48: return IDH_FLEET_GROUP_1;
    case 49: return IDH_FLEET_GROUP_2;
    case 50: return IDH_FLEET_GROUP_3;
    case 51: return IDH_FLEET_GROUP_4;
    case 52: return IDH_FLEET_GROUP_5;
    case 53: return IDH_FLEET_GROUP_6;
    case 54: return IDH_FORM_FLEET;
    case 55: return IDH_FULLSCREEN_MAP;
    case 56: return IDH_GAME_OPTIONS;
    case 57: return IDH_GROUP_0_CREATE;
    case 58: return IDH_GROUP_0_SELECT;
    case 59: return IDH_GROUP_1_CREATE;
    case 60: return IDH_GROUP_1_SELECT;
    case 61: return IDH_GROUP_2_CREATE;
    case 62: return IDH_GROUP_2_SELECT;
    case 63: return IDH_GROUP_3_CREATE;
    case 64: return IDH_GROUP_3_SELECT;
    case 65: return IDH_GROUP_4_CREATE;
    case 66: return IDH_GROUP_4_SELECT;
    case 67: return IDH_GROUP_5_CREATE;
    case 68: return IDH_GROUP_5_SELECT;
    case 69: return IDH_GROUP_6_CREATE;
    case 70: return IDH_GROUP_6_SELECT;
    case 71: return IDH_GROUP_7_CREATE;
    case 72: return IDH_GROUP_7_SELECT;
    case 73: return IDH_GROUP_8_CREATE;
    case 74: return IDH_GROUP_8_SELECT;
    case 75: return IDH_GROUP_9_CREATE;
    case 76: return IDH_GROUP_9_SELECT;
    case 77: return IDH_LOAD_MENU;
    case 78: return IDH_MISSION_OBJ;
    case 79: return IDH_MULTIPLAYER_SCORE;
    case 80: return IDH_NEXT_UNIT;
    case 81: return IDH_PATROL;
    case 82: return IDH_PAUSE_GAME;
    case 83: return IDH_PREV_UNIT;
    case 84: return IDH_QUICK_LOAD;
    case 85: return IDH_QUICK_SAVE;
    case 86: return IDH_RALLY_POINT;
    case 87: return IDH_RECORD_SPEECH;
    case 88: return IDH_REPAIR;
    case 89: return IDH_REPAIR_FLEET;
    case 90: return IDH_REPORT_STATS;
    case 91: return IDH_RESEARCH_PLAT;
    case 92: return IDH_RESUPPLY_FLEET;
    case 93: return IDH_ROTATE_0_WORLD;
    case 94: return IDH_ROTATE_90_WORLD_LEFT;
    case 95: return IDH_ROTATE_90_WORLD_RIGHT;
    case 96: return IDH_ROTATE_WORLD_LEFT;
    case 97: return IDH_ROTATE_WORLD_RIGHT;
    case 98: return IDH_SAVE_MENU;
    case 99: return IDH_SCREEN_SHOT;
    case 100: return IDH_SCROLL_DOWN;
    case 101: return IDH_SCROLL_DOWNLEFT;
    case 102: return IDH_SCROLL_DOWNRIGHT;
    case 103: return IDH_SCROLL_LEFT;
    case 104: return IDH_SCROLL_RIGHT;
    case 105: return IDH_SCROLL_UP;
    case 106: return IDH_SCROLL_UPLEFT;
    case 107: return IDH_SCROLL_UPRIGHT;
    case 108: return IDH_SEEK_EVENT;
    case 109: return IDH_SELECT_ALL_FLEET;
    case 110: return IDH_SELECT_IDLE_CIVILIAN;
    case 111: return IDH_SELECT_IDLE_MILITARY;
    case 112: return IDH_SELL;
    case 113: return IDH_SPECIAL_ABILITY;
    case 114: return IDH_SPECIAL_AEGIS;
    case 115: return IDH_SPECIAL_ASSAULT;
    case 116: return IDH_SPECIAL_AUGER;
    case 117: return IDH_SPECIAL_BOMBER;
    case 118: return IDH_SPECIAL_CLOAK;
    case 119: return IDH_SPECIAL_DESTABILIZER;
    case 120: return IDH_SPECIAL_FURYRAM;
    case 121: return IDH_SPECIAL_MASSDISRUPT;
    case 122: return IDH_SPECIAL_MIMIC;
    case 123: return IDH_SPECIAL_MINELAYER;
    case 124: return IDH_SPECIAL_PROBE;
    case 125: return IDH_SPECIAL_PROTEUSMINE;
    case 126: return IDH_SPECIAL_REPEL;
    case 127: return IDH_SPECIAL_REPULSOR;
    case 128: return IDH_SPECIAL_SHROUD;
    case 129: return IDH_SPECIAL_STASIS;
    case 130: return IDH_SPECIAL_SYNTHESIS;
    case 131: return IDH_SPECIAL_TEMPEST;
    case 132: return IDH_SPECIAL_VAMPIRE;
    case 133: return IDH_SPECIAL_WORMHOLE;
    case 134: return IDH_STANCE_ATTACK;
    case 135: return IDH_STANCE_DEFENSIVE;
    case 136: return IDH_STANCE_IDLE;
    case 137: return IDH_STANCE_STANDGROUND;
    case 138: return IDH_SUPPLY_STANCE_FULLYAUTO;
    case 139: return IDH_SUPPLY_STANCE_NOAUTO;
    case 140: return IDH_SUPPLY_STANCE_RESUPPLY;
    case 141: return IDH_TEST_SPEECH;
    case 142: return IDH_TOGGLE_SCREENMODE;
    case 143: return IDH_TOGGLE_TOOLBAR;
    case 144: return IDH_TOGGLE_ZOOM;
    case 145: return IDH_TRANSFER_FLAGSHIP;
    case 146: return IDH_WIN_CLOSE;
    case 147: return IDH_ZOOM_IN;
    case 148: return IDH_ZOOM_OUT;
    default: return resourceHotkey; // Keep zero, NONE and unknown values unchanged.
    }
}

inline GENRESULT PostNativeHudHotkey(IEventSystem *events, U32 command)
{
    return events->Post(CQE_HOTKEY, reinterpret_cast<void *>(
        static_cast<std::uintptr_t>(command)));
}
}
#endif

#endif
