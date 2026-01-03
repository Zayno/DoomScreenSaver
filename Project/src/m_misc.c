

#pragma warning( disable : 4091 )

#include <Windows.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <direct.h>
#endif


#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "c_console.h"
#include "i_system.h"
#include "m_config.h"
#include "m_misc.h"
#include "w_file.h"

const char *daynames[7] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

const char *monthnames[12] =
{
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"
};


// Check if a file exists
bool M_FileExists(const char *filename)
{
    FILE    *fstream = fopen(filename, "r");

    if (fstream)
    {
        fclose(fstream);
        return true;
    }

    return false;
}


// Check if a folder exists
bool M_FolderExists(const char *folder)
{
    struct stat status;

    return (!stat(folder, &status) && (status.st_mode & S_IFDIR));
}

// Safe string copy function that works like OpenBSD's strlcpy().
void M_StringCopy(char *dest, const char *src, const size_t dest_size)
{
    if (dest_size >= 1)
    {
        dest[dest_size - 1] = '\0';

        if (dest_size > 1)
            strncpy(dest, src, dest_size - 1);
    }
}

void M_CopyLumpName(char *dest, const char *src)
{
    for (int i = 0; i < 8; i++)
    {
        dest[i] = src[i];

        if (src[i] == '\0')
            break;
    }
}

char *M_ExtractFolder(const char *path)
{
    char    *pos;
    char    *folder;

    if (!*path)
        return "";

    folder = M_StringDuplicate(path);

    if ((pos = strrchr(folder, DIR_SEPARATOR)))
        *pos = '\0';

    return folder;
}

char *M_GetAppDataFolder(void)
{
    char    *executablefolder = M_GetExecutableFolder();

    return executablefolder;
}

char *M_GetResourceFolder(void)
{
    char    *executablefolder = M_GetExecutableFolder();

    // On Windows, load resources from the same folder as the executable.
    return executablefolder;
}

char *M_GetExecutableFolder(void)
{
    char    *folder = malloc(MAX_PATH);

    if (folder)
    {
        char    *pos;

        GetModuleFileName(NULL, folder, MAX_PATH);

        if ((pos = strrchr(folder, DIR_SEPARATOR)))
            *pos = '\0';
    }

    return folder;
}

char *M_TempFile(char *s)
{
    char    *tempdir;

    tempdir = getenv("TEMP");

    if (!tempdir)
        tempdir = ".";

    return M_StringJoin(tempdir, DIR_SEPARATOR_S, s, NULL);
}

// Return a newly-malloced string with all the strings given as arguments
// concatenated together.
char *M_StringJoin(const char *s, ...)
{
    char        *result;
    const char  *v;
    va_list     args;
    size_t      result_len = strlen(s) + 1;

    va_start(args, s);

    while (true)
    {
        if (!(v = va_arg(args, const char *)))
            break;

        result_len += strlen(v);
    }

    va_end(args);

    if (!(result = malloc(result_len)))
        return NULL;

    M_StringCopy(result, s, result_len);

    va_start(args, s);

    while (true)
    {
        if (!(v = va_arg(args, const char *)))
            break;

        strncat(result, v, result_len - strlen(result) - 1);
    }

    va_end(args);
    return result;
}

bool M_StrToInt(const char *str, int *result)
{
    return (sscanf(str, " 0x%2x", (unsigned int *)result) == 1
        || sscanf(str, " 0X%2x", (unsigned int *)result) == 1
        || sscanf(str, " 0%3o", (unsigned int *)result) == 1
        || sscanf(str, " %12d", result) == 1);
}

// Case-insensitive version of strstr()
const char *M_StrCaseStr(const char *haystack, const char *needle)
{
    const int   haystack_len = (int)strlen(haystack);
    const int   needle_len = (int)strlen(needle);
    int         len;

    if (haystack_len < needle_len)
        return NULL;

    len = haystack_len - needle_len;

    for (int i = 0; i <= len; i++)
        if (!strncasecmp(haystack + i, needle, needle_len))
            return (haystack + i);

    return NULL;
}

