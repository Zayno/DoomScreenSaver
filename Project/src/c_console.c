

#include <Windows.h>

#include "SDL_image.h"
#include "SDL_mixer.h"

#include "am_map.h"
#include "c_cmds.h"
#include "c_console.h"
#include "d_deh.h"
#include "doomstat.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "i_colors.h"
#include "i_controller.h"
#include "i_swap.h"
#include "i_system.h"
//#include "i_timer.h"
#include "m_cheat.h"
#include "m_config.h"
#include "m_menu.h"
#include "m_misc.h"
#include "p_spec.h"
#include "s_sound.h"
#include "v_video.h"
#include "version.h"
#include "w_wad.h"

console_t               *console = NULL;

bool                    consoleactive = false;
int                     consoleheight = 0;
int                     consoledirection = -1;
static int              consoleanim;

patch_t                 *consolefont[CONSOLEFONTSIZE];
patch_t                 *degree;
patch_t                 *unknownchar;
patch_t                 *altunderscores;
patch_t                 *altbuddha;
patch_t                 *lsquote;
patch_t                 *ldquote;

static patch_t          *brand;
static patch_t          *endash;
static patch_t          *trademark;
static patch_t          *copyright;
static patch_t          *regomark;
static patch_t          *multiply;
static patch_t          *warning;
static patch_t          *fps;
static patch_t          *ampm[2];

patch_t                 *bindlist;
patch_t                 *cmdlist;
patch_t                 *cvarlist;
patch_t                 *maplist;
patch_t                 *mapstats;
patch_t                 *playerstats;
patch_t                 *thinglist;

static short            brandwidth;
static short            brandheight;
static short            spacewidth;
static short            altbuddhawidth;
static short            fpswidth;
static short            ampmwidth;

char                    consoleinput[255] = "";
int                     numconsolestrings = 0;
size_t                  consolestringsmax = 0;

static size_t           undolevels;
static undohistory_t    *undohistory;

static bool             showcaret = true;
static uint64_t         caretwait;
int                     caretpos;
int                     selectstart;
int                     selectend;

bool                    pathoverlay;

char                    consolecheat[255];
char                    consolecheatparm[3];

static int              inputhistory = -1;
static int              outputhistory = -1;
static bool             topofconsole;
static bool             cheatsequence;

static int              degreewidth;
static int              suckswidth;
static int              timerwidth;
static int              timewidth;
static int              zerowidth;

static byte             *consoleautomapbevelcolor;
static byte             *consolebackcolor1;
static byte             *consolebackcolor2;
static byte             *consolebevelcolor1;
static byte             *consolebevelcolor2;
static int              consoleboldcolor;
static int              consolebolditalicscolor;
int                     consoleedgecolor1;
int                     consoleedgecolor2;
static int              consolecaretcolor;
static int              consoledividercolor;
static int              consoleinputcolor;
static int              consoleoutputcolor;
static int              consoleplayermessagecolor;
static int              consolescrollbarfacecolor;
static int              consolescrollbartrackcolor;
static int              consoleselectedinputbackgroundcolor;
static int              consoleselectedinputcolor;
static int              consolewarningboldcolor;
static int              consolewarningcolor;

static int              consolecolors[STRINGTYPES];
static int              consoleboldcolors[STRINGTYPES];

bool                    scrollbardrawn;
int                     scrollbarfacestart;
int                     scrollbarfaceend;

static void C_CreateTimeStamp(const int index)
{
}

void C_Input(const char *string, ...)
{
    return;

    va_list args;
    char    buffer[CONSOLETEXTMAXLENGTH];

    if (togglingvanilla)
        return;

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].indent = 0;
    console[numconsolestrings].wrap = 0;
    console[numconsolestrings++].stringtype = inputstring;
    inputhistory = -1;
    outputhistory = -1;
    consoleinput[0] = '\0';
    caretpos = 0;
    selectstart = 0;
    selectend = 0;
}

void C_Cheat(const char *string)
{
    char        buffer[CONSOLETEXTMAXLENGTH];
    const int   len = (int)strlen(string);

    for (int i = 0; i < len; i++)
        buffer[i] = '\x95';

    buffer[len] = '\0';

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].indent = 0;
    console[numconsolestrings].wrap = 0;
    console[numconsolestrings++].stringtype = cheatstring;
    inputhistory = -1;
    outputhistory = -1;
    consoleinput[0] = '\0';
    caretpos = 0;
    selectstart = 0;
    selectend = 0;
}

void C_IntegerCVAROutput(const char *cvar, const int value)
{
    char    *temp = commify(value);

    C_Input("%s %s", cvar, temp);
    free(temp);
}

void C_IntegerCVAROutputNoRepeat(const char *cvar, const int value)
{
    char    buffer[CONSOLETEXTMAXLENGTH];
    char    *temp = commify(value);

    M_snprintf(buffer, sizeof(buffer), "%s %s", cvar, temp);

    if (numconsolestrings && M_StringStartsWith(console[numconsolestrings - 1].string, cvar))
        M_StringCopy(console[numconsolestrings - 1].string, buffer, sizeof(console[0].string));
    else
        C_Input(buffer);

    free(temp);
}

void C_PercentCVAROutput(const char *cvar, const int value)
{
    char    *temp = commify(value);

    C_Input("%s %s%%", cvar, temp);
    free(temp);
}

void C_StringCVAROutput(const char *cvar, const char *string)
{
    C_Input("%s %s", cvar, string);
}

void C_Output(const char *string, ...)
{
    return;

    va_list args;
    char    buffer[CONSOLETEXTMAXLENGTH];

    if (!*string || togglingvanilla)
        return;

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].string[0] = toupper(console[numconsolestrings].string[0]);
    console[numconsolestrings].indent = 0;
    console[numconsolestrings].wrap = 0;
    console[numconsolestrings++].stringtype = outputstring;
    outputhistory = -1;
}

void C_TabbedOutput(const int tabs[MAXTABS], const char *string, ...)
{
    return;

    va_list args;
    char    buffer[CONSOLETEXTMAXLENGTH];

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].stringtype = outputstring;
    memcpy(console[numconsolestrings].tabs, tabs, sizeof(console[0].tabs));
    console[numconsolestrings].indent = (tabs[2] ? tabs[2] : (tabs[1] ? tabs[1] : tabs[0])) - 10;
    console[numconsolestrings++].wrap = 0;
    outputhistory = -1;
}

void C_Header(const int tabs[MAXTABS], patch_t *header, const char *string)
{
    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    console[numconsolestrings].stringtype = headerstring;
    memcpy(console[numconsolestrings].tabs, tabs, sizeof(console[0].tabs));
    console[numconsolestrings].header = header;
    console[numconsolestrings].wrap = 0;
    M_StringCopy(console[numconsolestrings++].string, string, sizeof(console[0].string));
    outputhistory = -1;
}

void C_Warning(const int minwarninglevel, const char *string, ...)
{
    return;

    va_list     args;
    char        buffer[CONSOLETEXTMAXLENGTH];
    const int   i = (numconsolestrings > 0 ? numconsolestrings - 1 : 0);

    if (con_warninglevel < minwarninglevel && !devparm)
        return;

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings > 0 && console[i].stringtype == warningstring && M_StringCompare(console[i].string, buffer))
        console[i].count++;
    else
    {
        if (numconsolestrings >= (int)consolestringsmax)
            console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

        M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
        console[numconsolestrings].indent = WARNINGWIDTH + 1;
        console[numconsolestrings].wrap = 0;
        console[numconsolestrings].stringtype = warningstring;
        console[numconsolestrings++].count = 1;
    }

    outputhistory = -1;
}

