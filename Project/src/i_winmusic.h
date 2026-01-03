


#pragma once

#include "doomtype.h"

bool I_Windows_InitMusic(void);
void I_Windows_PlaySong(bool looping);
void I_Windows_PauseSong(void);
void I_Windows_ResumeSong(void);
void I_Windows_StopSong(void);
void I_Windows_SetMusicVolume(int volume);
void I_Windows_RegisterSong(void *data, int size);
void I_Windows_UnregisterSong(void);
void I_Windows_ShutdownMusic(void);

