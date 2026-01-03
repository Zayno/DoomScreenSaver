

#include "c_console.h"
#include "m_misc.h"
#include "sc_man.h"
#include "w_wad.h"
#include "z_zone.h"

#define MAX_STRING_SIZE 1024
#define ASCII_COMMENT1  ';'
#define ASCII_COMMENT2  '/'
#define ASCII_QUOTE     '"'
#define ASCII_ESCAPE    '\\'

char            *sc_String;
float           sc_Number;
int             sc_Line;

static char     *ScriptBuffer;
static char     *ScriptPtr;
static char     *ScriptEndPtr;
static char     *ScriptLumpName;
static int      ScriptLumpNum;
static bool     sc_End;
static bool     ScriptOpen;
static bool     AlreadyGot;
static bool     SkipComma;

static void SC_ScriptError(void)
{
    char    *temp = commify(sc_Line);

    C_Warning(1, "Line %s in the " BOLD("%s") " lump couldn't be parsed.",
        temp, ScriptLumpName);
    free(temp);
}

void SC_Open(const int lumpnum)
{
    static char StringBuffer[MAX_STRING_SIZE];

    SC_Close();
    ScriptLumpNum = lumpnum;

    if (M_StringCompare((ScriptLumpName = M_StringDuplicate(lumpinfo[lumpnum]->name)), "BRGHTMPS"))
        SkipComma = true;

    ScriptBuffer = W_CacheLumpNum(ScriptLumpNum);
    ScriptPtr = ScriptBuffer;
    ScriptEndPtr = ScriptPtr + W_LumpLength(ScriptLumpNum);
    sc_Line = 1;
    sc_End = false;
    ScriptOpen = true;
    sc_String = StringBuffer;
    AlreadyGot = false;
}

void SC_Close(void)
{
    if (ScriptOpen)
    {
        if (ScriptLumpNum >= 0)
        {
            W_ReleaseLumpNum(ScriptLumpNum);
            free(ScriptLumpName);
        }
        else
            Z_Free(ScriptBuffer);

        SkipComma = false;
        ScriptOpen = false;
    }
}

bool SC_GetString(void)
{
    char    *text;
    bool    foundToken = false;

    if (AlreadyGot)
    {
        AlreadyGot = false;
        return true;
    }

    if (ScriptPtr >= ScriptEndPtr)
    {
        sc_End = true;
        return false;
    }

    while (!foundToken)
    {
        while (ScriptPtr < ScriptEndPtr
            && (*ScriptPtr <= 32 || *ScriptPtr == '{' || *ScriptPtr == '='
                || (*ScriptPtr == ',' && !SkipComma && *(ScriptPtr - 1) != '\'')))
            if (*ScriptPtr++ == '\n')
                sc_Line++;

        if (ScriptPtr >= ScriptEndPtr)
        {
            sc_End = true;
            return false;
        }

        if (*ScriptPtr != ASCII_COMMENT1
            && *ScriptPtr != ASCII_COMMENT2 && *(ScriptPtr + 1) != ASCII_COMMENT2)
            foundToken = true;
        else
        {
            while (*ScriptPtr++ != '\n')
                if (ScriptPtr >= ScriptEndPtr)
                {
                    sc_End = true;
                    return false;
                }

            sc_Line++;
        }
    }

    text = sc_String;

    if (*ScriptPtr == ASCII_QUOTE)
    {
        ScriptPtr++;

        while (*ScriptPtr != ASCII_QUOTE || *(ScriptPtr - 1) == ASCII_ESCAPE)
        {
            if (*ScriptPtr == ASCII_ESCAPE
                && (*(ScriptPtr + 1) == 'n' || *(ScriptPtr + 1) == 'N'))
            {
                ScriptPtr += 2;
                *text++ = '\n';
            }
            else if (*ScriptPtr != ASCII_ESCAPE)
                *text++ = *ScriptPtr++;
            else
                ScriptPtr++;

            if (ScriptPtr == ScriptEndPtr || text == &sc_String[MAX_STRING_SIZE - 1])
                break;
        }

        ScriptPtr++;
    }
    else
        while (*ScriptPtr > 32
            && *ScriptPtr != ASCII_COMMENT1
            && *ScriptPtr != ASCII_COMMENT2 && *(ScriptPtr + 1) != ASCII_COMMENT2)
        {
            if (*ScriptPtr == '}')
                ScriptPtr++;

            *text++ = *ScriptPtr++;

            if (*ScriptPtr == '{'
                || *ScriptPtr == '='
                || (*ScriptPtr == ',' && !SkipComma && *(ScriptPtr - 1) != '\''))
            {
                ScriptPtr++;
                break;
            }

            if (ScriptPtr == ScriptEndPtr || text == &sc_String[MAX_STRING_SIZE - 1])
                break;
        }

    *text = '\0';
    return true;
}

void SC_MustGetString(void)
{
    if (!SC_GetString())
        SC_ScriptError();
    else if (SC_Compare("\r"))
        SC_MustGetString();
}

bool SC_GetNumber(void)
{
    if (SC_GetString())
    {
        char    *stopstring;

        sc_Number = strtof(sc_String, &stopstring);
        return true;
    }
    else
        return false;
}

void SC_MustGetNumber(void)
{
    if (!SC_GetNumber())
        SC_ScriptError();
}

void SC_UnGet(void)
{
    AlreadyGot = true;
}

int SC_MatchString(char **strings)
{
    for (int i = 0; *strings; i++)
        if (SC_Compare(*strings++))
            return i;

    return -1;
}

bool SC_Compare(const char *text)
{
    return M_StringCompare(text, sc_String);
}