void C_PlayerMessage(const char *string, ...)
{
    return;

    va_list     args;
    char        buffer[CONSOLETEXTMAXLENGTH];
    const int   i = (numconsolestrings > 0 ? numconsolestrings - 1 : 0);

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings > 0 && console[i].stringtype == playermessagestring && M_StringCompare(console[i].string, buffer) && groupmessages)
    {
        C_CreateTimeStamp(i);
        console[i].count++;
    }
    else
    {
        if (numconsolestrings >= (int)consolestringsmax)
            console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

        M_StringReplaceAll(buffer, "\n", " ", false);
        M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
        console[numconsolestrings].stringtype = playermessagestring;
        C_CreateTimeStamp(numconsolestrings);
        console[numconsolestrings].string[0] = toupper(console[numconsolestrings].string[0]);
        console[numconsolestrings].indent = 0;
        console[numconsolestrings].wrap = 0;
        console[numconsolestrings++].count = 1;
    }

    outputhistory = -1;
}

void C_PlayerObituary(const char *string, ...)
{
    return;

    va_list args;
    char    buffer[CONSOLETEXTMAXLENGTH];

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].stringtype = playerwarningstring;
    C_CreateTimeStamp(numconsolestrings);
    console[numconsolestrings].string[0] = toupper(console[numconsolestrings].string[0]);
    console[numconsolestrings].indent = WARNINGWIDTH + 2;
    console[numconsolestrings].wrap = 0;
    console[numconsolestrings++].count = 1;

    outputhistory = -1;

    if (obituaries)
    {
        HU_SetPlayerMessage(buffer, false, false);
        message_warning = true;
    }
}

void C_PlayerWarning(const char *string, ...)
{
    return;

    va_list args;
    char    buffer[CONSOLETEXTMAXLENGTH];

    va_start(args, string);
    M_vsnprintf(buffer, CONSOLETEXTMAXLENGTH - 1, string, args);
    va_end(args);

    if (numconsolestrings >= (int)consolestringsmax)
        console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

    M_StringCopy(console[numconsolestrings].string, buffer, sizeof(console[0].string));
    console[numconsolestrings].stringtype = playerwarningstring;
    C_CreateTimeStamp(numconsolestrings);
    console[numconsolestrings].string[0] = toupper(console[numconsolestrings].string[0]);
    console[numconsolestrings].indent = WARNINGWIDTH + 2;
    console[numconsolestrings].wrap = 0;
    console[numconsolestrings++].count = 1;

    outputhistory = -1;
}

void C_ResetWrappedLines(void)
{
    for (int i = 0; i < numconsolestrings; i++)
        console[i].wrap = 0;
}

static void C_AddToUndoHistory(void)
{
    undohistory = I_Realloc(undohistory, (undolevels + 1) * sizeof(*undohistory));
    undohistory[undolevels].input = M_StringDuplicate(consoleinput);
    undohistory[undolevels].caretpos = caretpos;
    undohistory[undolevels].selectstart = selectstart;
    undohistory[undolevels].selectend = selectend;
    undolevels++;
}

void C_AddConsoleDivider(void)
{
    if (!numconsolestrings || console[numconsolestrings - 1].stringtype != dividerstring)
    {
        if (numconsolestrings >= (int)consolestringsmax)
            console = I_Realloc(console, (consolestringsmax += CONSOLESTRINGSMAX) * sizeof(*console));

        M_StringCopy(console[numconsolestrings].string, DIVIDERSTRING, sizeof(console[0].string));
        console[numconsolestrings++].stringtype = dividerstring;
    }
}

