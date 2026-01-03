

#pragma once

#include "SDL_mixer.h"

#include "sounds.h"

#define CHUNKSIZE                   1024
#define SAMPLERATE                  44100

#define LOWER_MUSIC_VOLUME_FACTOR   2.5f

#define DEFAULT_DEVICE              NULL

bool I_InitSound(void);
void I_ShutdownSound(void);
bool CacheSFX(sfxinfo_t *sfxinfo);
void I_UpdateSoundParms(const int channel, const int vol, const int sep);
int I_StartSound(const sfxinfo_t *sfxinfo, const int channel,
    const int vol, const int sep, const int pitch);
void I_StopSound(const int channel);
bool I_SoundIsPlaying(const int channel);

bool I_InitMusic(void);
void I_ShutdownMusic(void);
void I_SetMusicVolume(const int volume);
void I_PauseSong(void);
void I_ResumeSong(void);
void *I_RegisterSong(void *data, int size);
void I_UnregisterSong(void *handle);
void I_PlaySong(void *handle, const bool looping);
void I_StopSong(void);
bool I_AnySoundStillPlaying(void);

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets s_sfx lookup.
//
void S_Init(void);

// Shut down sound
void S_Shutdown(void);

void S_StopSoundEffect(const sfxnum_t sfxnum);
void S_StopSound(const mobj_t *origin);
void S_StopSounds(void);

//
// Per level startup code.
// Kills playing sounds at start of level,
//  determines music if any, changes music.
//
void S_Start(void);

//
// Start sound for thing at <origin_p>
//  using <sfxnum> from sounds.h
//
void S_StartSound(mobj_t *mobj, const sfxnum_t sfxnum);
void S_StartSectorSound(degenmobj_t *degenmobj, const sfxnum_t sfxnum);
void S_UnlinkSound(const mobj_t *origin);

// Start music using <musicnum> from sounds.h
void S_StartMusic(const musicnum_t musicnum);

// Start music using <musicnum> from sounds.h,
//  and set whether looping
void S_ChangeMusic(const musicnum_t musicnum, const bool looping,
    const bool allowrestart, const bool mapstart);

// Stops the music for sure.
void S_StopMusic(void);

// Stop and resume music, during game PAUSE.
void S_PauseMusic(void);
void S_ResumeMusic(void);

//
// Updates music and sounds
//
void S_UpdateSounds(void);

void S_LowerMusicVolume(void);
void S_RestoreMusicVolume(void);
void S_SetSfxVolume(const int volume);

#define MAX_MUS_ENTRIES 64

typedef struct
{
    mobj_t  *mapthing;
    mobj_t  *lastmapthing;
    int     tics;
    int     currentitem;
    int     items[MAX_MUS_ENTRIES];
    bool    fromsavegame;
} musinfo_t;

extern musinfo_t    musinfo;

void S_ChangeMusInfoMusic(const int lumpnum, const bool looping);
void S_ParseMusInfo(const char *lumpname);
void MusInfoThinker(mobj_t *thing);
void T_MAPMusic(void);
