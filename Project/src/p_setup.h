

#pragma once

typedef struct
{
    int type;
    int special;
    int tag;
} bossaction_t;

extern bool         canmodify;
extern bool         samelevel;
extern bool         secretmap;
extern bool         skipblstart;
extern bool         transferredsky;
extern const char   *linespecials[];

extern char         *mapinfolump;

extern bool         compat_corpsegibs;
extern bool         compat_floormove;
extern bool         compat_light;
extern bool         compat_limitpain;
extern bool         compat_nopassover;
extern bool         compat_stairs;
extern bool         compat_useblocking;
extern bool         compat_zombie;
extern bool         nograduallighting;

extern char         mapnum[16];
extern char         maptitle[256];
extern char         mapnumandtitle[512];
extern char         automaptitle[512];

void P_SetupLevel(int ep, int map);
void P_MapName(int ep, int map);

// Called by startup code.
void P_Init(void);

const char *P_GetMapAuthor(const int ep, const int map);
int P_GetNumBossActions(const int ep, const int map);
bossaction_t *P_GetBossAction(const int ep, const int map, const int i);
char *P_GetInterBackrop(const int ep, const int map);
int P_GetInterMusic(const int ep, const int map);
char *P_GetInterText(const int ep, const int map);
char *P_GetInterSecretText(const int ep, const int map);
char *P_GetLabel(const int ep, const int map);
bool P_GetMapEndBunny(const int ep, const int map);
bool P_GetMapEndCast(const int ep, const int map);
bool P_GetMapEndGame(const int ep, const int map);
int P_GetMapEndPic(const int ep, const int map);
char *P_GetMapEnterAnim(const int ep, const int map);
int P_GetMapEnterPic(const int ep, const int map);
char *P_GetMapExitAnim(const int ep, const int map);
int P_GetMapExitPic(const int ep, const int map);
void P_GetMapLiquids(const int ep, const int map);
int P_GetMapMusic(const int ep, const int map);
char *P_GetMapMusicComposer(const int ep, const int map);
char *P_GetMapMusicTitle(const int ep, const int map);
char *P_GetMapName(const int ep, const int map);
int P_GetMapNext(const int ep, const int map);
bool P_GetMapNoFreelook(const int ep, const int map);
bool P_GetMapNoJump(const int ep, const int map);
void P_GetMapNoLiquids(const int ep, const int map);
int P_GetMapPar(const int ep, const int map);
bool P_GetMapPistolStart(const int ep, const int map);
int P_GetMapSecretNext(const int ep, const int map);
int P_GetMapSky1Texture(const int ep, const int map);
float P_GetMapSky1ScrollDelta(const int ep, const int map);
int P_GetMapTitlePatch(const int ep, const int map);
int P_GetAllowMonsterTelefrags(const int ep, const int map);
bool P_IsSecret(const int ep, const int map);