const kern_t altkern[] =
{
    { ' ',  ' ',  -1 }, { ' ',  'J',  -1 }, { ' ',  'T',  -1 }, { '!',  ' ',   2 },
    { '"',  '+',  -1 }, { '"',  ',',  -2 }, { '"',  '.',  -2 }, { '"',  '4',  -1 },
    { '"',  'a',  -1 }, { '"',  'c',  -1 }, { '"',  'd',  -1 }, { '"',  'e',  -1 },
    { '"',  'g',  -1 }, { '"',  'J',  -2 }, { '"',  'j',  -2 }, { '"',  'o',  -1 },
    { '"',  'q',  -1 }, { '"',  's',  -1 }, { '(',  '(',  -1 }, { '(',  '-',  -1 },
    { '(',  '4',  -1 }, { '(',  't',  -1 }, { ')',  ')',  -1 }, { '+',  'j',  -2 },
    { ',',  '-',  -1 }, { ',',  '4',  -1 }, { ',',  '7',  -1 }, { '.',  '"',  -1 },
    { '.',  '4',  -1 }, { '.',  '7',  -1 }, { '.',  '\\', -1 }, { '/',  '/',  -2 },
    { '/',  '4',  -1 }, { '/',  'a',  -1 }, { '/',  'c',  -1 }, { '/',  'd',  -1 },
    { '/',  'e',  -1 }, { '/',  'g',  -1 }, { '/',  'q',  -1 }, { '/',  's',  -1 },
    { '0',  ',',  -1 }, { '0',  '4',  -1 }, { '0',  ';',  -1 }, { '0',  'j',  -2 },
    { '1',  '"',  -1 }, { '1',  ',',  -1 }, { '1',  '\'', -1 }, { '1',  '\\', -1 },
    { '1',  'j',  -2 }, { '2',  ',',  -1 }, { '2',  '4',  -1 }, { '2',  'j',  -2 },
    { '3',  ',',  -1 }, { '3',  '4',  -1 }, { '3',  ';',  -1 }, { '3',  'j',  -2 },
    { '4',  '"',  -1 }, { '4',  ')',  -1 }, { '4',  ',',  -1 }, { '4',  '4',  -1 },
    { '4',  '7',  -1 }, { '4',  '\'', -1 }, { '4',  '\\', -1 }, { '4',  'j',  -2 },
    { '5',  ',',  -1 }, { '5',  '4',  -1 }, { '5',  ';',  -1 }, { '5',  'j',  -2 },
    { '6',  ',',  -1 }, { '6',  '4',  -1 }, { '6',  'j',  -2 }, { '7',  ',',  -2 },
    { '7',  '.',  -1 }, { '7',  '4',  -1 }, { '7',  ';',  -1 }, { '7',  'j',  -2 },
    { '8',  ',',  -1 }, { '8',  '4',  -1 }, { '8',  ';',  -1 }, { '8',  'j',  -2 },
    { '9',  ',',  -1 }, { '9',  '4',  -1 }, { '9',  ';',  -1 }, { '9',  'j',  -2 },
    { '?',  ' ',   2 }, { '\'', 'a',  -1 }, { '\'', 'c',  -1 }, { '\'', 'd',  -1 },
    { '\'', 'e',  -1 }, { '\'', 'g',  -1 }, { '\'', 'J',  -2 }, { '\'', 'j',  -2 },
    { '\'', 'o',  -1 }, { '\'', 's',  -1 }, { '\'', 't',  -1 }, { '\\', '\\', -2 },
    { '\\', 'T',  -1 }, { '\\', 't',  -1 }, { '\\', 'V',  -1 }, { '\\', 'v',  -1 },
    { '\t', '-',   1 }, { '\t', '4',  -1 }, { '_',  'f',  -1 }, { '_',  'T',  -1 },
    { '_',  't',  -1 }, { '_',  'V',  -1 }, { '_',  'v',  -1 }, { 'a',  '"',  -1 },
    { 'a',  '\\', -1 }, { 'a',  'j',  -2 }, { 'b',  '"',  -1 }, { 'b',  ',',  -1 },
    { 'b',  ';',  -1 }, { 'b',  '\\', -1 }, { 'b',  'j',  -2 }, { 'c',  '"',  -1 },
    { 'c',  ',',  -1 }, { 'c',  ';',  -1 }, { 'c',  '\\', -1 }, { 'c',  'j',  -2 },
    { 'D',  'j',  -1 }, { 'd',  'j',  -2 }, { 'e',  '"',  -1 }, { 'e',  ')',  -1 },
    { 'e',  ',',  -1 }, { 'e',  ';',  -1 }, { 'e',  '\\', -1 }, { 'e',  '_',  -1 },
    { 'e',  'j',  -2 }, { 'F',  ' ',  -1 }, { 'f',  ' ',  -1 }, { 'F',  ',',  -1 },
    { 'f',  ',',  -1 }, { 'F',  '.',  -1 }, { 'f',  '.',  -1 }, { 'F',  ';',  -1 },
    { 'f',  ';',  -1 }, { 'f',  '_',  -1 }, { 'F',  '_',  -2 }, { 'F',  'a',  -1 },
    { 'f',  'a',  -1 }, { 'F',  'e',  -1 }, { 'f',  'f',  -1 }, { 'F',  'J',  -1 },
    { 'f',  'j',  -2 }, { 'F',  'o',  -1 }, { 'f',  't',  -1 }, { 'h',  '\\', -1 },
    { 'h',  'j',  -2 }, { 'i',  'j',  -2 }, { 'k',  'j',  -2 }, { 'L',  ' ',  -1 },
    { 'L',  '"',  -1 }, { 'L',  '-',  -1 }, { 'L',  '\'', -1 }, { 'L',  '\\', -1 },
    { 'l',  'j',  -2 }, { 'L',  'Y',  -1 }, { 'm',  '"',  -1 }, { 'm',  '\\', -1 },
    { 'm',  'j',  -2 }, { 'n',  '"',  -1 }, { 'n',  '\'', -1 }, { 'n',  '\\', -1 },
    { 'n',  'j',  -2 }, { 'o',  '"',  -1 }, { 'o',  ',',  -1 }, { 'o',  ';',  -1 },
    { 'o',  '\\', -1 }, { 'o',  'j',  -2 }, { 'p',  '"',  -1 }, { 'P',  ',',  -1 },
    { 'p',  ',',  -1 }, { 'P',  '.',  -1 }, { 'P',  ';',  -1 }, { 'p',  ';',  -1 },
    { 'p',  '\\', -1 }, { 'P',  '_',  -1 }, { 'P',  'J',  -1 }, { 'p',  'j',  -2 },
    { 'r',  ' ',  -1 }, { 'r',  '"',  -1 }, { 'r',  ')',  -1 }, { 'r',  ',',  -2 },
    { 'r',  '.',  -2 }, { 'r',  '/',  -1 }, { 'r',  ';',  -1 }, { 'r',  '\\', -1 },
    { 'r',  '_',  -1 }, { 'r',  'a',  -1 }, { 'r',  'j',  -2 }, { 's',  ',',  -1 },
    { 's',  ';',  -1 }, { 's',  'j',  -2 }, { 'T',  ' ',  -1 }, { 'T',  ',',  -1 },
    { 'T',  '.',  -1 }, { 'T',  ';',  -1 }, { 'T',  'a',  -1 }, { 'T',  'e',  -1 },
    { 't',  'j',  -2 }, { 'T',  'o',  -1 }, { 't',  't',  -1 }, { 'u',  'j',  -2 },
    { 'V',  ',',  -1 }, { 'v',  ',',  -1 }, { 'V',  '.',  -1 }, { 'v',  '4',  -1 },
    { 'V',  ';',  -1 }, { 'v',  ';',  -1 }, { 'v',  '_',  -1 }, { 'V',  'a',  -1 },
    { 'v',  'a',  -1 }, { 'v',  'j',  -2 }, { 'w',  'j',  -2 }, { 'x',  '7',  -1 },
    { 'x',  'j',  -2 }, { 'Y',  ',',  -1 }, { 'Y',  '.',  -1 }, { 'Y',  ';',  -1 },
    { 'z',  'j',  -2 }, { '\0', '\0',  0 }
};

int C_TextWidth(const char *text, const bool formatting, const bool kerning)
{
    bool            italics = false;
    bool            monospaced = false;
    const int       len = (int)strlen(text);
    unsigned char   prevletter = '\0';
    int             width = 0;

    for (int i = 0; i < len; i++)
    {
        const unsigned char letter = text[i];
        unsigned char       nextletter = text[i + 1];

        if (letter == ' ')
            width += spacewidth;
        else if (letter == BOLDONCHAR || letter == BOLDOFFCHAR)
            continue;
        else if (letter == ITALICSONCHAR)
        {
            italics = true;
            continue;
        }
        else if (letter == ITALICSOFFCHAR)
        {
            italics = false;
            continue;
        }
        else if (letter == MONOSPACEDONCHAR)
        {
            monospaced = true;
            continue;
        }
        else if (letter == MONOSPACEDOFFCHAR)
        {
            monospaced = false;
            continue;
        }
        else if (monospaced)
            width += zerowidth;
        else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 't'
            && tolower(text[i + 2]) == 'm' && text[i + 3] == ')' && formatting)
        {
            width += SHORT(trademark->width);
            i += 3;
        }
        else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 'a'
            && tolower(text[i + 2]) == 'm' && text[i + 3] == ')' && formatting)
        {
            width += ampmwidth;
            i += 3;
        }
        else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 'p'
            && tolower(text[i + 2]) == 'm' && text[i + 3] == ')' && formatting)
        {
            width += ampmwidth;
            i += 3;
        }
        else if (letter == '(' && i < len - 2 && tolower(text[i + 1]) == 'c' && text[i + 2] == ')' && formatting)
        {
            width += SHORT(copyright->width);
            i += 2;
        }
        else if (letter == '(' && i < len - 2 && tolower(text[i + 1]) == 'r' && text[i + 2] == ')' && formatting)
        {
            width += SHORT(regomark->width);
            i += 2;
        }
        else if (letter == 215 || (letter == 'x' && isdigit(prevletter) && (nextletter == '\0' || isdigit(nextletter))))
            width += SHORT(multiply->width);
        else if (letter == '-' && prevletter == ' ' && !isdigit(nextletter))
            width += SHORT(endash->width);
        else if (!i || prevletter == ' ' || prevletter == '(' || prevletter == '[' || prevletter == '\t')
        {
            if (letter == '\'')
                width += SHORT(lsquote->width);
            else if (letter == '"')
                width += SHORT(ldquote->width);
            else
            {
                const int   c = letter - CONSOLEFONTSTART;

                width += SHORT((c >= 0 && c < CONSOLEFONTSIZE ? consolefont[c] : unknownchar)->width);
            }
        }
        else
        {
            const int   c = letter - CONSOLEFONTSTART;

            width += SHORT((c >= 0 && c < CONSOLEFONTSIZE ? consolefont[c] : unknownchar)->width);

            if (letter == '-' && italics)
                width++;
        }

        if (kerning)
        {
            for (int j = 0; altkern[j].char1; j++)
                if (prevletter == altkern[j].char1 && letter == altkern[j].char2)
                {
                    width += altkern[j].adjust;
                    break;
                }

            if (prevletter == '/' && italics)
                width -= 2;
        }

        prevletter = letter;
    }

    return width;
}

