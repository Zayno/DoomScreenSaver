

#include "SDL_mixer.h"

#include "c_console.h"
#include "doomstat.h"
#include "i_winmusic.h"
#include "m_config.h"
#include "memio.h"
#include "mus2mid.h"
#include "s_sound.h"
#include "w_wad.h"
#include "z_zone.h"

bool        midimusictype;
bool        musmusictype;

bool        windowsmidi = false;

static bool music_initialized;

int         current_music_volume = 0;

// Shutdown music
void I_ShutdownMusic(void)
{
    if (!music_initialized)
        return;

    music_initialized = false;

    if (mus_playing)
        I_UnregisterSong(mus_playing->handle);

    Mix_CloseAudio();

    if (windowsmidi)
    {
        I_Windows_ShutdownMusic();
        windowsmidi = false;
    }
}

// Initialize music subsystem
bool I_InitMusic(void)
{
    int         freq = MIX_DEFAULT_FREQUENCY;
    int         channels;
    uint16_t    format;

    // Never let SDL Mixer use native MIDI on Windows. Avoids SDL Mixer bug
    // where music volume affects global application volume.
    SDL_setenv("SDL_MIXER_DISABLE_NATIVEMIDI", "1", true);

    // If SDL_mixer is not initialized, we have to initialize it and have the
    // responsibility to shut it down later on.
    if (!Mix_QuerySpec(&freq, &format, &channels))
        if (Mix_OpenAudioDevice(SAMPLERATE, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS,
            CHUNKSIZE, DEFAULT_DEVICE, SDL_AUDIO_ALLOW_ANY_CHANGE) < 0)
            return false;

    music_initialized = true;

    if (extras && W_CheckNumForName("H_INTRO") >= 0 && (!sigil || buckethead) && (!sigil2 || thorr) && !legacyofrust)
        return true;

    if (!(windowsmidi = I_Windows_InitMusic()))
        C_Warning(1, "Music couldn't be completely %s. Volume adjustment could be affected.",
            (english == english_american ? "initialized" : "initialised"));

    return music_initialized;
}

// Set music volume (0 - 127)
void I_SetMusicVolume(const int volume)
{
    // Internal state variable.
    current_music_volume = volume;

    if (midimusictype && windowsmidi)
        I_Windows_SetMusicVolume(current_music_volume);
    else
        Mix_VolumeMusic(current_music_volume);
}

// Start playing a mid
void I_PlaySong(void *handle, const bool looping)
{
    if (!music_initialized)
        return;

    if (midimusictype && windowsmidi)
        I_Windows_PlaySong(looping);
    else if (handle)
        Mix_PlayMusic(handle, (looping ? -1 : 1));
}

void I_PauseSong(void)
{
    if (!music_initialized)
        return;

    if (midimusictype && windowsmidi)
        I_Windows_PauseSong();
    else
        Mix_PauseMusic();
}

void I_ResumeSong(void)
{
    if (!music_initialized)
        return;

    if (midimusictype && windowsmidi)
        I_Windows_ResumeSong();
    else
        Mix_ResumeMusic();
}

void I_StopSong(void)
{
    if (!music_initialized)
        return;

    if (windowsmidi)
        I_Windows_StopSong();

    Mix_HaltMusic();
}

void I_UnregisterSong(void *handle)
{
    if (!music_initialized)
        return;

    if (windowsmidi)
        I_Windows_UnregisterSong();
    else if (handle)
        Mix_FreeMusic(handle);
}

void *I_RegisterSong(void *data, int size)
{
    if (!music_initialized)
        return NULL;
    else
    {
        Mix_Music   *music = NULL;
        SDL_RWops   *rwops = NULL;

        midimusictype = false;
        musmusictype = false;

        // Check for MIDI or MUS format first:
        if (size >= 14)
        {
            if (!memcmp(data, "MThd", 4))           // is it a MIDI?
                midimusictype = true;
            else if (!memcmp(data, "MUS\x1A", 4))   // is it a MUS?
            {
                MEMFILE *instream = mem_fopen_read(data, size);
                MEMFILE *outstream = mem_fopen_write();

                musmusictype = true;

                if (mus2mid(instream, outstream))
                {
                    void    *outbuf;
                    byte    *mid;
                    size_t  midlen;

                    mem_get_buf(outstream, &outbuf, &midlen);

                    if ((mid = Z_Malloc(midlen, PU_LEVEL, NULL)))
                    {
                        memcpy(mid, outbuf, midlen);
                        data = mid;
                        size = (int)midlen;
                    }
                }

                mem_fclose(instream);
                mem_fclose(outstream);

                midimusictype = true;               // now it's a MIDI
            }
        }

        if (midimusictype && windowsmidi)
        {
            I_Windows_RegisterSong(data, size);
            return NULL;
        }

        if ((rwops = SDL_RWFromMem(data, size)))
            music = Mix_LoadMUS_RW(rwops, 0);

        return music;
    }
}
