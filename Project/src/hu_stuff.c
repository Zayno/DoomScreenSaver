

#include <ctype.h>

#include "c_cmds.h"
#include "c_console.h"
#include "d_deh.h"
#include "doomstat.h"
#include "hu_lib.h"
#include "hu_stuff.h"
#include "i_colors.h"
#include "i_swap.h"
#include "i_timer.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_menu.h"
#include "m_misc.h"
#include "p_local.h"
#include "p_setup.h"
#include "st_stuff.h"
#include "v_video.h"
#include "w_wad.h"

#define NUMCROSSHAIRS   9
#define STSTR_BEHOLD2   "inVuln, bSrk, Inviso, Rad, Allmap or Lite-amp?"

patch_t* hu_font[HU_FONTSIZE];
static hu_textline_t    w_title;
static hu_textline_t    w_author;

bool                    message_fadeon;
bool                    message_dontfuckwithme;
bool                    message_secret;
bool                    message_warning;
bool                    message_on;
static bool             message_external;
static bool             message_nottobefuckedwith;

char                    prevmessage[133];

bool                    idbehold = false;
bool                    s_STSTR_BEHOLD2;

static hu_stext_t       w_message;
int                     message_counter;

static bool             headsupactive;

patch_t* minuspatch = NULL;
patch_t* buddha;
short                   minuspatchtopoffset1 = 0;
short                   minuspatchtopoffset2 = 0;
short                   minuspatchwidth = 0;
static patch_t* greenarmorpatch;
static patch_t* bluearmorpatch;

static patch_t* crosshairpatch[NUMCROSSHAIRS];
static short            crosshairwidth[NUMCROSSHAIRS];
static short            crosshairheight[NUMCROSSHAIRS];

static patch_t* stdisk;
static short            stdiskwidth;
static short            stdiskheight;
bool                    drawdisk = false;
int                     drawdisktics;

static int              coloroffset;

void A_Raise(mobj_t* actor, player_t* player, pspdef_t* psp);
void A_Lower(mobj_t* actor, player_t* player, pspdef_t* psp);

static void (*hudfunc)(int, int, patch_t*, const byte*);
static void (*hudnumfunc)(int, int, patch_t*, const byte*);
static void (*hudnumfunc2)(int, int, patch_t*, const byte*);

static void (*althudfunc)(int, int, patch_t*, int, int, const byte*, int);
void (*althudtextfunc)(int, int, byte*, patch_t*, bool, int, int, int, const byte*);
static void (*althudweaponfunc)(int, int, patch_t*, int, int, const byte*);
static void (*fillrectfunc)(int, int, int, int, int, int, int, bool, bool, const byte*, const byte*);
static void (*fillrectfunc2)(int, int, int, int, int, int, int, bool, bool, const byte*, const byte*);

static struct
{
	const char* patchnamea;
	const char* patchnameb;
	patch_t* patch;
} keypics[] = {
	{ "BKEYA0", "BKEYB0", NULL },
	{ "YKEYA0", "YKEYB0", NULL },
	{ "RKEYA0", "RKEYB0", NULL },
	{ "BSKUA0", "BSKUB0", NULL },
	{ "YSKUA0", "YSKUB0", NULL },
	{ "RSKUA0", "RSKUB0", NULL }
};

static void HU_AltInit(void);

static patch_t* HU_LoadHUDKeyPatch(int keypicnum)
{
	int lump;

	if (dehacked && (lump = W_CheckNumForName(keypics[keypicnum].patchnamea)) >= 0)
		return W_CacheLumpNum(lump);
	else if ((lump = W_CheckNumForName(keypics[keypicnum].patchnameb)) >= 0)
		return W_CacheLumpNum(lump);

	return NULL;
}

void HU_SetTranslucency(void)
{
	if (r_hud_translucency)
	{
		hudfunc = &V_DrawTranslucentHUDPatch;
		hudnumfunc = &V_DrawTranslucentHUDNumberPatch;
		hudnumfunc2 = &V_DrawTranslucentHighlightedHUDNumberPatch;
		althudfunc = &V_DrawTranslucentAltHUDPatch;
		althudtextfunc = &V_DrawTranslucentAltHUDText;
		althudweaponfunc = &V_DrawTranslucentAltHUDWeaponPatch;
		fillrectfunc = &V_FillSoftTransRect;
		fillrectfunc2 = &V_FillTransRect;
		coloroffset = 0;
	}
	else
	{
		hudfunc = &V_DrawHUDPatch;
		hudnumfunc = &V_DrawHUDPatch;
		hudnumfunc2 = &V_DrawHighlightedHUDNumberPatch;
		althudfunc = &V_DrawAltHUDPatch;
		althudtextfunc = &V_DrawAltHUDText;
		althudweaponfunc = &V_DrawAltHUDWeaponPatch;
		fillrectfunc = &V_FillRect;
		fillrectfunc2 = &V_FillRect;
		coloroffset = 4;
	}
}

