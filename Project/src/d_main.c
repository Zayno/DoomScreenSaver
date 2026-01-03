

#define __STDC_WANT_LIB_EXT1__  1

#include <time.h>

#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <commdlg.h>

#include "am_map.h"
#include "c_cmds.h"
#include "c_console.h"
#include "d_deh.h"
#include "d_iwad.h"
#include "doomstat.h"
#include "f_finale.h"
#include "f_wipe.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "i_colors.h"
#include "i_controller.h"
#include "i_swap.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_menu.h"
#include "m_misc.h"
#include "p_local.h"
#include "p_setup.h"
#include "s_sound.h"
#include "st_stuff.h"
#include "v_video.h"
#include "version.h"
#include "w_merge.h"
#include "w_wad.h"
#include "wi_stuff.h"
#include "m_random.h"
#include"..\msvc\resource.h"

char FullDoomFileTempPath[MAX_PATH] = { 0 };
char FullDoomRetroFileTempPath[MAX_PATH] = { 0 };


#define FADECOUNT    8
#define FADETICS     25

char **episodes[] =
{
    &s_CAPTION_EPISODE1,
    &s_CAPTION_EPISODE2,
    &s_CAPTION_EPISODE3,
    &s_CAPTION_EPISODE4,
    &s_CAPTION_EPISODE5,
    &s_CAPTION_EPISODE6,
    &s_CAPTION_EPISODE7,
    &s_CAPTION_EPISODE8,
    &s_CAPTION_EPISODE9,
    &s_CAPTION_EPISODE10
};

char **expansions[] =
{
    &s_CAPTION_EXPANSION1,
    &s_CAPTION_EXPANSION2
};

char **skilllevels[] =
{
    &s_M_SKILLLEVEL1,
    &s_M_SKILLLEVEL2,
    &s_M_SKILLLEVEL3,
    &s_M_SKILLLEVEL4,
    &s_M_SKILLLEVEL5
};

static char *iwadsrequired[] =
{
    "doom.wad",
    "doom2.wad",
    "tnt.wad",
    "plutonia.wad",
    "nerve.wad",
    "doom2.wad"
};

// Location where savegames are stored
char        *savegamefolder;

char        *autoloadfolder;
char        *autoloadiwadsubfolder;
char        *autoloadpwadsubfolder;
char        *autoloadsigilsubfolder = "";
char        *autoloadsigil2subfolder = "";
char        *autoloadnervesubfolder = "";

char        *pwadfile = "";

char        *resourcewad;

static char dehwarning[256];

char        *previouswad;

bool        devparm;            // started game with -devparm
bool        fastparm;           // checkparm of -fast
bool        freeze;
bool        infiniteammo;
bool        nomonsters;         // checkparm of -nomonsters
bool        pistolstart;        // [BH] checkparm of -pistolstart
bool        regenhealth;
bool        respawnitems;
bool        respawnmonsters;    // checkparm of -respawn
bool        solonet;            // checkparm of -solo-net

skill_t     startskill;
int         startepisode;
static int  startmap;
bool        autostart;

bool        advancetitle;



bool        realframe;
static bool error;

//extern ULONGLONG g_StartTime;

static void I_SetPriority(bool active)
{
    if (active)
    {
        SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    }
    else
    {
        SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL);
    }
}


//
// D_PostEvent
//
void D_PostEvent(event_t *ev)
{
    if (M_Responder(ev))
        return; // menu ate the event

    G_Responder(ev);
}

//
// D_Display
//  draw current display, possibly wiping it from the previous
//


void D_Display(void)
{
    static bool         pausedstate;
    static gamestate_t  oldgamestate = GS_NONE;


    // change the view size if needed
    if (setsizeneeded)
    {
        R_ExecuteSetViewSize();
        oldgamestate = GS_NONE; // force background redraw
    }

    if (gamestate != GS_LEVEL)
    {
        if (gamestate != oldgamestate)
            I_SetPalette(PLAYPAL);

        switch (gamestate)
        {
            case GS_INTERMISSION:
                WI_Drawer();
                break;

            case GS_FINALE:
                F_Drawer();
                break;

            case GS_TITLESCREEN:
                D_PageDrawer();
                break;

            default:
                break;
        }
    }
    else
    {
        HU_Erase();

        // draw the view directly
        R_RenderPlayerView();

        if (mapwindow || automapactive)
            AM_Drawer();

        if (!menuactive)
        {
            ST_Drawer((viewheight == SCREENHEIGHT), true);

            // see if the border needs to be initially drawn
            if (oldgamestate != GS_LEVEL && viewwidth != SCREENWIDTH)
                R_FillBackScreen();

            // see if the border needs to be updated to the screen
            if (!automapactive)
            {
                if (viewwidth != SCREENWIDTH)
                    R_DrawViewBorder();

                if (r_detail == r_detail_low)
                    postprocessfunc(screens[0], SCREENWIDTH, viewwindowx, viewwindowy * SCREENWIDTH,
                        viewwindowx + viewwidth, (viewwindowy + viewheight) * SCREENWIDTH,
                        lowpixelwidth, lowpixelheight);
            }

            HU_Drawer();
        }
    }

    oldgamestate = gamestate;

    // draw pause pic
    if ((pausedstate = paused))
    {
        M_DrawMenuBackground();

        if (M_PAUSE)
        {
            patch_t *patch = W_CacheLumpName("M_PAUSE");

            V_DrawMenuPatch((VANILLAWIDTH - SHORT(patch->width)) / 2,
                (VANILLAHEIGHT - SHORT(patch->height)) / 2, patch, false, SCREENWIDTH);
        }
        else
            M_DrawCenteredString((VANILLAHEIGHT - 16) / 2, s_M_PAUSED);
    }

    if (loadaction != ga_nothing)
        G_LoadedGameMessage();

    
    {
        if (!paused && !menuactive)
        {
            if (gamestate == GS_LEVEL)
            {
                if (timer)
                    C_UpdateTimerOverlay();

                if (viewplayer->cheats & CF_MYPOS)
                    C_UpdatePlayerPositionOverlay();

                if ((pathoverlay = (am_path && (automapactive || mapwindow))))
                    C_UpdatePathOverlay();

                if (am_playerstats && (automapactive || mapwindow))
                    C_UpdatePlayerStatsOverlay();
            }
        }

        //if (consoleheight)
        //    C_Drawer();

        // menus go directly to the screen
        M_Drawer();



        // normal update
        blitfunc();
        mapblitfunc();

        return;
    }

    return;

}

static LARGE_INTEGER perf_freq;
int MainLoopTics = 0;


//
// D_DoomLoop
//
static void D_DoomLoop(void)
{
    player_t    player = { 0 };

    viewplayer = &player;
    memset(viewplayer, 0, sizeof(*viewplayer));

    R_ExecuteSetViewSize();

    QueryPerformanceFrequency(&perf_freq);

    LONGLONG frame_duration = (perf_freq.QuadPart / TICRATE);

    LARGE_INTEGER Accumulate = { 0 };

	bool PauseToggle = false;
    while (true)
    {
        LARGE_INTEGER StartTime;
        LARGE_INTEGER NewTime;
        QueryPerformanceCounter(&StartTime);

		M_SetRNGState(MainLoopTics);
        TryRunTics();
        D_Display();

        QueryPerformanceCounter(&NewTime);

        long long elapsedMilliSecons = (NewTime.QuadPart - StartTime.QuadPart) * 1000LL / perf_freq.QuadPart;

        if (elapsedMilliSecons < 29)
        {
			//timing doesn't need to be very accurate here. keep it around 35 fps.
			//raising and lowering sleep time here does not break the screensaver. It makes it either go slow or fast.
			DWORD SleepMS = (DWORD)(29 - elapsedMilliSecons);
            Sleep(SleepMS);
        }

        MainLoopTics++;

        if (MainLoopTics > 190086)
        {
            //finished all pre-recorded inputs. Exit program.
            //TODO: implement screen saver loop instead of exiting.
			exit(0);
        }
    }
}

//
// TITLE LOOP
//
int             titlesequence = 0;
int             pagetic = 3 * TICRATE;
int             logotic = 3 * TICRATE;

static patch_t  *pagelump;
patch_t         *creditlump;
patch_t         *titlelump;

static patch_t  *fineprintlump;
static patch_t  *logolump[18];
static byte     *splashpal;
static short    fineprintx;
static short    fineprinty;
static short    fineprintwidth;
static short    fineprintheight;
static short    logox;
static short    logoy;
static short    logowidth;
static short    logoheight;

//
// D_PageTicker
//
void D_PageTicker(void)
{
    return;
}

