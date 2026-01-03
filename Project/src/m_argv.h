

#pragma once

extern int  myargc;
extern char **myargv;

// Returns the position of the given parameter
// in the arg list (0 if not found).
int M_CheckParm(const char *check);

// Same as M_CheckParm, but checks that an argument is available
// following the specified argument.
int M_CheckParmWithArgs(const char *check, int start);
int M_CheckParmsWithArgs(const char *check1, const char *check2, const char *check3, int start);

void M_FindResponseFile(void);