void HU_Init(void)
{
	int lump;

	// load the heads-up font
	for (int i = 0, j = HU_FONTSTART; i < HU_FONTSIZE; i++)
	{
		char    buffer[9];

		M_snprintf(buffer, sizeof(buffer), "STCFN%03i", j++);
		hu_font[i] = W_CacheLumpName(buffer);

		if (W_GetNumLumps(buffer) > 1)
			STCFNxxx = true;
	}

	buddha = W_CacheLumpNameFromResourceWAD("DRBUDDH1");

	if ((lump = W_CheckNumForName("STTMINUS")) >= 0
		&& (W_GetNumLumps("STTMINUS") > 1 || W_GetNumLumps("STTNUM0") == 1))
	{
		minuspatch = W_CacheLumpNum(lump);
		minuspatchwidth = SHORT(minuspatch->width);

		if (SHORT(minuspatch->height) == 6 && !SHORT(minuspatch->topoffset))
		{
			minuspatchtopoffset1 = -5;
			minuspatchtopoffset2 = -5;
		}
		else
		{
			minuspatchtopoffset1 = 0;
			minuspatchtopoffset2 = SHORT(minuspatch->topoffset);
		}
	}

	if ((lump = W_CheckNumForName("ARM1A0")) >= 0)
		greenarmorpatch = W_CacheLumpNum(lump);

	if ((lump = W_CheckNumForName("ARM2A0")) >= 0)
		bluearmorpatch = W_CacheLumpNum(lump);

	for (int i = 0; i < NUMCROSSHAIRS; i++)
	{
		char    buffer[9];

		M_snprintf(buffer, sizeof(buffer), "DRXHAIR%i", i + 1);
		crosshairpatch[i] = W_CacheLumpName(buffer);
		crosshairwidth[i] = SHORT(crosshairpatch[i]->width);
		crosshairheight[i] = SHORT(crosshairpatch[i]->height);
	}

	for (int i = 0; i < NUMWEAPONS; i++)
	{
		const spritenum_t   sprite = weaponinfo[i].ammosprite;

		if (!sprite
			|| (gamemode == shareware && (i == wp_plasma || i == wp_bfg))
			|| (gamemode != commercial && i == wp_supershotgun))
			weaponinfo[i].ammopatch = NULL;
		else
		{
			weaponinfo[i].ammopatch = W_CacheLumpNum(firstspritelump + sprite);

			for (int j = numstates - 1; j >= 0; j--)
			{
				state_t* state = &states[j];

				if (state->sprite == sprite)
				{
					spriteframe_t* frame = &sprites[sprite].spriteframes[state->frame & FF_FRAMEMASK];

					if (frame)
						weaponinfo[i].ammopatch = W_CacheLumpNum(firstspritelump + frame->lump[0]);

					break;
				}
			}
		}
	}

	keypics[it_bluecard].patch = HU_LoadHUDKeyPatch(it_bluecard);
	keypics[it_yellowcard].patch = HU_LoadHUDKeyPatch(hacx ? it_yellowskull : it_yellowcard);
	keypics[it_redcard].patch = HU_LoadHUDKeyPatch(it_redcard);

	if (gamemode != shareware)
	{
		keypics[it_blueskull].patch = HU_LoadHUDKeyPatch(it_blueskull);
		keypics[it_yellowskull].patch = HU_LoadHUDKeyPatch(it_yellowskull);
		keypics[it_redskull].patch = HU_LoadHUDKeyPatch(it_redskull);
	}

	if ((lump = W_CheckNumForName(M_CheckParm("-cdrom") ? "STCDROM" : "STDISK")) >= 0)
	{
		stdisk = W_CacheLumpNum(lump);
		stdiskwidth = SHORT(stdisk->width);
		stdiskheight = SHORT(stdisk->height);
	}

	s_STSTR_BEHOLD2 = M_StringCompare(s_STSTR_BEHOLD, STSTR_BEHOLD2);

	HU_AltInit();
	HU_SetTranslucency();
}

