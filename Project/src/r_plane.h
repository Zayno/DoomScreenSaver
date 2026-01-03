

#pragma once

// killough 10/98: special mask indicates sky flat comes from sidedef
#define PL_SKYFLAT      0x40000000
#define PL_FLATMAPPING  0xC0000000

// Visplane related.
extern int      *lastopening;
extern int      floorclip[MAXWIDTH];
extern int      ceilingclip[MAXWIDTH];
extern fixed_t  *yslope;
extern fixed_t  yslopes[LOOKDIRS][MAXHEIGHT];
extern int      openings[MAXOPENINGS];

void R_ClearPlanes(void);
void R_DrawPlanes(void);
visplane_t *R_FindPlane(fixed_t height, const int picnum, int lightlevel,
    const fixed_t x, const fixed_t y, const int colormap, const angle_t angle);
visplane_t *R_CheckPlane(visplane_t *pl, const int start, const int stop);
visplane_t *R_DupPlane(const visplane_t *pl, const int start, const int stop);
void R_InitDistortedFlats(void);
