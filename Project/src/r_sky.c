

#include "c_cmds.h"
#include "doomstat.h"
#include "m_array.h"
#include "m_config.h"
#include "p_setup.h"
#include "r_sky.h"

//
// sky mapping
//
int         skyflatnum;
int         skytexture;
int         skytexturemid;
int         skycolumnoffset;
int         skyscrolldelta;

fixed_t     skyiscale;

bool        canfreelook = false;

sky_t       *sky = NULL;

// PSX fire sky <https://fabiensanglard.net/doom_fire_psx/>
static byte fireindices[FIREWIDTH * FIREHEIGHT];
static byte firepixels[FIREWIDTH * FIREHEIGHT];

static void PrepareFirePixels(fire_t *fire)
{
    byte    *rover = firepixels;

    for (int x = 0; x < FIREWIDTH; x++)
    {
        byte    *src = fireindices + x;

        for (int y = 0; y < FIREHEIGHT; y++)
        {
            *rover++ = fire->palette[*src];
            src += FIREWIDTH;
        }
    }
}

static void SpreadFire(void)
{
    for (int x = 0; x < FIREWIDTH; x++)
        for (int y = 1; y < FIREHEIGHT; y++)
        {
            const int   src = y * FIREWIDTH + x;
            const int   index = fireindices[src];

            if (!index)
                fireindices[src - FIREWIDTH] = 0;
            else
            {
                const int   r = (M_BigRandom() & 3);

                fireindices[src - r + 1 - FIREWIDTH] = index - (r & 1);
            }
        }
}

static void SetupFire(fire_t *fire)
{
    const int   last = array_size(fire->palette) - 1;

    memset(fireindices, 0, FIREWIDTH * FIREHEIGHT);

    for (int i = 0; i < FIREWIDTH; i++)
        fireindices[(FIREHEIGHT - 1) * FIREWIDTH + i] = last;

    for (int i = 0; i < 64; i++)
        SpreadFire();

    PrepareFirePixels(fire);
}

byte *R_GetFireColumn(int col)
{
    while (col < 0)
        col += FIREWIDTH;

    col %= FIREWIDTH;
    return &firepixels[col * FIREHEIGHT];
}

static void InitSkyDefs(void)
{
    static skydefs_t    *skydefs;
    static bool         runonce = true;
    flatmap_t           *flatmap = NULL;

    if (runonce)
    {
        skydefs = R_ParseSkyDefs();
        runonce = false;
    }

    if (!skydefs)
        return;

    array_foreach(flatmap, skydefs->flatmapping)
    {
        const int   flatnum = R_FlatNumForName(flatmap->flat);
        const int   skytex = R_TextureNumForName(flatmap->sky);

        for (int i = 0; i < numsectors; i++)
        {
            if (sectors[i].floorpic == flatnum)
            {
                sectors[i].floorpic = skyflatnum;
                sectors[i].floorsky = (skytex | PL_FLATMAPPING);
            }

            if (sectors[i].ceilingpic == flatnum)
            {
                sectors[i].ceilingpic = skyflatnum;
                sectors[i].ceilingsky = (skytex | PL_FLATMAPPING);
            }
        }
    }

    array_foreach(sky, skydefs->skies)
        if (skytexture == R_CheckTextureNumForName(sky->skytex.name))
        {
            if (sky->type == SkyType_Fire)
                SetupFire(&sky->fire);

            return;
        }

    sky = NULL;
}

void R_UpdateSky(void)
{
    skytex_t    *background;

    if (!sky)
        return;

    if (sky->type == SkyType_Fire)
    {
        fire_t  *fire = &sky->fire;

        if (!fire->ticsleft)
        {
            SpreadFire();
            PrepareFirePixels(fire);
            fire->ticsleft = fire->updatetime;
        }

        fire->ticsleft--;
        return;
    }

    background = &sky->skytex;
    background->currx += background->scrollx;
    background->curry += background->scrolly;

    if (sky->type == SkyType_WithForeground)
    {
        skytex_t    *foreground = &sky->foreground;

        foreground->currx += foreground->scrollx;
        foreground->curry += foreground->scrolly;
    }
}

void R_InitSkyMap(void)
{
    int skyheight;

    skyflatnum = R_FlatNumForName(SKYFLATNAME);
    terraintypes[skyflatnum] = SKY;
    skytexture = P_GetMapSky1Texture(gameepisode, gamemap);
    canfreelook = ((freelook || keyboardfreelook ||autotilt || (weaponrecoil && r_screensize == r_screensize_max)) && !nofreelook);

    if (!skytexture || (BTSX && !canfreelook))
    {
        if (gamemode == commercial)
        {
            if (gamemission == pack_nerve)
            {
                if (gamemap < 4 || gamemap == 9)
                    skytexture = R_TextureNumForName("SKY1");
                else
                    skytexture = R_TextureNumForName("SKY3");
            }
            else
            {
                if (gamemap < 12)
                    skytexture = R_TextureNumForName("SKY1");
                else if (gamemap < 21)
                    skytexture = R_TextureNumForName("SKY2");
                else
                    skytexture = R_TextureNumForName("SKY3");
            }
        }
        else
        {
            switch (gameepisode)
            {
                default:
                case 1:
                    skytexture = R_TextureNumForName("SKY1");
                    break;

                case 2:
                    skytexture = R_TextureNumForName("SKY2");
                    break;

                case 3:
                    skytexture = R_TextureNumForName("SKY3");
                    break;

                case 4:
                    skytexture = R_TextureNumForName("SKY4");
                    break;

                case 5:
                    skytexture = R_TextureNumForName(R_CheckTextureNumForName("SKY5_ZD") != -1 ? "SKY5_ZD" : "SKY5");
                    break;

                case 6:
                    skytexture = R_TextureNumForName(R_CheckTextureNumForName("SKY6_ZD") != -1 ? "SKY6_ZD" : "SKY6");
                    break;
            }
        }
    }

    InitSkyDefs();

    skyheight = textureheight[skytexture] >> FRACBITS;

    if (skyheight > 128 && skyheight < VANILLAHEIGHT)
        skytexturemid = -54 * FRACUNIT * skyheight / SKYSTRETCH_HEIGHT;
    else if (skyheight > VANILLAHEIGHT)
        skytexturemid = (VANILLAHEIGHT - skyheight) * FRACUNIT * skyheight / SKYSTRETCH_HEIGHT;
    else
        skytexturemid = VANILLAHEIGHT / 2 * FRACUNIT;

    if (canfreelook)
        skyiscale = (fixed_t)(((uint64_t)SCREENWIDTH * VANILLAHEIGHT * FRACUNIT) / ((uint64_t)viewwidth * SCREENHEIGHT))
            * skyheight / SKYSTRETCH_HEIGHT;
    else
        skyiscale = (fixed_t)(((uint64_t)SCREENWIDTH * VANILLAHEIGHT * FRACUNIT) / ((uint64_t)viewwidth * SCREENHEIGHT));

    skyscrolldelta = (vanilla ? 0 : (int)(P_GetMapSky1ScrollDelta(gameepisode, gamemap) * FRACUNIT));

    R_InitColumnFunctions();
}