static void HU_Stop(void)
{
	headsupactive = false;
}

void HU_Start(void)
{
	char* title = M_StringDuplicate(automaptitle);
	const char* author = P_GetMapAuthor(gameepisode, gamemap);
	char* byline = M_StringJoin("By ", author, NULL);
	char* s1;
	char* s2;

	if (headsupactive)
		HU_Stop();

	message_on = false;
	message_dontfuckwithme = false;
	message_secret = false;
	message_warning = false;
	message_nottobefuckedwith = false;
	message_external = false;

	// create the message widget
	HUlib_InitSText(&w_message, w_message.l.x, w_message.l.y, hu_font, HU_FONTSTART, &message_on);

	// create the map title widget
	HUlib_InitTextLine(&w_title, w_title.x, w_title.y, hu_font, HU_FONTSTART);
	HUlib_InitTextLine(&w_author, w_author.x, w_author.y, hu_font, HU_FONTSTART);

	s1 = title;
	s2 = byline;

	while (*s1 && *s1 != '\r' && *s1 != '\n')
		HUlib_AddCharToTextLine(&w_title, *(s1++));

	if (author && *author)
		while (*s2 && *s2 != '\r' && *s2 != '\n')
			HUlib_AddCharToTextLine(&w_author, *(s2++));

	free(title);
	free(byline);

	headsupactive = true;
}

static void DrawHUDNumber(int* x, int y, int val, const byte* tinttab,
	void (*drawhudnumfunc)(int, int, patch_t*, const byte*))
{
	int i;

	if (val < 0)
	{
		if (negativehealth && minuspatch)
		{
			val = -val;
			drawhudnumfunc(*x, y - minuspatchtopoffset2, minuspatch, tinttab);
			*x += minuspatchwidth;

			if (val == 1 || val == 7 || (val >= 10 && val <= 19) || (val >= 70 && val <= 79))
				(*x)--;
		}
		else
			val = 0;
	}

	if (val >= 100)
	{
		if ((i = val / 100) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x)++;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);

		if ((i = (val %= 100) / 10) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x) += 2;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);

		if ((i = val % 10) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x) += 2;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);
	}
	else if (val >= 10)
	{
		if ((i = val / 10) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x)++;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);

		if ((i = val % 10) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x) += 2;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);
	}
	else
	{
		if ((i = val % 10) == 1)
		{
			(*x)++;
			drawhudnumfunc(*x, y, tallnum[i], tinttab);
			(*x)++;
		}
		else
			drawhudnumfunc(*x, y, tallnum[i], tinttab);

		*x += SHORT(tallnum[i]->width);
	}
}

static int HUDNumberWidth(int val)
{
	int width = 0;

	if (val < 0)
	{
		if (negativehealth && minuspatch)
		{
			val = -val;
			width = minuspatchwidth;

			if (val == 1 || val == 7 || (val >= 10 && val <= 19) || (val >= 70 && val <= 79))
				width--;
		}
		else
			val = 0;
	}

	if (val >= 200)
		return (width + tallnum0width * 3);
	else if (val >= 100)
		return (width + tallnum0width * 3 - 1);
	else if (val >= 20)
		return (width + tallnum0width * 2);
	else if (val >= 10)
		return (width + tallnum0width * 2 - 1);

	return (width + tallnum0width);
}

static void HU_DrawCrosshair(void)
{
	if (r_hud_translucency)
		althudfunc((SCREENWIDTH - crosshairwidth[crosshair - 1]) / 2,
			(SCREENHEIGHT - SBARHEIGHT * (r_screensize < r_screensize_max) - crosshairheight[crosshair - 1]) / 2,
			crosshairpatch[crosshair - 1], WHITE, nearestcolors[crosshaircolor],
			(viewplayer->attackdown ? tinttab60 : tinttab50), -1);
	else
		althudfunc((SCREENWIDTH - crosshairwidth[crosshair - 1]) / 2,
			(SCREENHEIGHT - SBARHEIGHT * (r_screensize < r_screensize_max) - crosshairheight[crosshair - 1]) / 2,
			crosshairpatch[crosshair - 1], WHITE, (viewplayer->attackdown ? nearestcolors[crosshaircolor] :
				black25[nearestcolors[crosshaircolor]]), NULL, -1);
}

