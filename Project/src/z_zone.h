

#pragma once

// Include system definitions so that prototypes become
// active before macro replacements below are in effect.
#include <string.h>

#include "doomdef.h"

//
// ZONE MEMORY
// PU - purge tags.
//
enum
{
    PU_FREE,       // a free block
    PU_STATIC,     // static entire execution time
    PU_LEVEL,      // static until level exited
    PU_LEVSPEC,    // a special thinker in a level
    PU_CACHE,
    PU_MAX         // Must always be last -- killough
};

#define PU_PURGELEVEL    PU_CACHE    // First purgeable tag's level

void *Z_Malloc(size_t size, unsigned char tag, void **user) ALLOCATTR(1);
void *Z_Calloc(size_t size1, size_t size2, unsigned char tag, void **user) ALLOCSATTR(1, 2);
char *Z_StringDuplicate(const char *, unsigned char tag, void **user);
void Z_Free(void *ptr);
void Z_FreeTags(unsigned char lowtag, unsigned char hightag);
void Z_ChangeTag(void *ptr, unsigned char tag);