#if !defined(stristr)
static char *stristr(char *ch1, const char *ch2)
{
    char    *chN1 = M_StringDuplicate(ch1);
    char    *chN2 = M_StringDuplicate(ch2);
    char    *chRet = NULL;
    char    *chNdx = chN1;

    while (*chNdx)
    {
        *chNdx = (char)tolower(*chNdx);
        chNdx++;
    }

    chNdx = chN2;

    while (*chNdx)
    {
        *chNdx = (char)tolower(*chNdx);
        chNdx++;
    }

    if ((chNdx = strstr(chN1, chN2)))
        chRet = ch1 + (chNdx - chN1);

    free(chN1);
    free(chN2);

    return chRet;
}
#endif

// String replace function.
char *M_StringReplaceFirst(char *haystack, const char *needle, const char *replacement)
{
    static char buffer[4096];
    char        *p;

    if (!(p = stristr(haystack, (char *)needle)))
        return haystack;

    strncpy(buffer, haystack, p - haystack);
    buffer[p - haystack] = '\0';
    sprintf(buffer + (p - haystack), "%s%s", replacement, p + strlen(needle));

    return buffer;
}

#if !defined(strrstr)
char *strrstr(const char *haystack, const char *needle)
{
    char    *r = NULL;

    if (!needle[0])
        return (char *)haystack + strlen(haystack);

    while (true)
    {
        char    *p = strstr(haystack, needle);

        if (!p)
            return r;

        r = p;
        haystack = p + 1;
    }
}
#endif

char *M_StringReplaceLast(char *haystack, const char *needle, const char *replacement)
{
    static char buffer[4096];
    char        *p;

    if (!(p = strrstr(haystack, (char *)needle)))
        return haystack;

    strncpy(buffer, haystack, p - haystack);
    buffer[p - haystack] = '\0';
    sprintf(buffer + (p - haystack), "%s%s", replacement, p + strlen(needle));

    return buffer;
}

void M_StringReplaceAll(char *haystack, const char *needle, const char *replacement, bool usecase)
{
    char        buffer[1024] = "";
    char        *insert_point = &buffer[0];
    char        *temp = haystack;
    const int   needle_len = (int)strlen(needle);
    const int   repl_len = (int)strlen(replacement);

    while (true)
    {
        char    *p = (usecase ? strstr(temp, (char *)needle) : stristr(temp, (char *)needle));

        if (!p)
        {
            strcpy(insert_point, temp);
            break;
        }

        memcpy(insert_point, temp, p - temp);
        insert_point += p - temp;

        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        temp = p + needle_len;
    }

    strcpy(haystack, buffer);
}

// Safe version of strdup() that checks the string was successfully allocated.
char *M_StringDuplicate(const char *orig)
{
    char    *result = strdup(orig);

    if (!result)
        I_Error("Failed to duplicate string.");

    return result;
}

// Returns true if str1 and str2 are the same.
// (Case-insensitive, return value reverse of strcasecmp() to avoid confusion.
bool M_StringCompare(const char *str1, const char *str2)
{
    return !strcasecmp(str1, str2);
}

// Returns true if string begins with the specified prefix.
bool M_StringStartsWith(const char *s, const char *prefix)
{
    const size_t    len = strlen(prefix);

    return (strlen(s) >= len && !strncasecmp(s, prefix, len));
}

// Returns true if string ends with the specified suffix.
bool M_StringEndsWith(const char *s, const char *suffix)
{
    const size_t    len1 = strlen(s);
    const size_t    len2 = strlen(suffix);

    return (len1 >= len2 && M_StringCompare(s + len1 - len2, suffix));
}

// Safe, portable vsnprintf().
void M_vsnprintf(char *buf, int buf_len, const char *s, va_list args)
{
    if (buf_len >= 1)
    {
        // Windows (and other OSes?) have a vsnprintf() that doesn't always
        // append a trailing \0. So we must do it, and write into a buffer
        // that is one byte shorter; otherwise this function is unsafe.
        const int   result = vsnprintf(buf, buf_len, s, args);

        // If truncated, change the final char in the buffer to a \0.
        // A negative result indicates a truncated buffer on Windows.
        if (result < 0 || result >= buf_len)
            buf[buf_len - 1] = '\0';
    }
}