uint64_t    ammohighlight = 0;
uint64_t    armorhighlight = 0;
uint64_t    healthhighlight = 0;

int         ammodiff[NUMAMMO] = { 0 };
int         maxammodiff[NUMAMMO] = { 0 };
int         armordiff = 0;
int         healthdiff = 0;

static void HU_DrawHUD(void)
{
	patch_t* patch = faces[st_faceindex];

	if (patch)
	{
		const int ScaleFactor = 3;
		const int margin = 10;
		int pw = SHORT(patch->width) * ScaleFactor;   // Scaled width
		int ph = SHORT(patch->height) * ScaleFactor;  // Scaled height

		static bool flip = false;

		if (gametime % 350 == 0)//every 10 seconds at 35 fps
			flip = !flip;

		if (flip)
		{
			V_DrawHUDPatchScaled(SCREENWIDTH - pw - margin, SCREENHEIGHT - ph - margin, patch, ScaleFactor);
		}
		else
		{
			int y = SCREENHEIGHT - ph - margin;
			V_DrawHUDPatchScaled(margin, y, patch, ScaleFactor);
		}
	}
}

typedef struct
{
	int     color;
	byte* tinttab;
	patch_t* patch;
	short   width;
	short   height;
} altkeypic_t;

static altkeypic_t altkeypics[NUMCARDS] =
{
	{ BLUE3,   NULL, NULL },
	{ YELLOW1, NULL, NULL },
	{ RED2,    NULL, NULL },
	{ BLUE3,   NULL, NULL },
	{ YELLOW1, NULL, NULL },
	{ RED2,    NULL, NULL }
};

static patch_t* altnumpatch[10];
static patch_t* altnumpatch2[10];
static patch_t* altminuspatch;
static patch_t* altendpatch;
static patch_t* altleftpatch1;
static patch_t* altleftpatch2;
static patch_t* altrightpatch;
static patch_t* altmarkpatch;
static patch_t* altmark2patch;

static short    altminuspatchwidth;

static int      gray;
static int      darkgray;
static int      green1;
static int      green2;
static int      green3;
static int      blue2;
static int      blue3;
static int      red2;
static int      yellow1;
static int      yellow2;

typedef struct
{
	short   width;
	short   height;
	short   leftoffset;
	short   topoffset1;
	short   topoffset2;
} pistol_t;

static pistol_t pistol1[] =
{
	{  57,  62, -126, -106, -106 },
	{  79,  82, -104,  -86, -109 },
	{  66,  81, -119,  -87, -111 },
	{  61,  81, -125,  -87,  -87 },
	{  78, 103, -106,  -65,  -65 }
};

static pistol_t pistol2[] =
{
	{ 107,  94,  -76, -106, -106 },
	{ 116, 114,  -67,  -86,  -86 },
	{ 111, 113,  -74,  -87,  -87 },
	{ 109, 113,  -77,  -87,  -87 },
	{ 113, 135,  -71,  -65,  -65 }
};

static bool HU_DefaultPistolSprites(void)
{
	bool    result1 = true;
	bool    result2 = true;

	for (int i = 0; i < 5; i++)
	{
		spriteframe_t* frame = sprites[SPR_PISG].spriteframes;

		if (frame)
		{
			patch_t* patch = W_CacheLumpNum(firstspritelump + frame[i & FF_FRAMEMASK].lump[0]);

			if (SHORT(patch->width) != pistol1[i].width
				|| SHORT(patch->height) != pistol1[i].height
				|| SHORT(patch->leftoffset) != pistol1[i].leftoffset
				|| (SHORT(patch->topoffset) != pistol1[i].topoffset1
					&& SHORT(patch->topoffset) != pistol1[i].topoffset2))
			{
				result1 = false;
				break;
			}
		}
	}

	if (!result1)
		for (int i = 0; i < 5; i++)
		{
			spriteframe_t* frame = sprites[SPR_PISG].spriteframes;

			if (frame)
			{
				patch_t* patch = W_CacheLumpNum(firstspritelump + frame[i & FF_FRAMEMASK].lump[0]);

				if (SHORT(patch->width) != pistol2[i].width
					|| SHORT(patch->height) != pistol2[i].height
					|| SHORT(patch->leftoffset) != pistol2[i].leftoffset
					|| (SHORT(patch->topoffset) != pistol2[i].topoffset1
						&& SHORT(patch->topoffset) != pistol2[i].topoffset2))
				{
					result2 = false;
					break;
				}
			}
		}

	return (result1 || result2);
}