//
// D_PageDrawer
//
void D_PageDrawer(void)
{
    V_DrawPagePatch(0, pagelump);
}

//
// This cycles through the title sequence.
//
void D_DoAdvanceTitle(void)
{
    viewplayer->playerstate = PST_LIVE;  // not reborn
    advancetitle = false;
    paused = false;
    gameaction = ga_nothing;
    gamestate = GS_TITLESCREEN;

    if (titlesequence == 1)
    {
        static bool flag = true;

        if (vid_widescreen_copy)
        {
            vid_widescreen_copy = false;
            vid_widescreen = true;
            I_RestartGraphics(false);
        }

        if (flag)
        {
            flag = false;
            I_InitKeyboard();

            if (alwaysrun)
                C_StringCVAROutput(stringize(alwaysrun), "on");
        }

        pagelump = titlelump;
        pagetic = PAGETICS;

        M_SetWindowCaption();
        S_StartMusic(gamemode == commercial ? mus_dm2ttl : mus_intro);

        if (devparm)
            C_ShowConsole(false);
    }
    else if (titlesequence == 2)
    {
        pagelump = creditlump;
        pagetic = PAGETICS;
    }

    if (++titlesequence > 2)
        titlesequence = 1;
}

//
// D_StartTitle
//
void D_StartTitle(int page)
{
    gameaction = ga_nothing;
    titlesequence = page;

    if (mapwindow)
        AM_ClearFB();

    advancetitle = true;
}

#define MAXDEHFILES 16

static char dehfiles[MAXDEHFILES][MAX_PATH];
static int  dehfilecount;

static bool DehFileProcessed(const char *path)
{
    for (int i = 0; i < dehfilecount; i++)
        if (M_StringCompare(path, dehfiles[i]))
            return true;

    return false;
}

static char *FindDehPath(char *path, const char *ext, char *pattern)
{
    // Returns a malloc'd path to the .deh file that matches a WAD path.
    // Or NULL if no matching .deh file can be found.
    // The pattern (not used in Windows) is the fnmatch pattern to search for.
    char    *dehpath = M_StringDuplicate(path);

    if (M_StringEndsWith(path, ".wad"))
        dehpath = M_StringReplaceFirst(path, ".wad", ext);
    else if (M_StringEndsWith(path, ".iwad"))
        dehpath = M_StringReplaceFirst(path, ".iwad", ext);
    else if (M_StringEndsWith(path, ".pwad"))
        dehpath = M_StringReplaceFirst(path, ".pwad", ext);

    return (M_FileExists(dehpath) ? dehpath : NULL);
}

static void LoadDEHFile(char *path, bool autoloaded)
{
    char    *dehpath = FindDehPath(path, ".bex", ".[Bb][Ee][Xx]");

    if (dehpath)
    {
        if (!DehFileProcessed(dehpath))
        {
            if (!HasDehackedLump(path))
                D_ProcessDehFile(dehpath, 0, autoloaded);

            if (dehfilecount < MAXDEHFILES)
            {
                M_StringCopy(dehfiles[dehfilecount], dehpath, sizeof(dehfiles[0]));
                dehfilecount++;
            }
        }
    }
    else
    {
        dehpath = FindDehPath(path, ".deh", ".[Dd][Ee][Hh]");

        if (dehpath && !DehFileProcessed(dehpath))
        {
            if (!HasDehackedLump(path))
                D_ProcessDehFile(dehpath, 0, autoloaded);

            if (dehfilecount < MAXDEHFILES)
            {
                M_StringCopy(dehfiles[dehfilecount], dehpath, sizeof(dehfiles[0]));
                dehfilecount++;
            }
        }
    }
}


bool D_IsDOOM1IWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "DOOM.WAD")
        || M_StringCompare(file, "DOOM1.WAD")
        || M_StringCompare(file, "DOOMU.WAD")
        || M_StringCompare(file, "BFGDOOM.WAD")
        || M_StringCompare(file, "KEXDOOM.WAD")
        || M_StringCompare(file, "UNITYDOOM.WAD")
        || M_StringCompare(file, "DOOMBFG.WAD")
        || M_StringCompare(file, "DOOMKEX.WAD")
        || M_StringCompare(file, "DOOMUNITY.WAD"));
}

bool D_IsSIGILWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "SIGIL.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT_V1_0.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT_V1_1.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT_V1_2.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT_V1_21.WAD")
        || M_StringCompare(file, "SIGIL_COMPAT_V1_23.WAD")
        || M_StringCompare(file, "SIGIL_V1_0.WAD")
        || M_StringCompare(file, "SIGIL_V1_1.WAD")
        || M_StringCompare(file, "SIGIL_V1_2.WAD")
        || M_StringCompare(file, "SIGIL_V1_21.WAD")
        || M_StringCompare(file, "SIGIL_V1_23.WAD")
        || M_StringCompare(file, "SIGIL_V1_23_REG.WAD")
        || M_StringCompare(file, "SIGIL1.WAD"));
}

bool D_IsSIGILREGWAD(char *filename)
{
    return (M_StringCompare(leafname(filename), "SIGIL_V1_23_REG.WAD"));
}

bool D_IsSIGILSHREDSWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "SIGIL_SHREDS.WAD")
        || M_StringCompare(file, "SIGIL_SHREDS_COMPAT.WAD"));
}

bool D_IsSIGIL2WAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "SIGIL_II_V1_0.WAD")
        || M_StringCompare(file, "SIGIL_II_MP3_V1_0.WAD")
        || M_StringCompare(file, "SIGIL2.WAD"));
}

bool D_IsDOOM2IWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "DOOM2.WAD")
        || M_StringCompare(file, "DOOM2F.WAD")
        || M_StringCompare(file, "BFGDOOM2.WAD")
        || M_StringCompare(file, "KEXDOOM2.WAD")
        || M_StringCompare(file, "UNITYDOOM2.WAD")
        || M_StringCompare(file, "DOOM2BFG.WAD")
        || M_StringCompare(file, "DOOM2KEX.WAD")
        || M_StringCompare(file, "DOOM2UNITY.WAD"));
}

bool D_IsNERVEWAD(char *filename)
{
    return (M_StringCompare(leafname(filename), "NERVE.WAD"));
}

bool D_IsLegacyOfRustWAD(char *filename)
{
    return (M_StringCompare(leafname(filename), "ID1.WAD"));
}

bool D_IsEXTRASWAD(char *filename)
{
    return (M_StringCompare(leafname(filename), "extras.wad"));
}

bool D_IsDOOMIWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (D_IsDOOM1IWAD(filename)
        || D_IsDOOM2IWAD(filename)
        || M_StringCompare(file, "chex.wad")
        || M_StringCompare(file, "rekkrsa.wad"));
}

bool D_IsFinalDOOMIWAD(char *filename)
{
    const char  *file = leafname(filename);

    return (M_StringCompare(file, "PLUTONIA.WAD")
        || M_StringCompare(file, "TNT.WAD"));
}

bool D_IsResourceWAD(char *filename)
{
    return (M_StringCompare(leafname(filename), DOOMRETRO_RESOURCEWAD));
}

static bool D_IsUnsupportedIWAD(char *filename)
{
    const struct
    {
        char    *iwad;
        char    *title;
    } unsupported[] = {
        { "heretic.wad",  "Heretic" },
        { "heretic1.wad", "Heretic" },
        { "hexen.wad",    "Hexen"   },
        { "hexdd.wad",    "Hexen"   },
        { "strife0.wad",  "Strife"  },
        { "strife1.wad",  "Strife"  },
        { "voices.wad",   "Strife"  }
    };

    for (int i = 0; i < arrlen(unsupported); i++)
        if (M_StringCompare(leafname(filename), unsupported[i].iwad))
        {
            char    buffer[1024];

            M_snprintf(buffer, sizeof(buffer), DOOMRETRO_NAME " doesn't support %s.\n", unsupported[i].title);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, DOOMRETRO_NAME, buffer, NULL);

            if (previouswad)
                wad = M_StringDuplicate(previouswad);

            error = true;
            return true;
        }

    return false;
}

static bool D_IsWADFile(const char *filename)
{
    return (M_StringEndsWith(filename, ".wad") || M_StringEndsWith(filename, ".iwad")
        || M_StringEndsWith(filename, ".pwad"));
}

static bool D_IsCFGFile(const char *filename)
{
    return M_StringEndsWith(filename, ".cfg");
}

static bool D_IsDEHFile(const char *filename)
{
    return (M_StringEndsWith(filename, ".deh") || M_StringEndsWith(filename, ".bex"));
}

