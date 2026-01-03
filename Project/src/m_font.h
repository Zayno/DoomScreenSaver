

#pragma once

#include "doomtype.h"

bool M_LoadFON2(byte *gfx_data, int size);
bool M_DrawFON2String(int x, int y, const char *str, bool highlight);
int M_GetFON2PixelWidth(const char *str);