static int C_OverlayWidth(const char *text, const bool monospaced)
{
    const int   len = (int)strlen(text);
    int         width = 0;

    for (int i = 0; i < len; i++)
    {
        const unsigned char letter = text[i];

        if (letter == ' ')
            width += spacewidth;
        else if (isdigit(letter))
            width += (monospaced ? zerowidth : SHORT(consolefont[letter - CONSOLEFONTSTART]->width));
        else if (letter >= CONSOLEFONTSTART)
            width += (SHORT(consolefont[letter - CONSOLEFONTSTART]->width) - (letter == ','));
    }

    return width;
}

static void C_DrawScrollbar(void)
{
    return;

    scrollbarfacestart = CONSOLESCROLLBARHEIGHT * MAX(0, (outputhistory == -1 ?
        numconsolestrings - CONSOLEBLANKLINES - CONSOLELINES : outputhistory - CONSOLEBLANKLINES)) / numconsolestrings;
    scrollbarfaceend = scrollbarfacestart + CONSOLESCROLLBARHEIGHT - CONSOLESCROLLBARHEIGHT
        * MAX(0, numconsolestrings - CONSOLEBLANKLINES - CONSOLELINES) / numconsolestrings;

    if (!scrollbarfacestart && scrollbarfaceend == CONSOLESCROLLBARHEIGHT)
        scrollbardrawn = false;
    else
    {
        const int   offset = (CONSOLEHEIGHT - consoleheight) * SCREENWIDTH;
        const int   gripstart = (scrollbarfacestart + (scrollbarfaceend - scrollbarfacestart) / 2 - 2) * SCREENWIDTH - offset;
        const int   trackend = MAX(0, CONSOLESCROLLBARHEIGHT * SCREENWIDTH - offset);

        // draw scrollbar track
        for (int y = 0; y < trackend; y += SCREENWIDTH)
            for (int x = CONSOLESCROLLBARX; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH; x++)
            {
                byte    *dot = *screens + y + x;

                *dot = tinttab50[*dot + consolescrollbartrackcolor];
            }

        if (scrollbarfaceend - scrollbarfacestart > 8)
        {
            const int   gripend = gripstart + 6 * SCREENWIDTH;

            // init scrollbar grip
            for (int y = gripstart; y < gripend; y += 2 * SCREENWIDTH)
                if (y >= 0)
                    for (int x = CONSOLESCROLLBARX + 1; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH - 1; x++)
                        tempscreen[y + x] = screens[0][y + x];

            // draw scrollbar face
            const int   end = scrollbarfaceend * SCREENWIDTH - offset;

            for (int y = scrollbarfacestart * SCREENWIDTH - offset; y < end; y += SCREENWIDTH)
                if (y >= 0)
                    for (int x = CONSOLESCROLLBARX; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH; x++)
                        screens[0][y + x] = consolescrollbarfacecolor;

            // draw scrollbar grip
            for (int y = gripstart; y < gripend; y += 2 * SCREENWIDTH)
                if (y >= 0)
                    for (int x = CONSOLESCROLLBARX + 1; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH - 1; x++)
                        screens[0][y + x] = tempscreen[y + x];
        }
        else
        {
            const int   end = scrollbarfaceend * SCREENWIDTH - offset;

            // draw scrollbar face
            for (int y = scrollbarfacestart * SCREENWIDTH - offset; y < end; y += SCREENWIDTH)
                if (y >= 0)
                    for (int x = CONSOLESCROLLBARX; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH; x++)
                        screens[0][y + x] = consolescrollbarfacecolor;
        }

        // draw scrollbar face shadow
        if (scrollbarfaceend >= 0)
            for (int x = CONSOLESCROLLBARX; x < CONSOLESCROLLBARX + CONSOLESCROLLBARWIDTH; x++)
            {
                byte    *dot = *screens + scrollbarfaceend + x;

                *dot = tinttab10[*dot];
            }

        scrollbardrawn = true;
    }
}

void C_ClearConsole(void)
{
    return;

    numconsolestrings = 0;
    consolestringsmax = CONSOLESTRINGSMAX;
    console = I_Realloc(console, consolestringsmax * sizeof(*console));
    memset(console, 0, consolestringsmax * sizeof(*console));

    for (int i = 0; i < CONSOLEBLANKLINES; i++)
    {
        console[numconsolestrings].string[0] = '\0';
        console[numconsolestrings].indent = 0;
        console[numconsolestrings].wrap = 0;
        console[numconsolestrings++].stringtype = outputstring;
    }
}

static void C_InitEdgeColors(void)
{
    consoleedgecolor1 = FindBrightDominantColor(W_CacheLumpName("STTNUM0"));

    if (W_GetNumLumps("STTNUM0") >= 2
        && !(consoleedgecolor1 >= nearestcolors[LIGHTGRAY1] && consoleedgecolor1 <= nearestcolors[DARKGRAY4])
        && consoleedgecolor1 != nearestcolors[WHITE])
    {
        consoleedgecolor2 = black25[consoleedgecolor1] << 8;
        consoleedgecolor1 <<= 8;
    }
    else
    {
        consoleedgecolor1 = FindBrightDominantColor(W_CacheLumpName("M_NGAME"));

        if (W_GetNumLumps("M_NGAME") >= 2
            && !(consoleedgecolor1 >= nearestcolors[LIGHTGRAY1] && consoleedgecolor1 <= nearestcolors[DARKGRAY4])
            && consoleedgecolor1 != nearestcolors[WHITE])
        {
            consoleedgecolor2 = black25[consoleedgecolor1] << 8;
            consoleedgecolor1 <<= 8;
        }
        else
        {
            consoleedgecolor1 = nearestcolors[CONSOLEEDGECOLOR1] << 8;
            consoleedgecolor2 = nearestcolors[CONSOLEEDGECOLOR2] << 8;
        }
    }
}

