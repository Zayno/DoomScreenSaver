

#pragma once

#include "d_event.h"
#include "doomdef.h"
#include "r_defs.h"

#define PAGETICS    (20 * TICRATE)

extern patch_t  *titlelump;
extern patch_t  *creditlump;
extern char     **episodes[];
extern char     **expansions[];
extern char     **skilllevels[];
extern char     *resourcewad;
extern char     *pwadfile;
extern bool     advancetitle;
extern int      logotic;
extern int      pagetic;
extern int      titlesequence;

extern char     *previouswad;

void D_Display(void);

//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
//
void D_DoomMain(void);

// Called by IO functions when input is detected.
void D_PostEvent(event_t *ev);

//
// BASE LEVEL
//
void D_PageTicker(void);
void D_PageDrawer(void);
void D_DoAdvanceTitle(void);
void D_StartTitle(int page);
bool D_IsDOOMIWAD(char *filename);
bool D_IsDOOM1IWAD(char *filename);
bool D_IsDOOM2IWAD(char *filename);
bool D_IsNERVEWAD(char *filename);
bool D_IsLegacyOfRustWAD(char *filename);
bool D_IsEXTRASWAD(char *filename);
bool D_IsSIGILWAD(char *filename);
bool D_IsSIGILREGWAD(char *filename);
bool D_IsSIGILSHREDSWAD(char *filename);
bool D_IsSIGIL2WAD(char *filename);
bool D_IsFinalDOOMIWAD(char *filename);
bool D_IsResourceWAD(char *filename);
void D_CheckSupportedPWAD(char *filename);
