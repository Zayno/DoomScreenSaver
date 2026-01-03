

#pragma once

#include "d_event.h"
#include "doomdef.h"
#include "m_misc.h"

#define DEVICE  "PC"
#define WINDOWS "Windows"
#define DESKTOP "desktop"

#define I_SDLError(func, offset) \
    I_Error("The call to %s() failed on line %s of %s: \n\"%s\"", \
        func, commify(__LINE__ + offset), leafname(__FILE__), SDL_GetError())

void I_ShutdownWindows32(void);

// Called by M_Responder when quit is selected.
void I_Quit(bool shutdown);

NORETURN void I_Error(const char *error, ...) FORMATATTR(1, 2);

void I_PrintWindowsVersion(void);
void I_PrintSystemInfo(void);

void *I_Malloc(size_t size);
void *I_Realloc(void *block, size_t size);