void C_Init(void)
{
    return;

    char    *appdatafolder = M_GetAppDataFolder();
    char    consolefolder[MAX_PATH];
    char    buffer[9];

    M_snprintf(consolefolder, sizeof(consolefolder), "%s" DIR_SEPARATOR_S DOOMRETRO_CONSOLEFOLDER, appdatafolder);
    free(appdatafolder);

    for (int i = 0, j = CONSOLEFONTSTART; i < CONSOLEFONTSIZE; i++)
    {
        M_snprintf(buffer, sizeof(buffer), "DRFON%03i", j++);
        consolefont[i] = W_CacheLastLumpName(W_CheckNumForName(buffer) >= 0 ? buffer : "DRFON000");
    }

    consoleautomapbevelcolor = &tinttab50[nearestcolors[CONSOLEAUTOMAPBEVELCOLOR] << 8];
    consolebackcolor1 = &tinttab50[nearestcolors[CONSOLEBACKCOLOR] << 8];
    consolebackcolor2 = &tinttab60[nearestblack << 8];
    consolebevelcolor1 = &tinttab50[nearestcolors[CONSOLEBEVELCOLOR] << 8];
    consolebevelcolor2 = &tinttab20[nearestcolors[CONSOLEBEVELCOLOR] << 8];
    consoleboldcolor = nearestcolors[CONSOLEBOLDCOLOR];
    consolebolditalicscolor = nearestcolors[CONSOLEBOLDITALICSCOLOR];
    consolecaretcolor = nearestcolors[CONSOLECARETCOLOR];
    consoledividercolor = nearestcolors[CONSOLEDIVIDERCOLOR] << 8;
    consoleinputcolor = nearestcolors[CONSOLEINPUTCOLOR];
    consoleoutputcolor = nearestcolors[CONSOLEOUTPUTCOLOR];
    consoleplayermessagecolor = (harmony ? 226 : nearestcolors[CONSOLEPLAYERMESSAGECOLOR]);
    consolescrollbarfacecolor = nearestcolors[CONSOLESCROLLBARFACECOLOR];
    consolescrollbartrackcolor = nearestcolors[CONSOLESCROLLBARTRACKCOLOR] << 8;
    consoleselectedinputbackgroundcolor = nearestcolors[CONSOLESELECTEDINPUTBACKGROUNDCOLOR];
    consoleselectedinputcolor = nearestcolors[CONSOLESELECTEDINPUTCOLOR];
    consolewarningboldcolor = nearestcolors[CONSOLEWARNINGBOLDCOLOR];
    consolewarningcolor = nearestcolors[CONSOLEWARNINGCOLOR];

    C_InitEdgeColors();

    consolecolors[inputstring] = consoleinputcolor;
    consolecolors[cheatstring] = consoleinputcolor;
    consolecolors[outputstring] = consoleoutputcolor;
    consolecolors[warningstring] = consolewarningcolor;
    consolecolors[playermessagestring] = consoleplayermessagecolor;
    consolecolors[playerwarningstring] = consolewarningcolor;

    consoleboldcolors[inputstring] = consoleboldcolor;
    consoleboldcolors[cheatstring] = consoleboldcolor;
    consoleboldcolors[outputstring] = consoleboldcolor;
    consoleboldcolors[warningstring] = consolewarningboldcolor;
    consoleboldcolors[playermessagestring] = consoleplayermessagecolor;
    consoleboldcolors[playerwarningstring] = consolewarningboldcolor;

    brand = W_CacheLastLumpName("DRBRAND");

    unknownchar = W_CacheLastLumpName("DRFON000");
    lsquote = consolefont[0x91 - CONSOLEFONTSTART];
    ldquote = consolefont[0x93 - CONSOLEFONTSTART];
    endash = consolefont[0x96 - CONSOLEFONTSTART];
    trademark = consolefont[0x99 - CONSOLEFONTSTART];
    copyright = consolefont[0xA9 - CONSOLEFONTSTART];
    regomark = consolefont[0xAE - CONSOLEFONTSTART];
    degree = consolefont[0xB0 - CONSOLEFONTSTART];
    multiply = consolefont[0xD7 - CONSOLEFONTSTART];

    warning = W_CacheLastLumpName("DRFONWRN");
    altunderscores = W_CacheLastLumpName("DRFONUND");
    altbuddha = W_CacheLumpNameFromResourceWAD("DRBUDDH2");
    fps = W_CacheLastLumpName("DRFONFPS");
    ampm[0] = W_CacheLastLumpName("DRFONAM");
    ampm[1] = W_CacheLastLumpName("DRFONPM");

    bindlist = W_CacheLastLumpName("DRBNDLST");
    cmdlist = W_CacheLastLumpName("DRCMDLST");
    cvarlist = W_CacheLastLumpName("DRCVRLST");
    maplist = W_CacheLastLumpName("DRMAPLST");
    mapstats = W_CacheLastLumpName("DRMAPST");
    playerstats = W_CacheLastLumpName("DRPLYRST");
    thinglist = W_CacheLastLumpName("DRTHNLST");

    brandwidth = SHORT(brand->width);
    brandheight = SHORT(brand->height);
    degreewidth = SHORT(degree->width);
    spacewidth = SHORT(consolefont[' ' - CONSOLEFONTSTART]->width);
    zerowidth = SHORT(consolefont['0' - CONSOLEFONTSTART]->width);
    altbuddhawidth = SHORT(altbuddha->width);
    fpswidth = SHORT(fps->width);
    ampmwidth = SHORT(ampm[0]->width);

    suckswidth = C_OverlayWidth(s_STSTR_SUCKS, false);
    timewidth = C_OverlayWidth("00:00.00", true);

    M_TranslateAutocomplete();
}

void C_ShowConsole(bool reset)
{
    return;

    consoleheight = MAX(1, consoleheight);
    consoledirection = 1;
    consoleanim = 0;
    showcaret = true;
    caretwait = 0;

    if (reset)
    {
        consoleinput[0] = '\0';
        caretpos = 0;
        selectstart = 0;
        selectend = 0;
        undolevels = 0;
        inputhistory = -1;
        outputhistory = -1;
    }

    for (int i = 0; i < MAXMOUSEBUTTONS + 2; i++)
        mousebuttons[i] = false;

    if (gamestate == GS_LEVEL)
        I_RestoreMousePointerPosition();

    if (keyboardalwaysrun == KEY_CAPSLOCK && alwaysrun && GetCapsLockState())
    {
        ToggleCapsLockState();
        nokeyevent = true;
    }

    S_StopSounds();
    S_LowerMusicVolume();
    SDL_StartTextInput();
    S_StartSound(NULL, sfx_consol);
}

void C_HideConsole(void)
{
    return;

    if (!consoleactive)
        return;

    if (keyboardalwaysrun == KEY_CAPSLOCK && alwaysrun && !GetCapsLockState())
    {
        ToggleCapsLockState();
        nokeyevent = true;
    }

    SDL_StopTextInput();

    consoledirection = -1;
    consoleanim = 0;

    I_SaveMousePointerPosition();

    S_StartSound(viewplayer->mo, sfx_consol);
    S_RestoreMusicVolume();
}

void C_HideConsoleFast(void)
{
    return;

    if (!consoleactive)
        return;

    if (keyboardalwaysrun == KEY_CAPSLOCK && alwaysrun && !GetCapsLockState())
    {
        ToggleCapsLockState();
        nokeyevent = true;
    }

    SDL_StopTextInput();

    consoledirection = -1;
    consoleanim = 0;
    consoleheight = 0;
    consoleactive = false;

    I_SaveMousePointerPosition();

    S_RestoreMusicVolume();
}

