

#pragma once

#include "doomtype.h"
#include "m_fixed.h"

typedef enum
{
    SkyType_Normal,
    SkyType_Fire,
    SkyType_WithForeground
} skytype_t;

typedef struct
{
    byte        *palette;
    int         updatetime;
    int         ticsleft;
} fire_t;

typedef struct
{
    const char  *name;
    double      mid;
    fixed_t     scrollx;
    fixed_t     currx;
    fixed_t     scrolly;
    fixed_t     curry;
    fixed_t     scalex;
    fixed_t     scaley;
} skytex_t;

typedef struct
{
    skytype_t   type;
    skytex_t    skytex;
    fire_t      fire;
    skytex_t    foreground;
} sky_t;

typedef struct
{
    const char  *flat;
    const char  *sky;
} flatmap_t;

typedef struct
{
    sky_t       *skies;
    flatmap_t   *flatmapping;
} skydefs_t;

skydefs_t *R_ParseSkyDefs(void);
