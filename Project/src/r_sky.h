

#pragma once

#include "r_skydefs.h"

// SKY, store the number for name.
#define SKYFLATNAME         "F_SKY1"

// The sky map is 256 * 128 * 4 maps.
#define ANGLETOSKYSHIFT     22

#define SKYSTRETCH_HEIGHT   (VANILLAHEIGHT + (r_screensize < r_screensize_max && !menuactive ? 38 : 64))

#define FIREWIDTH           128
#define FIREHEIGHT          320

extern int      skytexture;
extern int      skytexturemid;
extern int      skycolumnoffset;
extern int      skyscrolldelta;
extern fixed_t  skyiscale;
extern bool     canfreelook;
extern sky_t    *sky;

void R_InitSkyMap(void);
void R_UpdateSky(void);
byte *R_GetFireColumn(int col);
