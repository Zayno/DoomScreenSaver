

#pragma once

#include "doomtype.h"

extern char     *sc_String;
extern float    sc_Number;
extern int      sc_Line;

void SC_Open(const int lumpnum);
void SC_Close(void);
bool SC_GetString(void);
void SC_MustGetString(void);
bool SC_GetNumber(void);
void SC_MustGetNumber(void);
void SC_UnGet(void);
bool SC_Compare(const char *text);
int SC_MatchString(char **strings);