// Safe, portable snprintf().
void M_snprintf(char *buf, int buf_len, const char *s, ...)
{
    va_list args;

    va_start(args, s);
    M_vsnprintf(buf, buf_len, s, args);
    va_end(args);
}

#if !defined(strndup)
char *strndup(const char *s, size_t n)
{
    const size_t    len = strnlen(s, n);
    char            *new = malloc(len + 1);

    if (!new)
        return NULL;

    new[len] = '\0';
    return (char *)memcpy(new, s, len);
}
#endif

char *M_SubString(const char *str, size_t begin, size_t len)
{
    const size_t    length = strlen(str);

    if (!length || length < begin || length < begin + len)
        return 0;

    return strndup(str + begin, len);
}

char *uppercase(const char *str)
{
    char    *newstr;
    char    *p = newstr = M_StringDuplicate(str);

    while ((*p = toupper(*p)))
        p++;

    return newstr;
}

char *lowercase(char *str)
{
    for (char *p = str; *p; p++)
        *p = tolower(*p);

    return str;
}

char *titlecase(const char *str)
{
    char        *newstr = M_StringDuplicate(str);
    const int   len = (int)strlen(newstr);

    if (len > 0)
    {
        newstr[0] = toupper(newstr[0]);

        if (len > 1)
            for (int i = 1; i < len; i++)
                if ((newstr[i - 1] != '\'' || (i >= 2 && newstr[i - 2] == ' '))
                    && !isalnum((unsigned char)newstr[i - 1])
                    && isalnum((unsigned char)newstr[i]))
                    newstr[i] = toupper(newstr[i]);
    }

    return newstr;
}

char *sentencecase(const char *str)
{
    char    *newstr = M_StringDuplicate(str);

    if (newstr[0] != '\0')
        newstr[0] = toupper(newstr[0]);

    return newstr;
}

bool isuppercase(const char *str)
{
    const int   len = (int)strlen(str);

    for (int i = 0; i < len; i++)
        if (islower(str[i]))
            return false;

    return true;
}

bool islowercase(const char *str)
{
    const int   len = (int)strlen(str);

    for (int i = 0; i < len; i++)
        if (isupper(str[i]))
            return false;

    return true;
}

char *commify(int64_t value)
{
    char    result[64];

    M_snprintf(result, sizeof(result), "%" PRIi64, value);

    if (value <= -1000 || value >= 1000)
    {
        char    *pt;
        size_t  n;

        for (pt = result; *pt && *pt != '.'; pt++);

        n = result + sizeof(result) - pt;

        while (true)
            if ((pt -= 3) > result)
            {
                memmove(pt + 1, pt, n);
                *pt = ',';
                n += 4;
            }
            else
                break;
    }

    return M_StringDuplicate(result);
}

char *commifystring(const char *str)
{
    size_t  len = strlen(str);

    if (!len)
        return M_StringDuplicate("");

    bool        negative = (str[0] == '-');
    size_t      start = (negative ? 1 : 0);
    const char  *dot = strchr(str + start, '.');
    size_t      intlen = (dot ? (size_t)(dot - (str + start)) : len - start);
    size_t      commas = (intlen > 3 ? (intlen - 1) / 3 : 0);
    size_t      outlen = len + commas + 1;
    char        *out = (char *)I_Malloc(outlen);
    size_t      i = intlen;
    size_t      j = intlen + commas;

    out[j] = '\0';

    while (i > 0)
    {
        out[--j] = str[start + --i];

        if (i > 0 && (intlen - i) % 3 == 0)
            out[--j] = ',';
    }

    if (negative)
        out[0] = '-';

    if (dot)
        strcat(out, dot);

    return out;
}

char *commifystat(uint64_t value)
{
    char    result[64];

    M_snprintf(result, sizeof(result), "%" PRIu64, value);

    if (value >= 1000)
    {
        char    *pt;
        size_t  n;

        for (pt = result; *pt && *pt != '.'; pt++);

        n = result + sizeof(result) - pt;

        while (true)
            if ((pt -= 3) > result)
            {
                memmove(pt + 1, pt, n);
                *pt = ',';
                n += 4;
            }
            else
                break;
    }

    return M_StringDuplicate(result);
}

