

#pragma once

// Endianness handling.
// WAD files are stored little endian.

// Just use SDL's endianness swapping functions.

// These are deliberately cast to signed values; this is the behavior
// of the macros in the original source and some code relies on it.
#define SHORT(x)    ((signed short)SDL_SwapLE16(x))
#define LONG(x)     ((signed int)SDL_SwapLE32(x))
