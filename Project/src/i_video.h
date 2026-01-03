

#pragma once

#include <Windows.h>

#include "SDL.h"

#include "doomtype.h"

// Screen width and height.
#define VANILLAWIDTH        320
#define VANILLAHEIGHT       200

#define ACTUALVANILLAHEIGHT (VANILLAHEIGHT * 6 / 5)
#define ACTUALHEIGHT        (ACTUALVANILLAHEIGHT * 2)

#define VANILLASBARHEIGHT   32
#define SBARHEIGHT          (VANILLASBARHEIGHT * 2)

#define WIDEVANILLAWIDTH    (ACTUALVANILLAHEIGHT * 16 / 9)

#define NONWIDEWIDTH        (VANILLAWIDTH * 2)
#define NONWIDEASPECTRATIO  (4.0 / 3.0)

#define MAXWIDTH            (NONWIDEWIDTH * 6)
#define MAXHEIGHT           ((VANILLAHEIGHT + 1) * 2)
#define MAXSCREENAREA       (MAXWIDTH * MAXHEIGHT)

#define MAXWIDEFOVDELTA     32

#define MAXMOUSEBUTTONS     8

#define GAMMALEVELS         21

bool MouseShouldBeGrabbed(void);
void I_InitKeyboard(void);
void I_ShutdownKeyboard(void);
bool GetCapsLockState(void);

void ToggleCapsLockState(void);

// Called by D_DoomLoop,
// called before processing each tic in a frame.
// Quick synchronous operations are performed here.
// Can call D_PostEvent.
void I_StartTic(void);

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void);
void I_RestartGraphics(const bool recreatewindow);
void I_ShutdownGraphics(void);

void I_SaveMousePointerPosition(void);
void I_RestoreMousePointerPosition(void);

void GetWindowPosition(void);
void GetWindowSize(void);
void GetScreenResolution(void);

// Takes full 8 bit values.
void I_SetPalette(const byte *playpal);
void I_SetExternalAutomapPalette(void);

void I_UpdateBlitFunc(const bool shaking);
bool I_CreateExternalAutomap(void);
void I_DestroyExternalAutomap(void);

void I_ToggleFullscreen(const bool output);
void I_UpdateColors(void);
void I_SetMotionBlur(const int percent);

void I_SetGamma(const float value);

bool keystate(const int key);

void I_WindowResizeBlit(void);

extern void (*blitfunc)(void);
extern void (*mapblitfunc)(void);

extern int          SCREENWIDTH;
extern int          SCREENHEIGHT;
extern int          SCREENAREA;
extern int          WIDESCREENDELTA;
extern int          MAXWIDESCREENDELTA;
extern int          WIDEFOVDELTA;

extern bool         nowidescreen;
extern bool         vid_widescreen_copy;

extern int          shakedamage;

extern int          MAPWIDTH;
extern int          MAPHEIGHT;
extern int          MAPAREA;
extern int          MAPBOTTOM;

extern bool         sendpause;
extern bool         waspaused;

extern int          keydown;
extern int          keydown2;

extern bool         nokeyevent;

extern int          gammaindex;
extern const float  gammalevels[GAMMALEVELS];
extern float        brightness;

extern int          windowx;
extern int          windowy;
extern int          windowheight;
extern int          windowwidth;
extern int          windowborderwidth;
extern int          windowborderheight;

extern bool         usingcontroller;
extern bool         usingmouse;

extern SDL_Window   *window;

extern SDL_Window   *mapwindow;
extern byte         *mapscreen;

extern byte         *PLAYPAL;

extern bool         altdown;

