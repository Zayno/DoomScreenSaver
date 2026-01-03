

#pragma once

// Palette indices.
// For damage/bonus red-/gold-shifts
#define STARTREDPALS            1
#define NUMREDPALS              (PLAYPALs > 2 ? 8 : 20)
#define STARTBONUSPALS          (STARTREDPALS + NUMREDPALS)
#define NUMBONUSPALS            4
#define RADIATIONPAL            (STARTBONUSPALS + NUMBONUSPALS)

// Size of status bar.
#define ST_WIDTH                SCREENWIDTH

// Number of status faces.
#define ST_NUMPAINFACES         5
#define ST_NUMSTRAIGHTFACES     3
#define ST_NUMTURNFACES         2
#define ST_NUMSPECIALFACES      3

#define ST_FACESTRIDE           (ST_NUMSTRAIGHTFACES + ST_NUMTURNFACES + ST_NUMSPECIALFACES)

#define ST_NUMEXTRAFACES        2
#define ST_NUMXDTHFACES         9

#define ST_NUMFACES             (ST_FACESTRIDE * ST_NUMPAINFACES + ST_NUMEXTRAFACES + ST_NUMXDTHFACES)

#define ST_STRAIGHTFACE         1
#define ST_STRAIGHTFACECOUNT    (TICRATE / 2)

#define IDMUS_MAX               70

//
// STATUS BAR
//

// Called by main loop.
bool ST_Responder(const event_t *ev);

// Called by main loop.
void ST_Ticker(void);

// Called by main loop.
void ST_Drawer(bool fullscreen, bool refresh);

// Called when the console player is spawned on each level.
void ST_Start(void);

// Called by startup code.
void ST_Init(void);

void ST_InitStatBar(void);
void ST_PlayerCheated(const char *cheat, const char *parm, const char *output, const bool warning);

extern bool         idclev;
extern int          idclevtics;
extern bool         idmus;
extern int          st_palette;
extern bool         oldweaponsowned[NUMWEAPONS];
extern patch_t      *tallnum[10];
extern short        tallnum0width;
extern short        tallnum1width;
extern patch_t      *tallpercent;
extern short        tallpercentwidth;
extern bool         emptytallpercent;
extern int          caretcolor;
extern patch_t      *faces[ST_NUMFACES];
extern int          st_faceindex;
extern int          oldhealth;

extern byte         *grnrock;
extern patch_t      *brdr_t;
extern patch_t      *brdr_b;
extern patch_t      *brdr_l;
extern patch_t      *brdr_r;
extern patch_t      *brdr_tl;
extern patch_t      *brdr_tr;
extern patch_t      *brdr_bl;
extern patch_t      *brdr_br;

extern bool         st_drawbrdr;

extern const int    mus[IDMUS_MAX][6];