static void C_DrawBackground(void)
{
    return;

    const bool  inverted = ((viewplayer->fixedcolormap == INVERSECOLORMAP) != !r_textures);
    const int   height = (consoleheight + 5) * SCREENWIDTH;
    static byte blurscreen[MAXSCREENAREA];

    // apply random noise to background (use visual RNG to not affect gameplay)
    for (int i = height; i >= 0; i--)
        screens[0][i] = colormaps[0][M_VisualRandomInt(0, 7) * 256 + screens[0][i]];

    // blur background
    memcpy(blurscreen, screens[0], height);

    for (int y = 0; y <= height - SCREENWIDTH; y += SCREENWIDTH)
        for (int x = y; x <= y + SCREENWIDTH - 2; x++)
            blurscreen[x] = tinttab50[(blurscreen[x + 1] << 8) + blurscreen[x]];

    for (int y = 0; y <= height - SCREENWIDTH; y += SCREENWIDTH)
        for (int x = y + SCREENWIDTH - 2; x > y; x--)
            blurscreen[x] = tinttab50[(blurscreen[x - 1] << 8) + blurscreen[x]];

    for (int y = height - SCREENWIDTH; y >= SCREENWIDTH; y -= SCREENWIDTH)
        for (int x = y + SCREENWIDTH - 1; x >= y + 1; x--)
            blurscreen[x] = tinttab50[(blurscreen[x - SCREENWIDTH - 1] << 8) + blurscreen[x]];

    for (int y = 0; y <= height - 2 * SCREENWIDTH; y += SCREENWIDTH)
        for (int x = y; x <= y + SCREENWIDTH - 1; x++)
            blurscreen[x] = tinttab50[(blurscreen[x + SCREENWIDTH] << 8) + blurscreen[x]];

    for (int y = height - SCREENWIDTH; y >= SCREENWIDTH; y -= SCREENWIDTH)
        for (int x = y; x <= y + SCREENWIDTH - 1; x++)
            blurscreen[x] = tinttab50[(blurscreen[x - SCREENWIDTH] << 8) + blurscreen[x]];

    for (int y = 0; y <= height - 2 * SCREENWIDTH; y += SCREENWIDTH)
        for (int x = y + SCREENWIDTH - 1; x >= y + 1; x--)
            blurscreen[x] = tinttab50[(blurscreen[x + SCREENWIDTH - 1] << 8) + blurscreen[x]];

    for (int y = height - SCREENWIDTH; y >= SCREENWIDTH; y -= SCREENWIDTH)
        for (int x = y; x <= y + SCREENWIDTH - 2; x++)
            blurscreen[x] = tinttab50[(blurscreen[x - SCREENWIDTH + 1] << 8) + blurscreen[x]];

    // tint background
    if (inverted)
        for (int i = 0; i < height; i++)
            screens[0][i] = consolebackcolor2[blurscreen[i]];
    else
        for (int i = 0; i < height; i++)
            screens[0][i] = consolebackcolor1[blurscreen[i]];

    // apply corrugated glass effect to background
    for (int y = consoleheight % 3; y <= height - 3 * SCREENWIDTH; y += SCREENWIDTH)
    {
        for (int x = y + 2; x < y + SCREENWIDTH - 1; x += 3)
            screens[0][x] = colormaps[0][6 * 256 + screens[0][x + ((x % SCREENWIDTH) ? -1 : 1)]];

        for (int x = (y += SCREENWIDTH) + 1; x < y + SCREENWIDTH - 1; x += 3)
            screens[0][x] = colormaps[0][6 * 256 + screens[0][x + ((x % SCREENWIDTH) ? -1 : 1)]];

        for (int x = (y += SCREENWIDTH); x < y + SCREENWIDTH - 1; x += 3)
            screens[0][x] = colormaps[0][6 * 256 + screens[0][x + ((x % SCREENWIDTH) ? -1 : 1)]];
    }

    // draw bottom edge
    if (con_edgecolor == con_edgecolor_auto)
    {
        V_DrawConsoleBrandingPatch(SCREENWIDTH - MAXWIDESCREENDELTA - brandwidth + (vid_widescreen ? 19 : 44),
            consoleheight - brandheight + 2, brand, consoleedgecolor1, consoleedgecolor2,
            (luminance[consoleedgecolor1 >> 8] <= 128 ? nearestwhite : nearestblack));

        for (int i = height - 3 * SCREENWIDTH; i < height; i++)
            screens[0][i] = tinttab60[consoleedgecolor1 + screens[0][i]];
    }
    else
    {
        const int   color1 = nearestcolors[con_edgecolor] << 8;
        const int   color2 = black25[color1 >> 8] << 8;

        V_DrawConsoleBrandingPatch(SCREENWIDTH - MAXWIDESCREENDELTA - brandwidth + (vid_widescreen ? 19 : 44),
            consoleheight - brandheight + 2, brand, color1, color2,
            (luminance[color1 >> 8] <= 128 ? nearestwhite : nearestblack));

        for (int i = height - 3 * SCREENWIDTH; i < height; i++)
            screens[0][i] = tinttab60[color1 + screens[0][i]];
    }

    // bevel left and right edges
    if (automapactive && am_backcolor == am_backcolor_default)
    {
        for (int i = 0; i < height - 3 * SCREENWIDTH; i += SCREENWIDTH)
            screens[0][i] = consoleautomapbevelcolor[screens[0][i + 1]];

        for (int i = MAX(0, height - 3 * SCREENWIDTH); i < height; i += SCREENWIDTH)
            screens[0][i] = consolebevelcolor1[screens[0][i + 1]];

        for (int i = 0; i < height - (brandheight + 3) * SCREENWIDTH; i += SCREENWIDTH)
            screens[0][i + SCREENWIDTH - 1] = consoleautomapbevelcolor[screens[0][i + SCREENWIDTH - 2]];

        for (int i = MAX(0, height - (brandheight + 3) * SCREENWIDTH); i < height; i += SCREENWIDTH)
            screens[0][i + SCREENWIDTH - 1] = consolebevelcolor1[screens[0][i + SCREENWIDTH - 2]];
    }
    else
        for (int i = 0; i < height; i += SCREENWIDTH)
        {
            screens[0][i] = consolebevelcolor1[screens[0][i + 1]];
            screens[0][i + SCREENWIDTH - 1] = consolebevelcolor1[screens[0][i + SCREENWIDTH - 2]];
        }

    // bevel bottom edge
    if (inverted)
        for (int i = height - SCREENWIDTH + 1; i < height - 1; i++)
            screens[0][i] = consolebevelcolor2[screens[0][i]];
    else
        for (int i = height - SCREENWIDTH + 1; i < height - 1; i++)
            screens[0][i] = consolebevelcolor1[screens[0][i]];

    // draw shadow
    for (int i = SCREENWIDTH; i <= 4 * SCREENWIDTH; i += SCREENWIDTH)
        for (int j = height; j < height + i && j < SCREENAREA; j++)
            screens[0][j] = colormaps[0][4 * 256 + screens[0][j]];
}

