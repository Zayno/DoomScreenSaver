

#pragma once

#include "doomdef.h"
#include "doomtype.h"
#include "r_defs.h"
#include "sounds.h"

#define BFGCELLS    40

//
// MBF21: haleyjd 09/11/07: weapon flags
//
enum
{
    WPF_NOFLAG         = 0x00000000,    // no flag
    WPF_NOTHRUST       = 0x00000001,    // doesn't thrust Mobj's
    WPF_SILENT         = 0x00000002,    // weapon is silent
    WPF_NOAUTOFIRE     = 0x00000004,    // weapon won't autofire in A_WeaponReady
    WPF_FLEEMELEE      = 0x00000008,    // monsters consider it a melee weapon
    WPF_AUTOSWITCHFROM = 0x00000010,    // can be switched away from when ammo is picked up
    WPF_NOAUTOSWITCHTO = 0x00000020     // cannot be switched to when ammo is picked up
};

// Weapon info: sprite frames, ammunition use.
typedef struct
{
    char            *name;
    spritenum_t     weaponsprite;
    spritenum_t     ammosprite;
    ammotype_t      ammotype;
    char            ammoname[255];
    char            ammoplural[255];
    int             ammopershot;
    int             upstate;
    int             downstate;
    int             readystate;
    int             atkstate;
    int             flashstate;
    int             recoil;
    int             lowrumble;
    int             highrumble;
    int             tics;
    weapontype_t    prevweapon;
    weapontype_t    nextweapon;
    sfxnum_t        sound;
    int             flags;  // MBF21
    char            key;
    bool            altered;
    patch_t         *weaponpatch;
    int             weapony;
    patch_t         *ammopatch;
} weaponinfo_t;

extern weaponinfo_t weaponinfo[NUMWEAPONS];
extern char         *powerups[NUMPOWERS];
