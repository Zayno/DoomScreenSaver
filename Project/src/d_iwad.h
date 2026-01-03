

#pragma once

#include "doomdef.h"
#include "w_file.h"

extern char screenshotfolder[MAX_PATH];

char *D_FindWADByName(char *filename);
char *D_TryFindWADByName(char *filename);
char *D_FindIWAD(void);
void D_SetSaveGameFolder(bool output);
void D_SetAutoloadFolder(void);
void D_SetScreenshotsFolder(void);
void D_IdentifyVersion(void);
void D_SetGameDescription(void);
void D_IdentifyIWADByName(char *name);
void D_InitWADfolder(void);