static void HU_AltInit(void)
{
	char    buffer[9];
	patch_t* altkeypatch;
	patch_t* altskullpatch;

	for (int i = 0; i <= 9; i++)
	{
		M_snprintf(buffer, sizeof(buffer), "DRHUD%iA", i);
		altnumpatch[i] = W_CacheLumpName(buffer);
		M_snprintf(buffer, sizeof(buffer), "DRHUD%iB", i);
		altnumpatch2[i] = W_CacheLumpName(buffer);
	}

	altminuspatch = W_CacheLumpName("DRHUDNEG");
	altminuspatchwidth = SHORT(altminuspatch->width);

	altleftpatch1 = W_CacheLumpName("DRHUDL1");
	altleftpatch2 = W_CacheLumpName("DRHUDL2");
	altrightpatch = W_CacheLumpName("DRHUDR");

	altendpatch = W_CacheLumpName("DRHUDE");
	altmarkpatch = W_CacheLumpName("DRHUDIA");
	altmark2patch = W_CacheLumpName("DRHUDIB");

	altkeypatch = W_CacheLumpName("DRHUDKEY");
	altskullpatch = W_CacheLumpName("DRHUDSKU");

	for (int i = 0; i < NUMCARDS; i++)
		if (lumpinfo[i]->wadfile->type == PWAD)
		{
			if (keypics[i].patch)
				altkeypics[i].color = FindBrightDominantColor(keypics[i].patch);
		}
		else if (!BTSX)
			altkeypics[i].color = nearestcolors[altkeypics[i].color];

	altkeypics[it_bluecard].patch = altkeypatch;
	altkeypics[it_bluecard].tinttab = tinttab90;
	altkeypics[it_yellowcard].patch = altkeypatch;
	altkeypics[it_yellowcard].tinttab = tinttab60;
	altkeypics[it_redcard].patch = altkeypatch;
	altkeypics[it_redcard].tinttab = tinttab60;
	altkeypics[it_blueskull].patch = altskullpatch;
	altkeypics[it_blueskull].tinttab = tinttab90;
	altkeypics[it_yellowskull].patch = altskullpatch;
	altkeypics[it_yellowskull].tinttab = tinttab60;
	altkeypics[it_redskull].patch = altskullpatch;
	altkeypics[it_redskull].tinttab = tinttab60;

	for (int i = 0; i < NUMCARDS; i++)
	{
		altkeypics[i].width = SHORT(altkeypics[i].patch->width);
		altkeypics[i].height = SHORT(altkeypics[i].patch->height);
	}

	for (int i = 0; i < NUMWEAPONS; i++)
	{
		M_snprintf(buffer, sizeof(buffer), "DRHUDWP%i", i);

		if (W_CheckNumForName(buffer) >= 0)
		{
			weaponinfo[i].weaponpatch = W_CacheLumpName(buffer);
			weaponinfo[i].weapony = ALTHUD_Y + 11 - SHORT(weaponinfo[i].weaponpatch->height) / 2;
		}
		else
		{
			const spritenum_t   sprite = weaponinfo[i].weaponsprite;

			if (!sprite
				|| (gamemode == shareware && (i == wp_plasma || i == wp_bfg))
				|| (gamemode != commercial && i == wp_supershotgun))
				weaponinfo[i].weaponpatch = NULL;
			else
				for (int j = numstates - 1; j >= 0; j--)
				{
					state_t* state = &states[j];

					if (state->sprite == sprite)
					{
						weaponinfo[i].weaponpatch = W_CacheLumpNum(firstspritelump
							+ sprites[state->sprite].spriteframes[state->frame & FF_FRAMEMASK].lump[0]);
						weaponinfo[i].weapony = ALTHUD_Y + 11 - SHORT(weaponinfo[i].weaponpatch->height) / 2;
						break;
					}
				}
		}
	}

	if (!HU_DefaultPistolSprites() && W_GetNumLumps("DRHUDWP1") == 1)
		weaponinfo[wp_pistol].weaponpatch = NULL;

	gray = nearestcolors[GRAY1];
	darkgray = nearestcolors[DARKGRAY1];
}

