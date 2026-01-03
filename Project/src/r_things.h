

#pragma once

// Constant arrays used for psprite clipping
//  and initializing clipping.
extern int      negonearray[MAXWIDTH];
extern int      viewheightarray[MAXWIDTH];

// vars for R_DrawMaskedColumn
extern int      *mfloorclip;
extern int      *mceilingclip;
extern fixed_t  spryscale;
extern int64_t  sprtopscreen;

extern fixed_t  pspritescale;
extern fixed_t  pspriteiscale;

extern short    firstbloodsplatlump;
extern int      numbloodsplatlumps;

extern bool     allowwolfensteinss;

void R_AddSprites(sector_t *sec, int lightlevel);
void R_AddNearbySprites(sector_t *sec);
void R_InitSprites(void);
void R_ClearSprites(void);
void R_DrawMasked(void);
void R_DrawNearbySprites(void);