void D_CheckSupportedPWAD(char *filename)
{
    const char  *leaf = leafname(filename);

    if (M_StringCompare(leaf, "NERVE.WAD"))
    {
        nerve = true;
        expansion = 2;
    }
    else if (M_StringCompare(leaf, "chex.wad"))
        chex = chex1 = true;
    else if (M_StringCompare(leaf, "chex2.wad"))
        chex = chex2 = true;
    else if (M_StringCompare(leaf, "aaliens.wad"))
        moreblood = true;
    else if (M_StringCompare(leaf, "btsx_e1.wad"))
        BTSX = BTSXE1 = true;
    else if (M_StringCompare(leaf, "btsx_e1a.wad"))
        BTSX = BTSXE1 = BTSXE1A = true;
    else if (M_StringCompare(leaf, "btsx_e1b.wad"))
        BTSX = BTSXE1 = BTSXE1B = true;
    else if (M_StringCompare(leaf, "btsx_e2a.wad"))
        BTSX = BTSXE2 = BTSXE2A = true;
    else if (M_StringCompare(leaf, "btsx_e2b.wad"))
        BTSX = BTSXE2 = BTSXE2B = true;
    else if (M_StringCompare(leaf, "btsx_e3a.wad"))
        BTSX = BTSXE3 = BTSXE3A = true;
    else if (M_StringCompare(leaf, "btsx_e3b.wad"))
        BTSX = BTSXE3 = BTSXE3B = true;
    else if (M_StringCompare(leaf, "btsxe3pr.wad"))
        BTSX = BTSXE3 = true;
    else if (M_StringCompare(leaf, "e1m4b.wad"))
        E1M4B = true;
    else if (M_StringCompare(leaf, "e1m8b.wad"))
        E1M8B = true;
    else if (M_StringCompare(leaf, "iddm1.wad"))
        IDDM1 = true;
    else if (M_StringCompare(leaf, "KDiKDi_A.wad"))
        KDIKDIZD = KDIKDIZDA = true;
    else if (M_StringCompare(leaf, "KDiKDi_B.wad"))
        KDIKDIZD = KDIKDIZDB = true;
    else if (M_StringCompare(leaf, "one-humanity.wad"))
        onehumanity = true;
    else if (M_StringCompare(leaf, "d1spfx18.wad")
        || M_StringCompare(leaf, "d2spfx18.wad"))
        sprfix18 = true;
    else if (M_StringStartsWith(leaf, "Eviternity"))
        eviternity = true;
    else if (M_StringCompare(leaf, "d4v.wad"))
        doom4vanilla = true;
    else if (M_StringCompare(leaf, "REKKR.wad"))
        REKKR = true;
    else if (M_StringCompare(leaf, "rekkrsa.wad"))
        REKKR = REKKRSA = true;
    else if (M_StringCompare(leaf, "REKKRSL.wad")
        || M_StringCompare(leaf, "REKKRSL.iwad"))
        REKKR = REKKRSL = true;
    else if (M_StringCompare(leaf, "ar.wad"))
        anomalyreport = true;
    else if (M_StringCompare(leaf, "arrival.wad"))
        arrival = true;
    else if (M_StringCompare(leaf, "dbimpact.wad"))
        dbimpact = true;
    else if (M_StringCompare(leaf, "deathless.wad"))
        deathless = true;
    else if (M_StringCompare(leaf, "DoomZero.wad"))
        doomzero = true;
    else if (M_StringCompare(leaf, "earthless_pr.wad"))
        earthless = true;
    else if (M_StringCompare(leaf, "BGComp.wad"))
        ganymede = true;
    else if (M_StringCompare(leaf, "gd.wad"))
        goingdown = true;
    else if (M_StringCompare(leaf, "gdturbo.wad"))
        goingdownturbo = true;
    else if (M_StringCompare(leaf, "HarmonyC.wad"))
        harmonyc = true;
    else if (M_StringCompare(leaf, "ID1.wad"))
    {
        legacyofrust = true;
        moreblood = true;
    }
    else if (M_StringCompare(leaf, "masterlevels.wad"))
        masterlevels = true;
    else if (M_StringCompare(leaf, "neis.wad"))
        neis = true;
    else if (M_StringCompare(leaf, "TVR!.wad"))
        revolution = true;
    else if (M_StringCompare(leaf, "SCI.wad")
        || M_StringCompare(leaf, "SCI2.wad")
        || M_StringCompare(leaf, "sci-c.wad")
        || M_StringCompare(leaf, "sci2023.wad"))
        scientist = true;
    else if (M_StringCompare(leaf, "SD21.wad"))
    {
        SD21 = true;
        moreblood = true;
        fixspriteoffsets = true;
    }
    else if (M_StringCompare(leaf, "syringe.wad"))
        syringe = true;
    else if (M_StringCompare(leaf, "TTNS.wad") || M_StringCompare(leaf, "TTNSDX.wad"))
        TTNS = true;
    else if (M_StringCompare(leaf, "TTP.wad"))
        TTP = true;

    if (BTSX || REKKR)
        moreblood = true;
}

static bool D_IsUnsupportedPWAD(char *filename)
{
    return ((error = D_IsResourceWAD(filename)));
}

static void D_AutoloadExtrasWAD(void)
{
    char    path[MAX_PATH];

    if (M_CheckParm("-noautoload") || M_CheckParm("-nomusic") || M_CheckParm("-nosound"))
        return;

    M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "extras.wad");

    if (W_MergeFile(path, true))
        extras = true;
}

static void D_AutoloadSIGILWAD(void)
{
    bool    shreds = false;
    char    path[MAX_PATH];

    D_AutoloadExtrasWAD();

    if (sigil || sigil2 || M_CheckParm("-noautoload"))
        return;

    M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_V1_23_REG.wad");

    if (W_MergeFile(path, true))
    {
        sigil = true;
        shreds = true;
    }
    else
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_V1_23.wad");

        if (W_MergeFile(path, true))
            sigil = true;
        else
        {
            M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_v1_21.wad");

            if (W_MergeFile(path, true))
                sigil = true;
            else
            {
                M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_v1_2.wad");

                if (W_MergeFile(path, true))
                    sigil = true;
                else
                {
                    M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_v1_1.wad");

                    if (W_MergeFile(path, true))
                        sigil = true;
                    else
                    {
                        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_v1_0.wad");

                        if (W_MergeFile(path, true))
                            sigil = true;
                        else
                        {
                            M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL.wad");

                            if (W_MergeFile(path, true))
                                sigil = true;
                        }
                    }
                }
            }
        }
    }

    if (sigil && !shreds && !M_CheckParm("-nomusic") && !M_CheckParm("-nosound"))
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_SHREDS.wad");

        if (!W_MergeFile(path, true))
        {
            M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_SHREDS_COMPAT.wad");
            W_MergeFile(path, true);
        }
    }
}

static void D_AutoloadSIGIL2WAD(void)
{
    char    path[MAX_PATH];

    D_AutoloadExtrasWAD();

    if (!autosigil || M_CheckParm("-noautoload"))
        return;

    M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_II_MP3_V1_0.WAD");

    if (W_MergeFile(path, true))
        sigil2 = true;
    else
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL_II_V1_0.WAD");
        W_MergeFile(path, true);

        if (W_MergeFile(path, true))
            sigil = true;
        else
        {
            M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "SIGIL2.WAD");
            W_MergeFile(path, true);

            if (W_MergeFile(path, true))
                sigil = true;
        }
    }
}

static void D_AutoloadNerveWAD(void)
{
    char    path[MAX_PATH];

    D_AutoloadExtrasWAD();

    if (M_CheckParm("-noautoload"))
        return;

    M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "NERVE.WAD");

    if (W_MergeFile(path, true))
        nerve = true;
}

static bool D_AutoloadOtherBTSXWAD(void)
{
    char    path[MAX_PATH];

    if (BTSXE1A && !BTSXE1B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e1b.wad");
        return W_MergeFile(path, true);
    }
    else if (!BTSXE1A && BTSXE1B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e1a.wad");
        pwadfile = M_StringDuplicate("btsx_e1a.wad");
        return W_MergeFile(path, true);
    }
    else if (BTSXE2A && !BTSXE2B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e2b.wad");
        return W_MergeFile(path, true);
    }
    else if (!BTSXE2A && BTSXE2B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e2a.wad");
        pwadfile = M_StringDuplicate("btsx_e2a.wad");
        return W_MergeFile(path, true);
    }
    else if (BTSXE3A && !BTSXE3B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e3b.wad");
        return W_MergeFile(path, true);
    }
    else if (!BTSXE3A && BTSXE3B)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "btsx_e3a.wad");
        pwadfile = M_StringDuplicate("btsx_e3a.wad");
        return W_MergeFile(path, true);
    }

    return false;
}

