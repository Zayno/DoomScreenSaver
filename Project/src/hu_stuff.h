

#pragma once

#include "i_video.h"
#include "r_defs.h"

//
// Globally visible constants.
//
#define HU_FONTSTART                '!' // the first font character
#define HU_FONTEND                  '_' // the last font character

// Calculate # of characters in font.
#define HU_FONTSIZE                 (HU_FONTEND - HU_FONTSTART + 1)

#define HU_MSGX                     (3 * 2)
#define HU_MSGY                     (2 * 2)

#define HU_MSGTIMEOUT               (4 * TICRATE)

#define HUD_NUMBER_MIN              (negativehealth ? -99 : 0)
#define HUD_NUMBER_MAX              999

#define HUD_HEALTH_X                (MAXWIDESCREENDELTA + 2)
#define HUD_HEALTH_Y                (SCREENHEIGHT - 32)
#define HUD_HEALTH_MIN              20
#define HUD_HEALTH_WAIT             250
#define HUD_HEALTH_HIGHLIGHT_WAIT   250

#define HUD_ARMOR_X                 (MAXWIDESCREENDELTA + 71)
#define HUD_ARMOR_Y                 HUD_HEALTH_Y
#define HUD_ARMOR_HIGHLIGHT_WAIT    250

#define HUD_KEYS_X                  (SCREENWIDTH - MAXWIDESCREENDELTA - 35)
#define HUD_KEYS_Y                  HUD_HEALTH_Y

#define HUD_AMMO_X                  (SCREENWIDTH - MAXWIDESCREENDELTA + 2)
#define HUD_AMMO_Y                  HUD_HEALTH_Y
#define HUD_AMMO_MIN                10
#define HUD_AMMO_WAIT               250
#define HUD_AMMO_HIGHLIGHT_WAIT     250

#define HUD_KEY_WAIT                250

#define ALTHUD_LEFT_X               (MAXWIDESCREENDELTA + 9)
#define ALTHUD_RIGHT_X              (SCREENWIDTH - MAXWIDESCREENDELTA - 136)
#define ALTHUD_Y                    (SCREENHEIGHT - 41)

#define DRAWDISKTICS                (12 * TICRATE)

//
// HEADS UP TEXT
//
void HU_Init(void);
void HU_SetTranslucency(void);
void HU_Start(void);

void HU_Ticker(void);
void HU_Drawer(void);
void HU_Erase(void);

void HU_SetPlayerMessage(char *message, bool group, bool external);
void HU_PlayerMessage(char *message, bool group, bool external);

void HU_ClearMessages(void);

extern patch_t  *hu_font[HU_FONTSIZE];
extern patch_t  *minuspatch;
extern patch_t  *buddha;

extern uint64_t healthhighlight;
extern uint64_t ammohighlight;
extern uint64_t armorhighlight;
extern int      ammodiff[NUMAMMO];
extern int      maxammodiff[NUMAMMO];
extern int      armordiff;
extern int      healthdiff;
extern bool     drawdisk;
extern int      drawdisktics;
extern bool     idbehold;
extern int      message_counter;
extern bool     message_dontfuckwithme;
extern bool     message_secret;
extern bool     message_warning;
extern bool     message_on;
extern bool     message_fadeon;
extern char     prevmessage[133];
extern short    minuspatchtopoffset1;
extern short    minuspatchtopoffset2;
extern short    minuspatchwidth;