char *uncommify(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (*input != ',' || *(input + 1) == '\0')
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

bool wildcard(char *input, char *pattern)
{
    if (!*pattern || M_StringCompare(input, pattern))
        return true;

    for (int i = 0; pattern[i] != '\0'; i++)
    {
        if (pattern[i] == '?')
            continue;
        else if (pattern[i] == '*')
        {
            for (int j = i; input[j] != '\0'; j++)
                if (wildcard(input + j, pattern + i + 1))
                    return true;

            return false;
        }
        else if (pattern[i] != input[i])
            return false;
    }

    return false;
}

int gcd(int a, int b)
{
    return (!b ? a : gcd(b, a % b));
}

int numspaces(const char *str)
{
    int         result = 0;
    const int   len = (int)strlen(str);

    for (int i = 0; i < len; i++)
        result += (str[i] == ' ');

    return result;
}

char *removespaces(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (!isspace((unsigned char)*input))
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

char *removenonalpha(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (isalnum((unsigned char)*input))
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

char *trimwhitespace(char *input)
{
    char    *end;

    while (isspace((unsigned char)*input))
        input++;

    if (!*input)
        return input;

    end = input + strlen(input) - 1;

    while (end > input && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';

    return input;
}

char *makevalidfilename(const char *input)
{
    char        *newstr = M_StringDuplicate(input);
    const int   len = (int)strlen(newstr);

    for (int i = 0; i < len; i++)
        if (strchr("\\/:?\"<>|", newstr[i]))
            newstr[i] = ' ';

    return newstr;
}

char *leafname(char *path)
{
    char    cc;
    char    *ptr = path;

    do
    {
        cc = *ptr++;

        if (cc == '\\' || cc == '/')
            path = ptr;
    } while (cc);

    return path;
}

char *removeext(const char *file)
{
    char    *newstr = M_StringDuplicate(file);
    char    *lastdot = strrchr(newstr, '.');

    *lastdot = '\0';

    return newstr;
}

bool isvowel(const char ch)
{
    return !!strchr("aeiouAEIOU", ch);
}

bool ispunctuation(const char ch)
{
    return !!strchr(".!?", ch);
}

bool isbreak(const char ch)
{
    return !!strchr(" /\\-", ch);
}

char *striptrailingzero(double value, int precision)
{
    char    *result = malloc(100);

    if (result)
    {
        int len;

        M_snprintf(result, sizeof(result), "%.*f",
            (precision == 2 ? 2 : (value != floor(value))), value);
        len = (int)strlen(result);

        if (len >= 4 && result[len - 3] == '.' && result[len - 1] == '0')
            result[len - 1] = '\0';
    }

    return result;
}

void M_StripQuotes(char *str)
{
    int len = (int)strlen(str);

    if (len >= 2
        && (((str[0] == '"' || str[0] == '\x93')
            && (str[len - 1] == '"' || str[len - 1] == '\x94'))
            || ((str[0] == '\'' || str[0] == '\x91')
                && (str[len - 1] == '\'' || str[len - 1] == '\x92'))))
    {
        len -= 2;
        memmove(str, str + 1, len);
        str[len] = '\0';
    }
}

void M_NormalizeSlashes(char *str)
{
    char    *p;

    // Convert all slashes/backslashes to DIR_SEPARATOR
    for (p = str; *p; p++)
        if ((*p == '/' || *p == '\\') && *p != DIR_SEPARATOR)
            *p = DIR_SEPARATOR;

    // Remove trailing slashes
    while (p > str && *(--p) == DIR_SEPARATOR)
        *p = 0;

    // Collapse multiple slashes
    for (p = str; (*str++ = *p); )
        if (*p++ == DIR_SEPARATOR)
            while (*p == DIR_SEPARATOR)
                p++;
}

const char *pronoun(const pronoun_t type)
{
    if (type == personal)
        return (playergender == playergender_male ? "he" :
            (playergender == playergender_female ? "she" : "they"));
    else if (type == possessive)
        return (playergender == playergender_male ? "his" :
            (playergender == playergender_female ? "her" : "their"));
    else
        return (playergender == playergender_male ? "himself" :
            (playergender == playergender_female ? "herself" : "themselves"));
}

const char *words[][2] =
{
    { "agoniz",     "agonis"     }, { "airplane",   "aeroplane"  },
    { "analog",     "analogue"   }, { "armor",      "armour"     },
    { "artifact",   "artefact"   }, { "barreled",   "barrelled"  },
    { "behavior",   "behaviour"  }, { "caliber",    "calibre"    },
    { "centered",   "centred"    }, { "centering",  "centring"   },
    { "center",     "centre"     }, { "color",      "colour"     },
    { "defense",    "defence"    }, { "dialog",     "dialogue"   },
    { "disk",       "disc"       }, { "donut",      "doughnut"   },
    { "endeavor",   "endeavour"  }, { "favor",      "favour"     },
    { "fiber",      "fibre"      }, { "flavor",     "flavour"    },
    { "gray",       "grey"       }, { "harbor",     "harbour"    },
    { "honor",      "honour"     }, { "humor",      "humour"     },
    { "initializ",  "initialis"  }, { "inquir",     "enquir"     },
    { "jewelry",    "jewellery"  }, { "judgment",   "judgement"  },
    { "labor",      "labour"     }, { "license",    "licence"    },
    { "liter",      "litre"      }, { "meter",      "metre"      },
    { "neighbor",   "neighbour"  }, { "offense",    "offence"    },
    { "organiz",    "organis"    }, { "practice",   "practise"   },
    { "program",    "programme"  }, { "realiz",     "realis"     },
    { "randomiz",   "randomis"   }, { "recogniz",   "recognis"   },
    { "refueling",  "refuelling" }, { "rumor",      "rumour"     },
    { "savior",     "saviour"    }, { "savor",      "savour"     },
    { "skeptic",    "sceptic"    }, { "specializ",  "specialis"  },
    { "stabiliz",   "stabilis"   }, { "standardiz", "standardis" },
    { "synchroniz", "synchronis" }, { "theater",    "theatre"    },
    { "traveled",   "travelled"  }, { "traveling",  "travelling" },
    { "utiliz",     "utilis"     }, { "vapor",      "vapour"     },
    { "whiskey",    "whisky"     }, { "yogurt",     "yoghurt"    },
    { "",           ""           }
};

static void M_Translate(char *string, const char *word1, const char *word2)
{
    return;

    char    *temp1 = M_StringDuplicate(word1);
    char    *temp2 = M_StringDuplicate(word2);

    M_StringReplaceAll(string, temp1, temp2, true);

    temp1[0] = toupper(temp1[0]);
    temp2[0] = toupper(temp2[0]);

    M_StringReplaceAll(string, temp1, temp2, true);

    free(temp1);
    free(temp2);

    temp1 = uppercase(word1);
    temp2 = uppercase(word2);

    M_StringReplaceAll(string, temp1, temp2, true);

    free(temp1);
    free(temp2);
}

void M_AmericanToBritishEnglish(char *string)
{
    for (int i = 0; *words[i][0]; i++)
        M_Translate(string, words[i][0], words[i][1]);
}

void M_BritishToAmericanEnglish(char *string)
{
    for (int i = 0; *words[i][0]; i++)
        M_Translate(string, words[i][1], words[i][0]);
}

void M_TranslateAutocomplete(void)
{
    return;

    if (english == english_american)
        for (int i = 0; *autocompletelist[i].text; i++)
            M_BritishToAmericanEnglish(autocompletelist[i].text);
    else
        for (int i = 0; *autocompletelist[i].text; i++)
            M_AmericanToBritishEnglish(autocompletelist[i].text);
}

const char *dayofweek(int day, int month, int year)
{
    const int   adjustment = (14 - month) / 12;

    month += 12 * adjustment - 2;
    year -= adjustment;

    return daynames[(day + (13 * month - 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7];
}