static int C_DrawConsoleText(int x, int y, char *text, const int color1, const int color2,
    const int boldcolor, const byte *tinttab, const int tabs[MAXTABS], const bool formatting,
    const bool kerning, const bool wrapped, const int index, unsigned char prevletter,
    unsigned char prevletter2, void consoletextfunc(const int, const int, const patch_t *,
        const int, const int, const int, const bool, const byte *))
{
    return 0;

    bool            bold = false;
    bool            bolder = false;
    bool            italics = false;
    bool            monospaced = false;
    int             tab = -1;
    const int       len = (int)strlen(text);
    int             startx = x;
    unsigned char   prevletter3 = '\0';

    y -= CONSOLEHEIGHT - consoleheight;

    if (console[index].stringtype == warningstring
        || console[index].stringtype == playerwarningstring)
    {
        V_DrawConsoleTextPatch(x - 1, y, warning, WARNINGWIDTH, color1, color2, false, tinttab);
        x += (text[0] == 'T' ? WARNINGWIDTH : WARNINGWIDTH + 1);
    }

    if (M_StringCompare(text, s_STSTR_BUDDHA))
    {
        V_DrawConsoleTextPatch(x, y, altbuddha, altbuddhawidth, color1, color2, false, tinttab);
        return 0;
    }

    for (int i = 0; i < len; i++)
    {
        unsigned char   letter = text[i];
        unsigned char   nextletter = text[i + 1];

        if (cheatsequence)
        {
            letter = '\x95';
            nextletter = '\x95';
        }

        if (letter == BOLDONCHAR)
        {
            if (bold)
                bolder = true;
            else
                bold = true;
        }
        else if (letter == BOLDOFFCHAR)
        {
            if (bolder)
                bolder = false;
            else
                bold = false;
        }
        else if (letter == ITALICSONCHAR)
            italics = true;
        else if (letter == ITALICSOFFCHAR)
            italics = false;
        else if (letter == MONOSPACEDONCHAR)
            monospaced = true;
        else if (letter == MONOSPACEDOFFCHAR)
            monospaced = false;
        else
        {
            patch_t *patch = NULL;

            if (letter == ' ' && formatting)
                x += (monospaced ? zerowidth : spacewidth);
            else if (letter == '\t')
                x = (x > tabs[++tab] + CONSOLETEXTX - 10 ? x + spacewidth : tabs[tab] + CONSOLETEXTX - 10);
            else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 't' && tolower(text[i + 2]) == 'm' && text[i + 3] == ')'
                && formatting)
            {
                patch = trademark;
                i += 3;
            }
            else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 'a' && tolower(text[i + 2]) == 'm' && text[i + 3] == ')'
                && formatting)
            {
                patch = ampm[0];
                i += 3;
            }
            else if (letter == '(' && i < len - 3 && tolower(text[i + 1]) == 'p' && tolower(text[i + 2]) == 'm' && text[i + 3] == ')'
                && formatting)
            {
                patch = ampm[1];
                i += 3;
            }
            else if (letter == '(' && i < len - 2 && tolower(text[i + 1]) == 'c' && text[i + 2] == ')' && formatting)
            {
                patch = copyright;
                i += 2;
            }
            else if (letter == '(' && i < len - 2 && tolower(text[i + 1]) == 'r' && text[i + 2] == ')' && formatting)
            {
                patch = regomark;
                i += 2;
            }
            else if (letter == 'x' && isdigit(prevletter) && (i == len - 1 || isdigit(nextletter)))
                patch = multiply;
            else if (letter == '-' && (prevletter == ' ' || (prevletter == BOLDONCHAR && prevletter2 == ' '))
                && !isdigit(nextletter))
                patch = endash;
            else if (letter == '\n')
                break;
            else
            {
                const int   c = letter - CONSOLEFONTSTART;

                patch = (c >= 0 && c < CONSOLEFONTSIZE ? consolefont[c] : unknownchar);

                if (letter == '\'')
                {
                    if (prevletter == '\0' || (prevletter == ' ' && prevletter2 != '\'')
                        || prevletter == '\t' || prevletter == '(' || prevletter == '['
                        || prevletter == '{' || prevletter == '<' || prevletter == '"'
                        || ((prevletter == BOLDONCHAR || prevletter == ITALICSONCHAR)
                            && prevletter2 != '.' && nextletter != '.'))
                    {
                        patch = lsquote;

                        if (prevletter == '\0' && formatting)
                            x--;
                    }
                    else if (prevletter == 'I' && italics)
                        x++;
                }
                else if (letter == '"')
                {
                    if (prevletter == '\0' || (prevletter == ' ' && prevletter2 != '"')
                        || prevletter == '\t' || prevletter == '(' || prevletter == '['
                        || prevletter == '{' || prevletter == '<' || prevletter == '\''
                        || ((prevletter == BOLDONCHAR || prevletter == ITALICSONCHAR)
                            && prevletter2 != '.' && nextletter != '.'))
                    {
                        patch = ldquote;

                        if (prevletter == '\0' && formatting)
                            x--;
                    }
                }
            }

            if (kerning && !monospaced)
            {
                for (int j = 0; altkern[j].char1; j++)
                    if (prevletter == altkern[j].char1 && letter == altkern[j].char2)
                    {
                        x += altkern[j].adjust;
                        break;
                    }

                if (italics)
                {
                    if (letter == '-')
                        x++;
                    else if (letter == '\'' && prevletter != 'M')
                        x--;
                    else if (letter == 'v' && prevletter == '-')
                        x--;

                    if (prevletter == '/')
                        x--;
                    else if (prevletter == '\'')
                        x++;

                    if (letter == 'T' && prevletter == ITALICSONCHAR && prevletter2 == ' ')
                        x--;

                    if (prevletter == ITALICSONCHAR && prevletter2 == '\t')
                        x--;
                }
                else if ((letter == '-' || letter == '|' || letter == '[' || letter == ']' || letter == 215)
                    && prevletter == ITALICSOFFCHAR)
                    x++;
                else if (letter == '(' && prevletter == ' ')
                {
                    if (prevletter2 == '.')
                        x--;
                    else if (prevletter2 == '!')
                        x -= 2;
                }
                else if (prevletter == BOLDONCHAR
                    && (prevletter2 == '\t' || (prevletter2 == BOLDONCHAR && prevletter3 == '\t')))
                {
                    if (letter == '"' || letter == '\'' || letter == '(' || letter == '4')
                        x--;
                }
                else if (prevletter == BOLDOFFCHAR)
                {
                    if ((letter == ' ' || letter == ')' || letter == '.') && prevletter2 == 'r')
                        x--;
                    else if (letter == 'f' && prevletter2 == '[')
                        x--;
                    else if (letter == ',' && (prevletter2 == '"' || prevletter2 == '\'' || prevletter2 == 'r'))
                        x -= 2;
                    else if (letter == ',' && (prevletter2 == 'e' || prevletter2 == 'f'))
                        x--;
                    else if (letter == '.' && prevletter2 == '\"')
                        x--;
                }
            }

            if (patch)
            {
                int width = SHORT(patch->width);

                consoletextfunc(x + (monospaced && width <= zerowidth ? (zerowidth - width) / 2 : 0), y,
                    patch, width, (bold && italics ? (color1 == consolewarningcolor ? color1 :
                        consolebolditalicscolor) : (bold ? boldcolor : color1)),
                    color2, (italics && letter != '_' && letter != '-' && letter != '+' && letter != ','
                        && letter != '/' && patch != unknownchar), (bolder ? NULL : tinttab));
                x += (monospaced && width < zerowidth ? zerowidth : width) - (monospaced && letter == '4');

                if (x >= CONSOLETEXTPIXELWIDTH && wrapped)
                {
                    for (int j = 1; j <= 3; j++)
                    {
                        patch = consolefont['.' - CONSOLEFONTSTART];
                        width = SHORT(patch->width);
                        consoletextfunc(x, y, patch, width, (bold && italics ? (color1 == consolewarningcolor ?
                            color1 : consolebolditalicscolor) : (bold ? boldcolor : color1)), color2, false,
                            (bolder ? NULL : tinttab));
                        x += (monospaced ? zerowidth : width);
                    }

                    break;
                }
            }
        }

        prevletter3 = prevletter2;
        prevletter2 = prevletter;
        prevletter = letter;
    }

    return (x - startx);
}

static void C_DrawOverlayText(byte *screen, const int screenwidth, int x,
    const int y, const byte *tinttab, const char *text, const int color,
    const bool monospaced, const int shadowcolor)
{
    return;

    const int   len = (int)strlen(text);

    for (int i = 0; i < len; i++)
    {
        const unsigned char letter = text[i];

        if (letter == ' ')
            x += spacewidth;
        else
        {
            patch_t     *patch = consolefont[letter - CONSOLEFONTSTART];
            const int   width = SHORT(patch->width);

            if (isdigit(letter) && monospaced)
            {
                V_DrawOverlayTextPatch(screen, screenwidth, x + (letter == '1') - (letter == '4'),
                    y, patch, width - 1, color, shadowcolor, tinttab);
                x += zerowidth;
            }
            else
            {
                V_DrawOverlayTextPatch(screen, screenwidth, x - (letter == ','),
                    y, patch, width - 1, color, shadowcolor, tinttab);
                x += (width - (letter == ','));
            }
        }
    }
}

static void C_DrawTimeStamp(int x, const int y, const int index, const int color)
{
    return;

}

static byte C_GetOverlayTextColor(void)
{
    if (((viewplayer->fixedcolormap == INVERSECOLORMAP) != !r_textures) && !automapactive && gamestate == GS_LEVEL)
        return nearestblack;
    else if (automapactive)
        return nearestcolors[am_playerstatscolor];
    else
        return nearestwhite;
}

static int C_GetOverlayTextShadowColor(void)
{
    return ((viewplayer->fixedcolormap == INVERSECOLORMAP) != !r_textures && !r_hud_translucency ? -1 : nearestdarkgray);
}

void C_UpdateFPSOverlay(void)
{
    return;

}

void C_UpdateTimerOverlay(void)
{
    return;

}

void C_UpdatePlayerPositionOverlay(void)
{
    return;

}

void C_UpdatePathOverlay(void)
{
    return;

}

void C_UpdatePlayerStatsOverlay(void)
{
    return;

}

