

#pragma once

#include "doomdef.h"
#include "w_file.h"

//
// TYPES
//

//
// WADFILE I/O related stuff.
//

#define IWAD    1
#define PWAD    2

typedef struct
{
    char        name[9];
    int         size;
    void        *cache;

    // killough 01/31/98: hash table fields, used for ultra-fast hash table lookup
    int         index;
    int         next;

    int         position;

    wadfile_t   *wadfile;
} lumpinfo_t;

extern lumpinfo_t   **lumpinfo;
extern int          numlumps;
extern char         *wadsloaded;

bool IsUltimateDOOM(const char *iwadname);

char *GetCorrectCase(char *path);

char *W_GuessFilename(char *path, const char *string);

bool W_AddFile(char *filename, bool autoloaded);
bool W_AutoloadFile(const char *filename, const char *folder, const bool nonerveorsigil);
bool W_AutoloadFiles(const char *folder, const bool nonerveorsigil);
int W_WadType(char *filename);

int W_CheckNumForName(const char *name);

int W_CheckNumForNameFromTo(int min, int max, const char *name);
int W_GetNumForName(const char *name);
int W_GetLastNumForName(const char *name);
int W_GetXNumForName(const char *name, const int x);
int W_GetNumForNameFromResourceWAD(const char *name);
void W_HashNumForNameFromTo(int from, int to, int size);

int W_GetNumLumps(const char *name);
int W_GetNumLumps2(const char *name);

int W_LumpLength(int lump);

void *W_CacheLumpNum(int lumpnum);

#define W_CacheLumpName(name)                   W_CacheLumpNum(W_GetNumForName(name))
#define W_CacheLastLumpName(name)               W_CacheLumpNum(W_GetLastNumForName(name))
#define W_CacheXLumpName(name, x)               W_CacheLumpNum(W_GetXNumForName(name, x))
#define W_CacheLumpNameFromResourceWAD(name)    W_CacheLumpNum(W_GetNumForNameFromResourceWAD(name))

void W_Init(void);
void W_CheckForPNGLumps(void);

unsigned int W_LumpNameHash(const char *s);

void W_ReleaseLumpNum(int lumpnum);

#define W_ReleaseLumpName(name)     W_ReleaseLumpNum(W_GetNumForName(name))

void W_CloseFiles(void);

gamemission_t IWADRequiredByPWAD(char *pwadname);
bool HasDehackedLump(const char *pwadname);
