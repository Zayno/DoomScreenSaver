

#pragma once

#include "doomtype.h"
#include "m_config.h"
#include "r_defs.h"

#define BLACK           0
#define BLUE1         198
#define BLUE2         201
#define BLUE3         203
#define DARKBLUE      244
#define DARKGRAY1     102
#define DARKGRAY2     106
#define DARKGRAY3     109
#define DARKGRAY4     111
#define GRAY1          92
#define GRAY2          96
#define GREEN1        112
#define GREEN2        114
#define GREEN3        118
#define GREEN4        125
#define LIGHTGRAY1     80
#define LIGHTGRAY2     82
#define LIGHTGRAY3     86
#define PINK          251
#define RED1          176
#define RED2          180
#define WHITE           4
#define YELLOW1       161
#define YELLOW2       231

#define CTOLERANCE 0.0001

extern byte *tinttab4;
extern byte *tinttab5;
extern byte *tinttab10;
extern byte *tinttab15;
extern byte *tinttab20;
extern byte *tinttab25;
extern byte *tinttab30;
extern byte *tinttab33;
extern byte *tinttab40;
extern byte *tinttab45;
extern byte *tinttab50;
extern byte *tinttab60;
extern byte *tinttab66;
extern byte *tinttab70;
extern byte *tinttab75;
extern byte *tinttab80;
extern byte *tinttab90;

extern byte *tranmap;
extern byte *tinttabadditive;
extern byte *tinttabred;
extern byte *tinttabredwhite1;
extern byte *tinttabredwhite2;
extern byte *tinttabgreen;
extern byte *tinttabblue;
extern byte *tinttabred33;
extern byte *tinttabredwhite50;
extern byte *tinttabgreen33;
extern byte *tinttabblue25;

extern byte nearestcolors[256];

extern byte nearestblack;
extern byte nearestdarkblue;
extern byte nearestdarkgray;
extern byte nearestgold;
extern byte nearestgreen;
extern byte nearestlightgray;
extern byte nearestred;
extern byte nearestwhite;

extern byte *black10;
extern byte *black25;
extern byte *black40;
extern byte *black75;
extern byte *gold4;
extern byte *white5;
extern byte *white25;
extern byte *white33;
extern byte *white75;

extern byte luminance[256];

void I_InitTintTables(byte *palette);
int FindNearestColor(byte *palette, const byte red, const byte green, const byte blue);
void I_InitColors(byte *palette);

int FindBrightDominantColor(patch_t *patch);
int FindDominantEdgeColor(patch_t *patch);

byte I_GoldTranslation(byte *playpal, byte color);
