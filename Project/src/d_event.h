

#pragma once

#include "i_video.h"

//
// Event handling.
//

// Input event types.
typedef enum
{
    ev_none,
    ev_keydown,
    ev_keyup,
    ev_textinput
} evtype_t;

// Event structure.
typedef struct
{
    evtype_t    type;
    int         data1;          // keys/mouse buttons
    int         data2;          // mouse x move
    int         data3;          // mouse y move
} event_t;

typedef enum
{
    ga_nothing,
    ga_loadlevel,
    ga_newgame,
    ga_loadgame,
    ga_savegame,
    ga_completed,
    ga_victory,
    ga_worlddone,
    ga_autoloadgame,
    ga_autosavegame
} gameaction_t;

//
// Button/action code definitions.
//
enum
{
    // Press "Fire".
    BT_ATTACK       = 1,
    // Use button, to open doors, activate switches.
    BT_USE          = 2,

    // Flag: game events, not really buttons.
    BT_SPECIAL      = 128,
    BT_SPECIALMASK  = 3,

    // Flag, weapon change pending.
    // If true, the next 4 bits hold weapon num.
    BT_CHANGE       = 4,
    // The 4-bit weapon mask and shift, convenience.
    BT_WEAPONMASK   = (8 + 16 + 32 + 64),
    BT_WEAPONSHIFT  = 3,

    BT_NOBEST      =  2048,

    BT_JUMP         = 4096,

    // Pause the game.
    BTS_PAUSE       = 1,
    // Save the game at each console.
    BTS_SAVEGAME    = 2
};

extern gameaction_t gameaction;