static bool D_AutoloadOtherKDIKDIZDWAD(void)
{
    char    path[MAX_PATH];

    if (KDIKDIZDA && !KDIKDIZDB)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "KDiKDi_B.wad");
        return W_MergeFile(path, true);
    }
    else if (!KDIKDIZDA && KDIKDIZDB)
    {
        M_snprintf(path, sizeof(path), "%s" DIR_SEPARATOR_S "%s", wadfolder, "KDiKDi_A.wad");
        pwadfile = M_StringDuplicate("KDiKDi_A.wad");
        return W_MergeFile(path, true);
    }

    return false;
}

static bool D_CheckParms(void)
{
    bool    result = false;

    if (myargc == 2 && D_IsWADFile(myargv[1]))
    {
        char    *folder = M_ExtractFolder(myargv[1]);

        // check if it's a valid and supported IWAD
        if (D_IsDOOMIWAD(myargv[1]) || (W_WadType(myargv[1]) == IWAD && !D_IsUnsupportedIWAD(myargv[1])))
        {
            D_IdentifyIWADByName(myargv[1]);

            if (W_AddFile(myargv[1], false))
            {
                result = true;
                wadfolder = M_StringDuplicate(folder);

                // if DOOM.WAD is selected, load SIGIL.WAD automatically if present
                if (D_IsDOOM1IWAD(myargv[1]) && IsUltimateDOOM(myargv[1]))
                {
                    D_AutoloadSIGILWAD();
                    D_AutoloadSIGIL2WAD();
                }
                // if DOOM2.WAD is selected, load NERVE.WAD automatically if present
                else if (D_IsDOOM2IWAD(myargv[1]))
                    D_AutoloadNerveWAD();
            }
        }

        // if it's a PWAD, determine the IWAD required and try loading that as well
        else if (W_WadType(myargv[1]) == PWAD && !D_IsUnsupportedPWAD(myargv[1]))
        {
            gamemission_t   iwadrequired = IWADRequiredByPWAD(myargv[1]);
            char            fullpath[MAX_PATH];

            if (iwadrequired == none)
                iwadrequired = doom2;

            // try the current folder first
            M_snprintf(fullpath, sizeof(fullpath), "%s" DIR_SEPARATOR_S "%s", folder,
                (M_StringCompare(leafname(myargv[1]), "chex2.wad") ? "chex.wad" : iwadsrequired[iwadrequired]));
            D_IdentifyIWADByName(fullpath);

            if (W_AddFile(fullpath, true))
            {
                result = true;
                wadfolder = M_StringDuplicate(folder);
                D_CheckSupportedPWAD(myargv[1]);

                if (D_IsSIGIL2WAD(myargv[1]))
                    D_AutoloadSIGILWAD();

                if (W_MergeFile(myargv[1], false))
                {
                    modifiedgame = true;

                    if (legacyofrust)
                        D_AutoloadExtrasWAD();

                    if (IWADRequiredByPWAD(myargv[1]) != none)
                        pwadfile = M_StringDuplicate(leafname(myargv[1]));

                    if (!M_CheckParm("-nodeh") && !M_CheckParm("-nobex") && !D_IsDEHFile(myargv[1]))
                        LoadDEHFile(myargv[1], true);
                }
            }
            else
            {
                // otherwise try the wadfolder CVAR
                M_snprintf(fullpath, sizeof(fullpath), "%s" DIR_SEPARATOR_S "%s", wadfolder,
                    (M_StringCompare(leafname(myargv[1]), "chex2.wad") ? "chex.wad" : iwadsrequired[iwadrequired]));

                D_IdentifyIWADByName(fullpath);

                if (W_AddFile(fullpath, true))
                {
                    result = true;
                    D_CheckSupportedPWAD(myargv[1]);

                    if (D_IsSIGIL2WAD(myargv[1]))
                        D_AutoloadSIGILWAD();

                    if (W_MergeFile(myargv[1], false))
                    {
                        modifiedgame = true;

                        if (legacyofrust)
                            D_AutoloadExtrasWAD();

                        if (IWADRequiredByPWAD(myargv[1]) != none)
                            pwadfile = M_StringDuplicate(leafname(myargv[1]));


                        if (!M_CheckParm("-nodeh") && !M_CheckParm("-nobex") && !D_IsDEHFile(myargv[1]))
                            LoadDEHFile(myargv[1], true);
                    }
                }
                else
                {
                    // still nothing? try some common installation folders
                    if (W_AddFile(D_FindWADByName((M_StringCompare(leafname(myargv[1]), "chex2.wad") ?
                        "chex.wad" : iwadsrequired[iwadrequired])), true))
                    {
                        result = true;
                        D_CheckSupportedPWAD(myargv[1]);

                        if (D_IsSIGIL2WAD(myargv[1]))
                            D_AutoloadSIGILWAD();

                        if (W_MergeFile(myargv[1], false))
                        {
                            modifiedgame = true;

                            if (legacyofrust)
                                D_AutoloadExtrasWAD();

                            if (IWADRequiredByPWAD(myargv[1]) != none)
                                pwadfile = M_StringDuplicate(leafname(myargv[1]));


                            if (!M_CheckParm("-nodeh") && !M_CheckParm("-nobex") && !D_IsDEHFile(myargv[1]))
                                LoadDEHFile(myargv[1], true);
                        }
                    }
                }
            }
        }

        if (BTSX)
            D_AutoloadOtherBTSXWAD();
        else if (KDIKDIZD)
            D_AutoloadOtherKDIKDIZDWAD();

        free(folder);
    }

    return result;
}

static char *invalidwad;

static int D_OpenWADLauncher(void)
{
    int             iwadfound = -1;
    bool            fileopenedok;

    char            szFile[4096];

    M_StringCopy(szFile, (invalidwad ? invalidwad : wad), sizeof(szFile));

    fileopenedok = true;

    if (fileopenedok)
    {
        bool    onlyoneselected;
        bool    guess = false;


        iwadfound = 0;

        // only one file was selected
        if (wad)
            previouswad = M_StringDuplicate(wad);

        wad = "";

        onlyoneselected = true;;

        if (onlyoneselected)
        {
            char    *file = (char *)FullDoomFileTempPath;
            char    *folder = M_ExtractFolder(file);

            wad = M_StringDuplicate(leafname(file));

            // check if it's a valid and supported IWAD
            if (D_IsDOOMIWAD(file) || (W_WadType(file) == IWAD && !D_IsUnsupportedIWAD(file)))
            {
                D_IdentifyIWADByName(file);

                if (W_AddFile(file, false))
                {
                    iwadfound = 1;
                    wadfolder = M_StringDuplicate(folder);

                    // if DOOM.WAD is selected, load SIGIL.WAD automatically if present
                    if (D_IsDOOM1IWAD(file) && IsUltimateDOOM(file))
                    {
                        D_AutoloadSIGILWAD();
                        D_AutoloadSIGIL2WAD();
                    }
                    // if DOOM2.WAD is selected, load NERVE.WAD automatically if present
                    else if (D_IsDOOM2IWAD(file))
                        D_AutoloadNerveWAD();
                }
            }

            if (BTSX)
                D_AutoloadOtherBTSXWAD();
            else if (KDIKDIZD)
                D_AutoloadOtherKDIKDIZDWAD();

            free(folder);
        }
    }

    return iwadfound;
}

static void D_ProcessDehOnCmdLine(void)
{
    int p = M_CheckParm("-deh");

    if (p || (p = M_CheckParm("-bex")))
    {
        bool    deh = true;

        while (++p < myargc)
            if (*myargv[p] == '-')
                deh = (M_StringCompare(myargv[p], "-deh") || M_StringCompare(myargv[p], "-bex"));
            else if (deh)
                D_ProcessDehFile(myargv[p], 0, false);
    }
}