static void DrawAltHUDNumber(int x, int y, int val, int color, const byte* tinttab, int shadow)
{
	if (val < 0)
	{
		if (negativehealth)
		{
			val = -val;
			althudfunc(x - altminuspatchwidth - (val == 1 || val == 7 || (val >= 10 && val <= 19)
				|| (val >= 70 && val <= 79) || (val >= 100 && val <= 199) ? 1 : 2), y,
				altminuspatch, WHITE, color, tinttab, shadow);
		}
		else
			val = 0;
	}

	if (val == 1 || val % 10 == 1)
		x++;

	if (val >= 100)
	{
		patch_t* patch = altnumpatch[val / 100];

		althudfunc(x, y, patch, WHITE, color, tinttab, shadow);
		x += SHORT(patch->width) + 2;
		althudfunc(x, y, (patch = altnumpatch[(val %= 100) / 10]), WHITE, color, tinttab, shadow);
		althudfunc(x + SHORT(patch->width) + 2, y, altnumpatch[val % 10], WHITE, color, tinttab, shadow);
	}
	else if (val >= 10)
	{
		patch_t* patch = altnumpatch[val / 10];

		althudfunc(x, y, patch, WHITE, color, tinttab, shadow);
		althudfunc(x + SHORT(patch->width) + 2, y, altnumpatch[val % 10], WHITE, color, tinttab, shadow);
	}
	else
		althudfunc(x, y, altnumpatch[val % 10], WHITE, color, tinttab, shadow);
}

static int AltHUDNumberWidth(int val)
{
	int width = 0;

	if (val >= 100)
	{
		width = SHORT(altnumpatch[val / 100]->width) + 2;
		width += SHORT(altnumpatch[(val %= 100) / 10]->width) + 2;
	}
	else if (val >= 10)
		width = SHORT(altnumpatch[val / 10]->width) + 2;

	return (width + SHORT(altnumpatch[val % 10]->width));
}

static void DrawAltHUDNumber2(int x, int y, int val, int color, const byte* tinttab, int shadow)
{
	if (val == 1 || val % 10 == 1)
		x++;

	if (val >= 100)
	{
		patch_t* patch = altnumpatch2[val / 100];

		althudfunc(x, y, patch, WHITE, color, tinttab, shadow);
		x += SHORT(patch->width) + 2;

		patch = altnumpatch2[(val %= 100) / 10];

		althudfunc(x, y, patch, WHITE, color, tinttab, shadow);
		x += SHORT(patch->width) + 2;
	}
	else if (val >= 10)
	{
		patch_t* patch = altnumpatch2[val / 10];

		althudfunc(x, y, patch, WHITE, color, tinttab, shadow);
		x += SHORT(patch->width) + 2;
	}

	althudfunc(x, y, altnumpatch2[val % 10], WHITE, color, tinttab, shadow);
}

static int AltHUDNumber2Width(int val)
{
	int width = 0;

	if (val >= 100)
	{
		width = SHORT(altnumpatch2[val / 100]->width) + 2;
		width += SHORT(altnumpatch2[(val %= 100) / 10]->width) + 2;
	}
	else if (val >= 10)
		width = SHORT(altnumpatch2[val / 10]->width) + 2;

	return (width + SHORT(altnumpatch2[val % 10]->width));
}

void HU_Drawer(void)
{
	if (menuactive)
		return;

	if (*w_message.l.l)
	{
		if (vanilla && !vid_widescreen)
		{
			w_message.l.x = 0;
			w_message.l.y = 0;
		}
		else if ((r_screensize == r_screensize_max) || message_external)
		{
			w_message.l.x = MAXWIDESCREENDELTA / 2 - HU_MSGX - 3;
			w_message.l.y = HU_MSGY + 4;
		}
		else if (vid_widescreen && r_screensize == r_screensize_max - 1)
		{
			w_message.l.x = WIDESCREENDELTA + HU_MSGX * !automapactive;
			w_message.l.y = HU_MSGY + 1;
		}
		else
		{
			w_message.l.x = (!vid_widescreen && automapactive ? 0 : HU_MSGX);
			w_message.l.y = HU_MSGY + 1;
		}

		HUlib_DrawSText(&w_message, message_external);
	}


	{
		if (crosshair != crosshair_none)
		{
			const ammotype_t    ammotype = weaponinfo[viewplayer->readyweapon].ammotype;

			if (ammotype != am_noammo && viewplayer->ammo[ammotype])
			{
				const actionf_t action = viewplayer->psprites[ps_weapon].state->action;

				if (action != &A_Raise && action != &A_Lower)
					HU_DrawCrosshair();
			}
		}

		if (r_hud)
		{
			HU_DrawHUD();
		}

		if (mapwindow)
		{
			{
				w_title.x = 25;
				w_title.y = MAPHEIGHT - SHORT(hu_font[0]->height) * 2 - 16;
				HUlib_DrawAutomapTextLine(&w_title, true);
			}
		}
	}
}