static bool IsCheatSequence(char *string)
{
    const int   length = (int)strlen(string);

    if (M_StringStartsWith(string, cheat_clev_xy.sequence)
        && length == strlen(cheat_clev_xy.sequence) + cheat_clev_xy.parameter_chars
        && isdigit(string[length - 1])
        && isdigit(string[length - 2]))
    {
        static char lump[7];

        if (legacyofrust)
        {
            int ep = string[length - 2] - '0';
            int map = string[length - 1] - '0';

            if (ep <= 2 && map <= 7)
                map = (ep - 1) * 7 + map;
            else if (ep <= 2 && map == 8)
                map = 14 + ep;
            else
                map = ep * 10 + map;

            M_snprintf(lump, sizeof(lump), "MAP%02i", map);
        }
        else if (gamemode == commercial)
            M_snprintf(lump, sizeof(lump), "MAP%c%c", string[length - 2], string[length - 1]);
        else
            M_snprintf(lump, sizeof(lump), "E%cM%c", string[length - 2], string[length - 1]);

        return (W_CheckNumForName(lump) >= 0);
    }
    else if (M_StringStartsWith(string, cheat_mus_xy.sequence)
        && length == strlen(cheat_mus_xy.sequence) + cheat_mus_xy.parameter_chars
        && isdigit(string[length - 1])
        && isdigit(string[length - 2]))
        return (!nomusic && musicvolume);
    else if (gamestate != GS_LEVEL)
        return false;
    else if (M_StringCompare(string, cheat_god.sequence))
        return (gameskill != sk_nightmare);
    else if (M_StringCompare(string, cheat_ammonokey.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_ammo.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_noclip.sequence))
        return (gamemode != commercial && gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_commercial_noclip.sequence))
        return (gamemode == commercial && gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[0].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[1].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[2].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[3].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[4].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_powerup[5].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_choppers.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_buddha.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(string, cheat_mypos.sequence))
        return true;
    else if (M_StringCompare(string, cheat_amap.sequence))
        return (gameskill != sk_nightmare && (automapactive || mapwindow));

    return false;
}

void C_Drawer(void)
{
    return;

}

bool C_ExecuteInputString(const char *input)
{
    char    *string = M_StringDuplicate(input);
    char    *strings[255] = { "" };
    int     j = 0;
    bool    makesound = false;

    M_StripQuotes(string);
    strings[0] = strtok(string, ";");

    while (strings[j])
    {
        if (!C_ValidateInput(trimwhitespace(strings[j])))
            break;

        if (M_StringStartsWith(strings[j], "toggle"))
            makesound = true;

        strings[++j] = strtok(NULL, ";");
    }

    if (makesound)
        S_StartSound(NULL, sfx_swtchn);

    free(string);
    return true;
}

bool C_ValidateInput(char *input)
{
    const int   length = (int)strlen(input);

    for (int i = 0; *consolecmds[i].name; i++)
    {
        char    cmd[128] = "";

        if (consolecmds[i].type == CT_CHEAT)
        {
            if (consolecmds[i].parameters)
            {
                if (isdigit((int)input[length - 2]) && isdigit((int)input[length - 1]))
                {
                    consolecheatparm[0] = input[length - 2];
                    consolecheatparm[1] = input[length - 1];
                    consolecheatparm[2] = '\0';

                    M_StringCopy(cmd, input, sizeof(cmd));
                    cmd[length - 2] = '\0';

                    if (M_StringCompare(cmd, consolecmds[i].name)
                        && length == strlen(cmd) + 2
                        && consolecmds[i].func1(consolecmds[i].name, consolecheatparm))
                    {
                        if (gamestate == GS_LEVEL)
                            M_StringCopy(consolecheat, cmd, sizeof(consolecheat));

                        return true;
                    }
                }
            }
            else if (M_StringCompare(input, consolecmds[i].name)
                && consolecmds[i].func1(consolecmds[i].name, ""))
            {
                M_StringCopy(consolecheat, input, sizeof(consolecheat));
                return true;
            }
        }
        else
        {
            char    parms[128] = "";

            if (sscanf(input, "%127s %127[^\n]", cmd, parms) > 0)
            {
                char    *temp = M_StringDuplicate(parms);

                M_StripQuotes(temp);

                if ((M_StringCompare(cmd, consolecmds[i].name)
                    || M_StringCompare(cmd, consolecmds[i].altspelling)
                    || M_StringCompare(cmd, consolecmds[i].alternate))
                    && consolecmds[i].func1(consolecmds[i].name, temp)
                    && (consolecmds[i].parameters || !*temp))
                {
                    if (!executingalias && !resettingcvar && !togglingcvar && !parsingcfgfile)
                    {
                        if (temp[0] != '\0')
                            C_Input((input[length - 1] == '%' ? "%s %s%" : "%s %s"), cmd, parms);
                        else
                            C_Input("%s%s", cmd, (input[length - 1] == ' ' ? " " : ""));
                    }

                    consolecmds[i].func2(consolecmds[i].name, temp);
                    free(temp);

                    return true;
                }

                free(temp);
            }
        }
    }

    if (C_ExecuteAlias(input))
        return true;

    for (int i = 0; *actions[i].action; i++)
        if (M_StringCompare(input, actions[i].action))
        {
            C_Input(input);

            if (actions[i].func)
            {
                if (consoleactive && actions[i].hideconsole)
                    C_HideConsoleFast();

                actions[i].func();
            }

            return true;
        }

    return false;
}

bool C_Responder(event_t *ev)
{
    return false;

}

void C_PrintCompileDate(void)
{
    return;

    char    shortmonthname[4] = "";
    int     minute, hour, day, year;

    if (sscanf(__DATE__, "%3s %2d %4d", shortmonthname, &day, &year) == 3
        && sscanf(__TIME__, "%02d:%02d:%*d", &hour, &minute) == 2)
    {
        const char  shortmonthnames[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
        const int   month = (int)(strstr(shortmonthnames, shortmonthname) - shortmonthnames) / 3;

        C_Output("This %i-bit " ITALICS("%s") " app of " ITALICS("%s")
            " was built with love by %s at %i:%02i%s on %s, %s %i, %i somewhere in %s.",
            8 * (int)sizeof(intptr_t), WINDOWS, DOOMRETRO_NAMEANDVERSIONSTRING, DOOMRETRO_CREATOR,
            (hour ? hour - 12 * (hour > 12) : 12), minute, (hour < 12 ? "(AM)" : "(PM)"),
            dayofweek(day, month + 1, year), monthnames[month], day, year, DOOMRETRO_HOMEOFCREATOR);
    }

#if   defined(_MSC_FULL_VER) && defined(_MSC_BUILD)
    if (_MSC_BUILD)
        C_Output("It was compiled using v%i.%02i.%i.%i of the " ITALICS("Microsoft C/C++ Optimizing Compiler."),
            _MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, _MSC_FULL_VER % 100000, _MSC_BUILD);
    else
        C_Output("It was compiled using v%i.%02i.%i of the " ITALICS("Microsoft C/C++ Optimizing Compiler."),
            _MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, _MSC_FULL_VER % 100000);
#endif
}

void C_PrintSDLVersions(void)
{
    return;

    C_Output(ITALICS("%s") " is using v%i.%i.%i of the " ITALICS("SDL (Simple DirectMedia Layer)") " library.",
        DOOMRETRO_NAME, SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

    C_Output("It is also using v%i.%i.%i of the " ITALICS("SDL_mixer")
        " library and v%i.%i.%i of the " ITALICS("SDL_image") " library.",
        SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL,
        SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_PATCHLEVEL);
}
