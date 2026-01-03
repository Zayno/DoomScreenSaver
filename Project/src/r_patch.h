

#pragma once

typedef struct
{
    int             topdelta;
    int             length;
} rpost_t;

typedef struct
{
    int             numposts;
    rpost_t         *posts;
    unsigned char   *pixels;
} rcolumn_t;

typedef struct
{
    int             width;
    int             height;
    unsigned int    widthmask;

    int             leftoffset;
    int             topoffset;

    // this is the single malloc'ed/free'd array
    // for this patch
    unsigned char   *data;

    // these are pointers into the data array
    unsigned char   *pixels;
    rcolumn_t       *columns;
    rpost_t         *posts;
} rpatch_t;

const rpatch_t *R_CachePatchNum(const int id);

const rpatch_t *R_CacheTextureCompositePatchNum(const int id);

const rcolumn_t *R_GetPatchColumnWrapped(const rpatch_t *patch, int columnindex);
const rcolumn_t *R_GetPatchColumnClamped(const rpatch_t *patch, int columnindex);

void R_InitPatches(void);
bool R_CheckIfPatch(const int lump);