static void D_ProcessDehInWad(void)
{
    if (*dehwarning)
        C_Warning(1, dehwarning);

    if (chex1)
        D_ProcessDehFile(NULL, W_GetNumForName("CHEXBEX"), true);

    if (!M_CheckParm("-nodeh") && !M_CheckParm("-nobex"))
        for (int i = 0; i < numlumps; i++)
            if (M_StringCompare(lumpinfo[i]->name, "DEHACKED")
                && !D_IsResourceWAD(lumpinfo[i]->wadfile->path))
                D_ProcessDehFile(NULL, i, false);

    for (int i = numlumps - 1; i >= 0; i--)
        if (M_StringCompare(lumpinfo[i]->name, "DEHACKED")
            && D_IsResourceWAD(lumpinfo[i]->wadfile->path))
        {
            D_ProcessDehFile(NULL, i, false);
            break;
        }
}
//TTF_Font* MyFont;
//
// D_DoomMainSetup
//
// CPhipps - the old contents of D_DoomMain, but moved out of the main
//  line of execution so its stack space can be freed
static void D_DoomMainSetup(void)
{
    int     p = 0;
    int     choseniwad = 0;
    bool    autoloading = false;
    char    lumpname[6];
    char    *appdatafolder = M_GetAppDataFolder();
    char    *iwadfile;
    int     startloadgame;
    char    *resourcefolder = M_GetResourceFolder();
    
    //resourcewad = M_StringJoin(resourcefolder, DIR_SEPARATOR_S, DOOMRETRO_RESOURCEWAD, NULL);
    resourcewad = FullDoomRetroFileTempPath;
    free(resourcefolder);

    C_ClearConsole();

    dsdh_InitTables();
    D_BuildBEXTables();

    C_PrintCompileDate();
    I_PrintWindowsVersion();

    I_PrintSystemInfo();
    C_PrintSDLVersions();
    I_SetPriority(true);

    SDL_Init(SDL_INIT_EVERYTHING);

    iwadfile = D_FindIWAD();

    for (int i = 0; i < MAXALIASES; i++)
    {
        aliases[i].name[0] = '\0';
        aliases[i].string[0] = '\0';
    }

    if (M_StringCompare(wadfolder, wadfolder_default) || !M_FolderExists(wadfolder))
        D_InitWADfolder();

    if ((respawnmonsters = M_CheckParm("-respawn")))
        C_Output("A " BOLD("-respawn") " parameter was found on the command-line. "
            "Monsters will now respawn.");
    else if ((respawnmonsters = M_CheckParm("-respawnmonsters")))
        C_Output("A " BOLD("-respawnmonsters") " parameter was found on the command-line. "
            "Monsters will now respawn.");

    if ((nomonsters = M_CheckParm("-nomonsters")))
    {
        C_Output("A " BOLD("-nomonsters") " parameter was found on the command-line. "
            "No monsters will now be spawned.");
        stat_cheatsentered = SafeAdd(stat_cheatsentered, 1);
        
    }

    if ((pistolstart = M_CheckParm("-pistolstart")))
        C_Output("A " BOLD("-pistolstart") " parameter was found on the command-line. "
            "The player will now start each map with 100%% health, no armor, "
            "and only a pistol with 50 bullets.");

    if ((fastparm = M_CheckParm("-fast")))
        C_Output("A " BOLD("-fast") " parameter was found on the command-line. "
            "Monsters will now be fast.");
    else if ((fastparm = M_CheckParm("-fastmonsters")))
        C_Output("A " BOLD("-fastmonsters") " parameter was found on the command-line. "
            "Monsters will now be fast.");

    if ((solonet = M_CheckParm("-solonet")))
        C_Output("A " BOLD("-solonet") " parameter was found on the command-line. "
            "Things usually intended for multiplayer will now spawn at the start of each map, "
            "and the player will respawn without the map restarting if they die.");
    else if ((solonet = M_CheckParm("-solo-net")))
        C_Output("A " BOLD("-solo-net") " parameter was found on the command-line. "
            "Things usually intended for multiplayer will now spawn at the start of each map, "
            "and the player will respawn without the map restarting if they die.");

    if ((devparm = M_CheckParm("-devparm")))
        C_Output("A " BOLD("-devparm") " parameter was found on the command-line. %s", s_D_DEVSTR);

    // turbo option
    if ((p = M_CheckParm("-turbo")))
    {
        int scale = turbo_default * 2;

        if (p < myargc - 1)
        {
            scale = strtol(myargv[p + 1], NULL, 10);

            if (scale < turbo_min || scale > turbo_max)
                scale = turbo_default * 2;
        }

        C_Output("A " BOLD("-turbo") " parameter was found on the command-line. "
            "The player will now be %i%% their normal speed.", scale);

        if (scale != turbo_default)
            G_SetMovementSpeed(scale);

        if (scale > turbo_default)
        {
            stat_cheatsentered = SafeAdd(stat_cheatsentered, 1);
            
        }
    }
    else
        G_SetMovementSpeed(turbo);

    // init subsystems
    V_Init();

    if (!stat_runs)
    {
        const time_t    now = time(NULL);
        struct tm       *currenttime = localtime(&now);

        stat_firstrun = (uint64_t)currenttime->tm_mday + ((uint64_t)currenttime->tm_mon + 1) * 100
            + ((uint64_t)currenttime->tm_year + 1900) * 10000;

        C_Output("This is the first time " ITALICS(DOOMRETRO_NAME) " has been run on this " DEVICE ".");
    }
    else
    {
        char    *temp = commify(SafeAdd(stat_runs, 1));

        if (stat_firstrun)
        {
            const int   day = stat_firstrun % 100;
            const int   month = (stat_firstrun % 10000) / 100;
            const int   year = (int)stat_firstrun / 10000;

            C_Output(ITALICS(DOOMRETRO_NAME) " has been run %s times on this " DEVICE " since it was installed on %s, %s %i, %i.",
                temp, dayofweek(day, month, year), monthnames[month - 1], day, year);
        }
        else
            C_Output(ITALICS(DOOMRETRO_NAME) " has been run %s times on this " DEVICE ".", temp);

        free(temp);
    }

    if (!M_FileExists(resourcewad))
        I_Error("%s can't be found.", resourcewad);

    if (M_CheckParm("-nodeh"))
        C_Output("A " BOLD("-nodeh") " parameter was found on the command-line. "
            "All " BOLD("DEHACKED") " lumps will now be ignored.");
    else if (M_CheckParm("-nobex"))
        C_Output("A " BOLD("-nobex") " parameter was found on the command-line. "
            "All " BOLD("DEHACKED") " lumps will now be ignored.");

    p = M_CheckParmsWithArgs("-file", "-pwad", "-merge", 1);

    if (!(choseniwad = D_CheckParms()))
    {
        if (iwadfile)
        {
            if (W_AddFile(iwadfile, false))
                stat_runs = SafeAdd(stat_runs, 1);
        }
        else if (!p)
        {
            do
            {
                if ((choseniwad = D_OpenWADLauncher()) == -1)
                    I_Quit(false);
                else if (!choseniwad && !error && (!*wad || D_IsWADFile(wad)))
                {
                    char    buffer[256];

                    M_snprintf(buffer, sizeof(buffer), DOOMRETRO_NAME " couldn't find %s.\n",
                        (*wad ? wad : "any IWADs"));

                    if (previouswad)
                        wad = M_StringDuplicate(previouswad);

                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, DOOMRETRO_NAME, buffer, NULL);
                }
            } while (!choseniwad);

            stat_runs = SafeAdd(stat_runs, 1);
        }
    }

    

    if (keyboardscreenshot == KEY_PRINTSCREEN || keyboardscreenshot2 == KEY_PRINTSCREEN)
    {
        RegisterHotKey(NULL, 1, MOD_ALT, VK_SNAPSHOT);
        RegisterHotKey(NULL, 2, 0, VK_SNAPSHOT);
    }

    if (p > 0)
        do
        {
            for (p++; p < myargc && myargv[p][0] != '-'; p++)
            {
                char    *file = D_TryFindWADByName(myargv[p]);

                if (iwadfile)
                {
                    D_CheckSupportedPWAD(file);

                    if (D_IsSIGIL2WAD(file))
                        D_AutoloadSIGILWAD();

                    if (W_MergeFile(file, false))
                    {
                        modifiedgame = true;

                        if (legacyofrust)
                            D_AutoloadExtrasWAD();

                        if (IWADRequiredByPWAD(file) != none)
                            pwadfile = M_StringDuplicate(leafname(file));
                    }
                }
                else
                {
                    gamemission_t   iwadrequired = IWADRequiredByPWAD(file);
                    char            fullpath[MAX_PATH];
                    char            *folder = M_ExtractFolder(file);

                    if (iwadrequired == none)
                        iwadrequired = doom2;

                    // try the current folder first
                    M_snprintf(fullpath, sizeof(fullpath), "%s" DIR_SEPARATOR_S "%s",
                        folder, iwadsrequired[iwadrequired]);
                    D_IdentifyIWADByName(fullpath);

                    if (W_AddFile(fullpath, true))
                    {
                        iwadfile = M_StringDuplicate(fullpath);
                        wadfolder = M_StringDuplicate(folder);
                        D_CheckSupportedPWAD(file);

                        if (D_IsSIGIL2WAD(file))
                            D_AutoloadSIGILWAD();

                        if (W_MergeFile(file, false))
                        {
                            modifiedgame = true;

                            if (legacyofrust)
                                D_AutoloadExtrasWAD();

                            if (IWADRequiredByPWAD(file) != none)
                                pwadfile = M_StringDuplicate(leafname(file));
                        }
                    }
                    else
                    {
                        // otherwise try the wadfolder CVAR
                        M_snprintf(fullpath, sizeof(fullpath), "%s" DIR_SEPARATOR_S "%s",
                            wadfolder, iwadsrequired[iwadrequired]);
                        D_IdentifyIWADByName(fullpath);

                        if (W_AddFile(fullpath, true))
                        {
                            iwadfile = M_StringDuplicate(fullpath);
                            D_CheckSupportedPWAD(file);

                            if (D_IsSIGIL2WAD(file))
                                D_AutoloadSIGILWAD();

                            if (W_MergeFile(file, false))
                            {
                                modifiedgame = true;

                                if (legacyofrust)
                                    D_AutoloadExtrasWAD();

                                if (IWADRequiredByPWAD(file) != none)
                                    pwadfile = M_StringDuplicate(leafname(file));
                            }
                        }
                        else
                        {
                            // still nothing? try some common installation folders
                            if (W_AddFile(D_FindWADByName(iwadsrequired[iwadrequired]), true))
                            {
                                iwadfile = M_StringDuplicate(fullpath);
                                D_CheckSupportedPWAD(file);

                                if (D_IsSIGIL2WAD(file))
                                    D_AutoloadSIGILWAD();

                                if (W_MergeFile(file, false))
                                {
                                    modifiedgame = true;

                                    if (legacyofrust)
                                        D_AutoloadExtrasWAD();

                                    if (IWADRequiredByPWAD(file) != none)
                                        pwadfile = M_StringDuplicate(leafname(file));
                                }
                            }
                        }
                    }

                    free(folder);
                }
            }
        } while ((p = M_CheckParmsWithArgs("-file", "-pwad", "-merge", p)));

    if (!iwadfile && !modifiedgame && !choseniwad)
        I_Error(DOOMRETRO_NAME " couldn't find any IWADs.");

    W_Init();
    D_IdentifyVersion();

    if (!M_CheckParm("-noautoload") && gamemode != shareware)
    {
        D_SetAutoloadFolder();

        if (gamemission == doom)
        {
            bool    nosigil = false;

            if (W_GetNumLumps("M_DOOM") > 2
                || W_GetNumLumps("E1M1") > 1
                || !W_GetNumLumps("E4M1")
                || W_GetNumLumps("M_EPI5")
                || W_GetNumLumps("E5M1"))
                nosigil = true;
            else
            {
                autoloading = W_AutoloadFile("SIGIL_V1_23_REG.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL_V1_23.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL_v1_21.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL_v1_2.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL_v1_1.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL_v1_0.wad", autoloadfolder, false);
                autoloading |= W_AutoloadFile("SIGIL.wad", autoloadfolder, false);

                if (!autoloading && !REKKRSL)
                {
                    autoloading = W_AutoloadFile("SIGIL_V1_23_REG.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL_V1_23.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL_v1_21.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL_v1_2.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL_v1_1.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL_v1_0.wad", autoloadiwadsubfolder, false);
                    autoloading |= W_AutoloadFile("SIGIL.wad", autoloadiwadsubfolder, false);
                }

                if (autoloading)
                {
                    bool    autoloading2 = false;

                    autoloading2 = W_AutoloadFile("SIGIL_II_MP3_V1_0.WAD", autoloadfolder, false);
                    autoloading2 |= W_AutoloadFile("SIGIL_II_V1_0.WAD", autoloadfolder, false);
                    autoloading2 |= W_AutoloadFile("SIGIL2.WAD", autoloadfolder, false);

                    if (!autoloading2)
                    {
                        autoloading2 = W_AutoloadFile("SIGIL_II_MP3_V1_0.WAD", autoloadiwadsubfolder, false);
                        autoloading2 |= W_AutoloadFile("SIGIL_II_V1_0.WAD", autoloadiwadsubfolder, false);
                        autoloading2 |= W_AutoloadFile("SIGIL2.WAD", autoloadiwadsubfolder, false);
                    }

                    autoloading |= autoloading2;
                }
            }

            autoloading |= W_AutoloadFiles(autoloadfolder, nosigil);
            autoloading |= W_AutoloadFiles(autoloadiwadsubfolder, nosigil);

            if (sigil && autoloadsigilsubfolder)
            {
                autoloadsigilsubfolder = M_StringJoin(autoloadfolder, autoloadsigilsubfolder, DIR_SEPARATOR_S, NULL);
                autoloading |= W_AutoloadFiles(autoloadsigilsubfolder, false);
            }

            if (sigil2 && autoloadsigil2subfolder)
            {
                autoloadsigil2subfolder = M_StringJoin(autoloadfolder, autoloadsigil2subfolder, DIR_SEPARATOR_S, NULL);
                autoloading |= W_AutoloadFiles(autoloadsigil2subfolder, false);
            }
        }
        else
        {
            bool    nonerve = false;

            if (gamemission == doom2)
            {
                if (W_GetNumLumps("M_DOOM") > 2 || W_GetNumLumps("MAP01") > 1)
                    nonerve = true;
                else
                    autoloading = W_AutoloadFile("NERVE.WAD", autoloadiwadsubfolder, false);
            }

            autoloading |= W_AutoloadFiles(autoloadfolder, nonerve);
            autoloading |= W_AutoloadFiles(autoloadiwadsubfolder, nonerve);

            if (nerve && autoloadnervesubfolder)
            {
                autoloadnervesubfolder = M_StringJoin(autoloadfolder, autoloadnervesubfolder, DIR_SEPARATOR_S, NULL);
                autoloading |= W_AutoloadFiles(autoloadnervesubfolder, false);
            }
        }

        if (autoloadpwadsubfolder)
            autoloading |= W_AutoloadFiles(autoloadpwadsubfolder, false);

        if (autoloading)
            W_Init();
    }

    W_CheckForPNGLumps();

    FREEDM = (W_CheckNumForName("FREEDM") >= 0);

    PLAYPALs = (FREEDOOM || chex || hacx || harmony || REKKRSA ? 2 : W_GetNumLumps("PLAYPAL"));
    STBARs = W_GetNumLumps("STBAR");

    DBIGFONT = (W_CheckNumForName("DBIGFONT") >= 0);
    DSFLAMST = (W_GetNumLumps("DSFLAMST") > 1);
    E1M4 = (W_GetNumLumps("E1M4") > 1);
    E1M8 = (W_GetNumLumps("E1M8") > 1);
    M_DOOM = (W_GetNumLumps("M_DOOM") > 2 && !nerve);
    M_EPISOD = (W_GetNumLumps("M_EPISOD") > 1);
    M_GDHIGH = (W_GetNumLumps("M_GDHIGH") > 1);
    M_GDLOW = (W_GetNumLumps("M_GDLOW") > 1);
    M_LOADG = (W_GetNumLumps("M_LOADG") > 1);
    M_LGTTL = (W_GetNumLumps("M_LGTTL") > 1);
    M_LSCNTR = (W_GetNumLumps("M_LSCNTR") > 1);
    M_MSENS = (W_GetNumLumps("M_MSENS") > 1);
    M_MSGOFF = (W_GetNumLumps("M_MSGOFF") > 1);
    M_MSGON = (W_GetNumLumps("M_MSGON") > 1);
    M_NEWG = (W_GetNumLumps("M_NEWG") > 1);
    M_NGAME = (W_GetNumLumps("M_NGAME") > 1);
    M_NMARE = (W_GetNumLumps("M_NMARE") > 1);
    M_OPTTTL = (W_GetNumLumps("M_OPTTTL") > 1);
    M_PAUSE = (W_GetNumLumps("M_PAUSE") > 1);
    M_SAVEG = (W_GetNumLumps("M_SAVEG") > 1);
    M_SGTTL = (W_GetNumLumps("M_SGTTL") > 1);
    M_SKILL = (W_GetNumLumps("M_SKILL") > 1);
    M_SKULL1 = (W_GetNumLumps("M_SKULL1") > 1);
    M_SVOL = (W_GetNumLumps("M_SVOL") > 1);
    STYSNUM0 = (W_GetNumLumps("STYSNUM0") > 1);
    WICOLON = (W_GetNumLumps("WICOLON") > 1);
    WIPERIOD = (W_GetNumLumps("WIPERIOD") > 1);
    WISCRT2 = (W_GetNumLumps("WISCRT2") > 1);

    I_InitGraphics();
    I_InitController();

    D_ProcessDehOnCmdLine();
    D_ProcessDehInWad();
    D_PostProcessDeh();
    D_TranslateDehStrings();
    D_SetGameDescription();

    if (dehcount > 2)
    {
        if (gamemode == shareware)
        {
            free(appdatafolder);
            I_Error("Other files can’t be loaded with the shareware version of DOOM.");
        }

        C_Warning(0, "Loading multiple " BOLD("DEHACKED") " lumps or files may cause unexpected results.");
    }

    if (!autoloading)
    {
        if (autoloadpwadsubfolder)
            C_Output("Any " BOLD(".wad") ", " BOLD(".deh") " or " BOLD(".cfg") " files in "
                BOLD("%s") ", " BOLD("%s") " or " BOLD("%s") " will be automatically loaded.",
                autoloadfolder, autoloadiwadsubfolder, autoloadpwadsubfolder);
        else
            C_Output("Any " BOLD(".wad") ", " BOLD(".deh") " or " BOLD(".cfg") " files in "
                BOLD("%s") " or " BOLD("%s") " will be automatically loaded.",
                autoloadfolder, autoloadiwadsubfolder);
    }

    if (!M_StringCompare(s_VERSION, DOOMRETRO_NAMEANDVERSIONSTRING))
    {
        free(appdatafolder);
        I_Error("The wrong version of %s was found.", resourcewad);
    }

    unity = (W_CheckNumForName("TITLEPIC") >= 0
        && SHORT(((patch_t *)W_CacheLastLumpName("TITLEPIC"))->width) > VANILLAWIDTH
        && D_IsDOOMIWAD(lumpinfo[W_GetLastNumForName("TITLEPIC")]->wadfile->path));

    kex = (unity && W_CheckNumForName("GAMECONF") >= 0
        && D_IsDOOMIWAD(lumpinfo[W_GetLastNumForName("GAMECONF")]->wadfile->path));

    if (nerve && expansion == 2)
        gamemission = pack_nerve;

    FREEDOOM1 = (FREEDOOM && gamemission == doom);

    D_SetSaveGameFolder(true);

    D_SetScreenshotsFolder();

    C_Output("Files created using the " BOLD("condump") " CCMD are placed in "
        BOLD("%s" DIR_SEPARATOR_S DOOMRETRO_CONSOLEFOLDER DIR_SEPARATOR_S) ".", appdatafolder);

    free(appdatafolder);

    // Check for -file in shareware
    if (modifiedgame)
    {
        if (gamemode == shareware)
            I_Error("Other files can’t be loaded with the shareware version of DOOM.");

        // Check for fake IWAD with right name,
        // but w/o all the lumps of the registered version.
        if (gamemode == registered)
        {
            // These are the lumps that will be checked in IWAD,
            // if any one is not present, execution will be aborted.
            const char name[23][9] =
            {
                "E2M1", "E2M2", "E2M3", "E2M4", "E2M5", "E2M6", "E2M7", "E2M8", "E2M9",
                "E3M1", "E3M3", "E3M3", "E3M4", "E3M5", "E3M6", "E3M7", "E3M8", "E3M9",
                "DPHOOF", "BFGGA0", "HEADA1", "CYBRA1", "SPIDA1D1"
            };

            for (int i = 0; i < 23; i++)
                if (W_CheckNumForName(name[i]) < 0)
                    I_Error("This is not the registered version of DOOM.WAD.");
        }
    }

    // get skill/episode/map from parms
    startskill = sk_medium;
    startepisode = 1;
    startmap = 1;
    autostart = false;

    if ((p = M_CheckParmsWithArgs("-skill", "-skilllevel", "", 1)))
    {
        const int   temp = myargv[p + 1][0] - '1';

        if (temp >= sk_baby && temp <= sk_nightmare)
        {
            char    *string = titlecase(*skilllevels[temp]);

            startskill = (skill_t)temp;
            skilllevel = startskill + 1;
            

            M_StringReplaceAll(string, ".", "", false);
            M_StringReplaceAll(string, "!", "", false);

            C_Output("A " BOLD("%s") " parameter was found on the command-line. "
                "The skill level is now " ITALICS("%s."), myargv[p], string);
            free(string);
        }
    }

    if ((p = M_CheckParmWithArgs("-episode", 1)) && gamemode != commercial)
    {
        const int   temp = myargv[p + 1][0] - '0';

        if ((gamemode == shareware && temp == 1) || (temp >= 1 && ((gamemode == registered && temp <= 3)
            || (gamemode == retail && temp <= 4) || (sigil && temp <= 5) || (sigil2 && temp <= 6))))
        {
            startepisode = temp;
            episode = temp;
            
            M_snprintf(lumpname, sizeof(lumpname), "E%iM%i", startepisode, startmap);
            autostart = true;
            C_Output("An " BOLD("-episode") " parameter was found on the command-line. "
                "The episode is now " ITALICS("%s."), *episodes[episode - 1]);
        }
    }

    if ((p = M_CheckParmWithArgs("-expansion", 1)) && gamemode == commercial)
    {
        const int   temp = myargv[p + 1][0] - '0';

        if (temp <= (nerve ? 2 : 1))
        {
            gamemission = (temp == 1 ? doom2 : pack_nerve);
            expansion = temp;
            
            M_snprintf(lumpname, sizeof(lumpname), "MAP%02i", startmap);
            autostart = true;
            C_Output("An " BOLD("-expansion") " parameter was found on the command-line. "
                "The expansion is now " ITALICS("%s."), *expansions[expansion - 1]);
        }
    }

    if ((p = M_CheckParmsWithArgs("-warp", "+map", "", 1)))
    {
        if (gamemode == commercial)
        {
            if (strlen(myargv[p + 1]) == 5 && toupper(myargv[p + 1][0]) == 'M' && toupper(myargv[p + 1][1]) == 'A'
                && toupper(myargv[p + 1][2]) == 'P' && isdigit((int)myargv[p + 1][3]) && isdigit((int)myargv[p + 1][4]))
                startmap = (myargv[p + 1][3] - '0') * 10 + myargv[p + 1][4] - '0';
            else
                startmap = strtol(myargv[p + 1], NULL, 10);

            M_snprintf(lumpname, sizeof(lumpname), "MAP%02i", startmap);
        }
        else
        {
            if (strlen(myargv[p + 1]) == 4 && toupper(myargv[p + 1][0]) == 'E' && isdigit((int)myargv[p + 1][1])
                && toupper(myargv[p + 1][2]) == 'M' && isdigit((int)myargv[p + 1][3]))
            {
                startepisode = myargv[p + 1][1] - '0';
                startmap = myargv[p + 1][3] - '0';
            }
            else
            {
                startepisode = myargv[p + 1][0] - '0';

                if (p + 2 < myargc)
                    startmap = myargv[p + 2][0] - '0';
            }

            M_snprintf(lumpname, sizeof(lumpname), "E%iM%i", startepisode, startmap);
        }

        if ((BTSX && W_GetNumLumps(lumpname) > 1) || W_CheckNumForName(lumpname) >= 0)
        {
            autostart = true;

            if (startmap > 1)
            {
                stat_cheatsentered = SafeAdd(stat_cheatsentered, 1);
                
            }
        }
    }

    if (M_CheckParm("-dog"))
    {
        P_InitHelperDogs(1);

        C_Output("A " BOLD("-dog") " parameter was found on the command-line. "
            "A friendly dog will enter the game with %s.",
            (M_StringCompare(playername, playername_default) ? "you" : playername));
    }
    else if ((p = M_CheckParmWithArgs("-dogs", 1)))
    {
        const int   dogs = strtol(myargv[p + 1], NULL, 10);

        if (dogs == 1)
        {
            P_InitHelperDogs(1);

            C_Output("A " BOLD("-dogs") " parameter was found on the command-line. "
                "A friendly dog will enter the game with %s.",
                (M_StringCompare(playername, playername_default) ? "you" : playername));
        }
        else if (dogs > 1)
        {
            P_InitHelperDogs(MIN(dogs, MAXFRIENDS));

            C_Output("A " BOLD("-dogs") " parameter was found on the command-line. "
                "Up to %i friendly dogs will enter the game with %s.", MIN(dogs, MAXFRIENDS),
                (M_StringCompare(playername, playername_default) ? "you" : playername));
        }
    }
    else if (M_CheckParm("-dogs"))
    {
        P_InitHelperDogs(MAXFRIENDS);

        C_Output("A " BOLD("-dogs") " parameter was found on the command-line. "
            "Up to %i friendly dogs will enter the game with %s.", MAXFRIENDS,
            (M_StringCompare(playername, playername_default) ? "you" : playername));
    }

    M_Init();
    R_Init();
    P_Init();
    S_Init();
    HU_Init();
    ST_Init();
    AM_Init();
    C_Init();
    V_InitColorTranslation();

    if ((startloadgame = ((p = M_CheckParmWithArgs("-loadgame", 1)) ? strtol(myargv[p + 1], NULL, 10) : -1)) >= 0
        && startloadgame < savegame_max)
    {
        menuactive = false;
        I_InitKeyboard();

        if (alwaysrun)
            C_StringCVAROutput(stringize(alwaysrun), "on");

        G_LoadGame(P_SaveGameFile(startloadgame));
    }

    for (int i = 0; i < 18; i++)
    {
        char    buffer[9];

        M_snprintf(buffer, sizeof(buffer), "DRLOGO%02i", i + 1);
        logolump[i] = W_CacheLastLumpName(buffer);
    }

    logowidth = SHORT(logolump[0]->width);
    logoheight = SHORT(logolump[0]->height);
    logox = (SCREENWIDTH - logowidth) / 2;
    logoy = (SCREENHEIGHT - logoheight) / 2 + 1;

    fineprintlump = W_CacheLastLumpName("DRFNPRNT");
    fineprintwidth = SHORT(fineprintlump->width);
    fineprintheight = SHORT(fineprintlump->height);
    fineprintx = (SCREENWIDTH - fineprintwidth) / 2;
    fineprinty = SCREENHEIGHT - fineprintheight - 8;

    if (autosigil)
    {
        titlelump = W_CacheLastLumpName("TITLEPI1");
        creditlump = W_CacheLastLumpName("CREDIT2");
    }
    else if (REKKR)
    {
        titlelump = W_CacheLastLumpName(W_CheckNumForName("TITLEPIW") >= 0 ? "TITLEPIW" : "TITLEPIC");
        creditlump = W_CacheLastLumpName(W_CheckNumForName("CREDITW") >= 0 ? "CREDITW" : "CREDIT");
    }
    else
    {
        const int   titlepics = W_GetNumLumps("TITLEPIC");
        const int   credits = W_GetNumLumps("CREDIT");

        if (((titlepics == 1 && lumpinfo[W_GetNumForName("TITLEPIC")]->wadfile->type == PWAD)
            || titlepics > 1) && !nerve)
            titlelump = W_CacheLumpName("TITLEPIC");
        else
            switch (gamemission)
            {
                case doom:
                    titlelump = W_CacheLumpName("TITLEPI1");
                    break;

                case doom2:
                case pack_nerve:
                    titlelump = W_CacheLumpName("TITLEPI2");
                    break;

                case pack_plut:
                    titlelump = W_CacheLumpName("TITLEPIP");
                    break;

                case pack_tnt:
                    titlelump = W_CacheLumpName("TITLEPIT");
                    break;

                case none:
                    break;
            }

        if ((credits == 1 && lumpinfo[W_GetNumForName("CREDIT")]->wadfile->type == PWAD) || credits > 1)
            creditlump = W_CacheLumpName("CREDIT");
        else
            creditlump = W_CacheLumpName(gamemission == doom ? (gamemode == shareware ? "CREDIT1" : "CREDIT2") : "CREDIT3");
    }

    if (gameaction != ga_loadgame)
    {
        if (autostart)
        {
            menuactive = false;
            I_InitKeyboard();

            if (vid_widescreen_copy)
            {
                vid_widescreen_copy = false;
                vid_widescreen = true;
                I_RestartGraphics(false);
            }

            if (alwaysrun)
                C_StringCVAROutput(stringize(alwaysrun), "on");

            if (M_CheckParmWithArgs("-warp", 1))
                C_Output("A " BOLD("-warp") " parameter was found on the command-line. Warping %s to %s...",
                    (M_StringCompare(playername, playername_default) ? "you" : playername), lumpname);
            else if (M_CheckParmWithArgs("+map", 1))
                C_Output("A " BOLD("+map") " parameter was found on the command-line. Warping %s to %s...",
                    (M_StringCompare(playername, playername_default) ? "you" : playername), lumpname);
            else
                C_Output("Warping %s to %s...",
                    (M_StringCompare(playername, playername_default) ? "you" : playername), lumpname);

            G_DeferredInitNew(startskill, startepisode, startmap);
        }
        else
        {
            if (wad && previouswad && !M_StringCompare(wad, previouswad))
            {
                episode = episode_default;
                EpiDef.laston = episode - 1;
                expansion = expansion_default;
                ExpDef.laston = expansion - 1;
                
            }

            if ( true /*M_CheckParm("-nosplash")*/)
            {
                C_Warning(0, "A " BOLD("-nosplash") " parameter was found on the command-line. "
                    ITALICS(DOOMRETRO_NAME "'s") " splash screen wasn't displayed.");
                menuactive = false;
                D_StartTitle(1);
            }
            else
            {
                splashpal = W_CacheLastLumpName("SPLSHPAL");
                D_StartTitle(0);
            }
        }
    }
}



