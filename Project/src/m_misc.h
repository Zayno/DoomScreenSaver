

#pragma once

#include <stdio.h>
#include <stdlib.h>


#include "doomtype.h"

extern const char   *daynames[7];
extern const char   *monthnames[12];

typedef enum
{
    personal,
    possessive,
    reflexive
} pronoun_t;

bool M_FileExists(const char *filename);
bool M_FolderExists(const char *folder);
char *M_ExtractFolder(const char *path);


// Returns the file system location where application resource files are located.
// On Windows and Linux, this is the folder in which doomretro.exe is located;
// on macOS, this is the Contents/Resources folder within the application bundle.
char *M_GetResourceFolder(void);

// Returns the file system location where generated application
// data (configuration files, logs, savegames etc.) should be saved.
// On Windows and Linux, this is the folder in which doomretro.exe is located;
// on macOS, this is ~/Library/Application Support/DOOM Retro/.
char *M_GetAppDataFolder(void);

char *M_GetExecutableFolder(void);
bool M_StrToInt(const char *str, int *result);
const char *M_StrCaseStr(const char *haystack, const char *needle);
void M_StringCopy(char *dest, const char *src, const size_t dest_size);
void M_CopyLumpName(char *dest, const char *src);
char *M_StringReplaceFirst(char *haystack, const char *needle, const char *replacement);
char *M_StringReplaceLast(char *haystack, const char *needle, const char *replacement);
void M_StringReplaceAll(char *haystack, const char *needle, const char *replacement, bool usecase);
char *M_TempFile(char *s);
char *M_StringJoin(const char *s, ...);
bool M_StringStartsWith(const char *s, const char *prefix);
bool M_StringEndsWith(const char *s, const char *suffix);
void M_vsnprintf(char *buf, int buf_len, const char *s, va_list args);
void M_snprintf(char *buf, int buf_len, const char *s, ...);
char *M_SubString(const char *str, size_t begin, size_t len);
char *M_StringDuplicate(const char *orig);
bool M_StringCompare(const char *str1, const char *str2);
char *uppercase(const char *str);
char *lowercase(char *str);
char *titlecase(const char *str);
char *sentencecase(const char *str);
bool isuppercase(const char *str);
bool islowercase(const char *str);
char *commify(int64_t value);
char *commifystring(const char *str);
char *commifystat(uint64_t value);
char *uncommify(const char *input);
bool wildcard(char *input, char *pattern);
int gcd(int a, int b);
int numspaces(const char *str);
char *removespaces(const char *input);
char *removenonalpha(const char *input);
char *trimwhitespace(char *input);
char *makevalidfilename(const char *input);
char *leafname(char *path);
char *removeext(const char *file);
bool isvowel(const char ch);
bool ispunctuation(const char ch);
bool isbreak(const char ch);
char *striptrailingzero(double value, int precision);
void M_StripQuotes(char *str);
void M_NormalizeSlashes(char *str);
const char *pronoun(const pronoun_t type);
void M_AmericanToBritishEnglish(char *string);
void M_BritishToAmericanEnglish(char *string);
void M_TranslateAutocomplete(void);
const char *dayofweek(int day, int month, int year);
