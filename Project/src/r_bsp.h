

#pragma once

extern seg_t        *curline;
extern line_t       *linedef;
extern sector_t     *frontsector;
extern sector_t     *backsector;

extern drawseg_t    *drawsegs;

extern byte         *solidcol;

extern drawseg_t    *ds_p;

// BSP?
void R_InitClipSegs(void);
void R_ClearClipSegs(void);
void R_ClearDrawSegs(void);

void R_RenderBSPNode(int bspnum);

// killough 04/13/98: fake floors/ceilings for deep water/fake ceilings:
sector_t *R_FakeFlat(sector_t *sec, sector_t *tempsec,
    int *floorlightlevel, int *ceilinglightlevel, const bool back);
