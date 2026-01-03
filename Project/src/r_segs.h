

#pragma once

#define SPARKLEFIX  64

extern unsigned int maxdrawsegs;

void R_RenderMaskedSegRange(const drawseg_t *ds, const int x1, const int x2);
void R_StoreWallRange(const int start, const int stop);
