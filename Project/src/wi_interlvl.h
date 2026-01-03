

#pragma once

typedef enum
{
    AnimCondition_None,
    AnimCondition_MapNumGreater,
    AnimCondition_MapNumEqual,
    AnimCondition_MapVisited,
    AnimCondition_MapNotSecret,
    AnimCondition_SecretVisited,
    AnimCondition_Tally,
    AnimCondition_IsEntering
} animcondition_t;

typedef enum
{
    Frame_None           = 0x0000,
    Frame_Infinite       = 0x0001,
    Frame_FixedDuration  = 0x0002,
    Frame_RandomDuration = 0x0004,
    Frame_RandomStart    = 0x1000
} frametype_t;

typedef struct
{
    animcondition_t     condition;
    int                 param;
} interlevelcond_t;

typedef struct
{
    char                *imagelump;
    frametype_t         type;
    int                 duration;
    int                 maxduration;
} interlevelframe_t;

typedef struct
{
    interlevelframe_t   *frames;
    interlevelcond_t    *conditions;
    int                 xpos;
    int                 ypos;
} interlevelanim_t;

typedef struct
{
    interlevelanim_t    *anims;
    interlevelcond_t    *conditions;
} interlevellayer_t;

typedef struct
{
    char                *musiclump;
    char                *backgroundlump;
    interlevellayer_t   *layers;
} interlevel_t;

interlevel_t *WI_ParseInterlevel(const char *lumpname);