void HU_Erase(void)
{
	if (message_on)
		HUlib_EraseSText(&w_message);

	if (mapwindow || automapactive)
		HUlib_EraseTextLine(&w_title);
}

void HU_Ticker(void)
{
	// tic down message counter if message is up
	if (message_counter && !menuactive && !--message_counter)
	{
		message_on = false;
		message_nottobefuckedwith = false;
		message_secret = false;
		message_warning = false;
	}

	// display message if necessary
	else if (viewplayer->message && (!message_nottobefuckedwith || message_dontfuckwithme)
		&& (r_hud || r_screensize < r_screensize_max))
	{
		if (messages || message_dontfuckwithme || message_secret || message_warning)
		{
			HUlib_AddMessageToSText(&w_message, viewplayer->message);

			message_fadeon = (!message_on || message_counter <= 5);
			message_on = true;
			message_counter = HU_MSGTIMEOUT;
			message_nottobefuckedwith = message_dontfuckwithme;
			message_dontfuckwithme = false;

			if (viewplayer->message)
				free(viewplayer->message);

			viewplayer->message = NULL;
		}
	}
}

void HU_SetPlayerMessage(char* message, bool group, bool external)
{
	if (message_secret || message_warning || !viewplayer || togglingvanilla)
		return;

	M_StringReplaceAll(message, "%%", "%", false);

	if (!group)
	{
		free(viewplayer->message);
		viewplayer->message = sentencecase(message);
	}
	else
	{
		static int  messagecount = 1;

		if (gametime - viewplayer->prevmessagetics < HU_MSGTIMEOUT
			&& M_StringCompare(message, viewplayer->prevmessage) && groupmessages)
		{
			char    buffer[133];
			char* temp = commify(++messagecount);

			M_snprintf(buffer, sizeof(buffer), "%s (%s)", message, temp);
			free(viewplayer->message);
			viewplayer->message = sentencecase(buffer);
			free(temp);
		}
		else
		{
			messagecount = 1;
			viewplayer->message = sentencecase(message);
			M_StringCopy(viewplayer->prevmessage, message, sizeof(viewplayer->prevmessage));
		}

		viewplayer->prevmessagetics = gametime;
	}

	M_StringCopy(prevmessage, viewplayer->message, sizeof(prevmessage));
	message_external = (external && mapwindow);
}

void HU_PlayerMessage(char* message, bool group, bool external)
{
	return;

	char        buffer[133] = "";
	const int   len = (int)strlen(message);

	if (!len || (len == 1 && !isalnum(message[0])))
		return;

	M_StringReplaceAll(message, " _", " " ITALICSON, false);
	M_StringReplaceAll(message, "_ ", ITALICSOFF " ", false);

	if (len >= 2 && message[0] == '%' && message[1] == 's')
		M_snprintf(buffer, sizeof(buffer), message,
			M_StringCompare(playername, playername_default) ? "you" : playername);
	else
		for (int i = 0, j = 0; i < len; i++)
		{
			if (message[i] == '%')
				buffer[j++] = '%';

			buffer[j++] = message[i];
		}

	buffer[0] = toupper(buffer[0]);
	C_PlayerMessage(buffer);

	if (gamestate == GS_LEVEL && !message_dontfuckwithme)
		HU_SetPlayerMessage(buffer, group, external);

	viewplayer->prevmessagetics = gametime;
}

void HU_ClearMessages(void)
{
	if (viewplayer->message)
		free(viewplayer->message);

	viewplayer->message = NULL;
	message_counter = 7;
	message_on = false;
	message_nottobefuckedwith = false;
	message_dontfuckwithme = false;
	message_secret = false;
	message_warning = false;
}