void HandleResourceWAD()
{
    HMODULE hModule = GetModuleHandle(NULL); // Get the handle to the current executable
    if (hModule == NULL)
        return;    

    HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDR_RCDATA2), RT_RCDATA);

    if (hResInfo == NULL)    
        return;
    
    DWORD size = SizeofResource(hModule, hResInfo);
    if (size == 0)
    
        return;

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData == NULL)    
        return;
    


    const void* pData = LockResource(hResData);
    if (!pData)
        return;

    char tempDir[MAX_PATH];
    DWORD len = GetTempPath(MAX_PATH, tempDir);

    if (len == 0 || len > MAX_PATH)
    {
        int x = 5;
        //return L"";
    }

    strcat(tempDir, "doomretro.wad");


    char tempFile[MAX_PATH] = { 0 };
    M_StringCopy(tempFile, tempDir, MAX_PATH);

    HANDLE hFile = CreateFile(
        tempFile,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        int x = 5;
        //return "";
    }

    if (GetLastError() != ERROR_ALREADY_EXISTS)
    {
        DWORD written = 0;
        BOOL ok = WriteFile(hFile, pData, size, &written, NULL);
        DWORD lastErr = ok ? ERROR_SUCCESS : GetLastError();
        CloseHandle(hFile);
        hFile = NULL;

        if (!ok || written != size)
        {
            // cleanup on failure
            DeleteFile(tempFile);
            SetLastError(lastErr);
            return;
        }
    }

    if (hFile)
        CloseHandle(hFile);

    M_StringCopy(FullDoomRetroFileTempPath, tempFile, MAX_PATH);
}

// Returns full path to the saved temp file (L"") on failure.
void SaveEmbeddedToTemp()
{
    HMODULE hModule = GetModuleHandle(NULL); // Get the handle to the current executable
    if (hModule == NULL)
    {
        return;
    }

    // 1. Find the resource
    HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDR_RCDATA1), RT_RCDATA);

    if (hResInfo == NULL)
    {
        return;
    }

    // 2. Get the size of the resource
    DWORD size = SizeofResource(hModule, hResInfo);
    if (size == 0)
    {
        return;
    }

    // 3. Load the resource
    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData == NULL)
    {
        return;
    }


    const void* pData = LockResource(hResData);
    if (!pData)
        return;

    // 2) Get the system temp directory
    char tempDir[MAX_PATH];
    DWORD len = GetTempPath(MAX_PATH, tempDir);

    if (len == 0 || len > MAX_PATH)
    {
        int x = 5;
        //return L"";
    }

    strcat(tempDir, "DOOM.WAD");


    char tempFile[MAX_PATH] = { 0 };
    M_StringCopy(tempFile, tempDir, MAX_PATH);

    HANDLE hFile = CreateFile(
        tempFile,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        int x = 5;
        //return "";
    }

    if (GetLastError() != ERROR_ALREADY_EXISTS)
    {

        DWORD written = 0;
        BOOL ok = WriteFile(hFile, pData, size, &written, NULL);
        DWORD lastErr = ok ? ERROR_SUCCESS : GetLastError();
        CloseHandle(hFile);
        hFile = NULL;

        if (!ok || written != size)
        {
            // cleanup on failure
            DeleteFile(tempFile);
            SetLastError(lastErr);
            return;
        }
    }

    if(hFile)
        CloseHandle(hFile);

    M_StringCopy(FullDoomFileTempPath, tempFile, MAX_PATH);
    // 5) Done — return the full path to the temp file
    //return std::wstring(tempFile);
}


void D_DoomMain(void)
{
	//Load game files from embedded resources
    HandleResourceWAD();
    SaveEmbeddedToTemp();

    D_DoomMainSetup();  // CPhipps - setup out of main execution stack
    D_DoomLoop();       // never returns
}
