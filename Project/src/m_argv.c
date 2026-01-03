

#include <ctype.h>

#include "i_system.h"
#include "m_misc.h"
#include "w_file.h"

int     myargc;
char    **myargv;


extern int Registry_AudioEnabled;
extern int Registry_MusicLevel;
extern int Registry_SFXLevel;

//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
//
int M_CheckParmWithArgs(const char *check, int start)
{
    for (int i = start; i < myargc - 1; i++)
        if (M_StringCompare(check, myargv[i]))
            return i;

    return 0;
}

int M_CheckParmsWithArgs(const char *check1, const char *check2, const char *check3, int start)
{
    for (int i = start; i < myargc - 1; i++)
        if ((*check1 && M_StringCompare(check1, myargv[i]))
            || (*check2 && M_StringCompare(check2, myargv[i]))
            || (*check3 && M_StringCompare(check3, myargv[i])))
            return i;

    return 0;
}

int M_CheckParm(const char* check)
{

	if (M_StringCompare(check, "-nosound"))
	{
		return (Registry_AudioEnabled == 0);
	}

    if (M_StringCompare(check, "-nomusic"))
    {
        return (Registry_MusicLevel == 0);
    }

    if (M_StringCompare(check, "-nosfx"))
    {
        return (Registry_SFXLevel == 0);
    }


	for (int i = 1; i < myargc; i++)
		if (M_StringCompare(check, myargv[i]))
			return i;

	return 0;
}

#define MAXARGVS    100

static void LoadResponseFile(size_t argv_index, const char *filename)
{
    size_t  size;
    char    *infile;
    char    *file;
    char    **newargv;
    int     newargc;
    size_t  i = 0;

    // Read the response file into memory
    FILE    *handle = fopen(filename, "rb");

    if (!handle)
        return;

    size = W_FileLength(handle);

    // Read in the entire file
    // Allocate one byte extra - this is in case there is an argument
    // at the end of the response file, in which case a '\0' will be
    // needed.
    if (!(file = malloc(size + 1)))
        return;

    while (i < size)
        i += fread(file + i, 1, size - i, handle);

    fclose(handle);

    // Create new arguments list array
    if (!(newargv = malloc(sizeof(char *) * MAXARGVS)))
        return;

    newargc = 0;
    memset(newargv, 0, sizeof(char *) * MAXARGVS);

    // Copy all the arguments in the list up to the response file

    for (i = 0; i < argv_index; i++)
    {
        newargv[i] = myargv[i];
        myargv[i] = NULL;
        newargc++;
    }

    infile = file;
    i = 0;

    while (i < size)
    {
        // Skip past space characters to the next argument
        while (i < size && isspace(infile[i]))
            i++;

        if (i >= size)
            break;

        // If the next argument is enclosed in quote marks, treat
        // the contents as a single argument. This allows long filenames
        // to be specified.
        if (infile[i] == '\"')
        {
            char    *argstart;

            // Skip the first character (")
            argstart = &infile[++i];

            // Read all characters between quotes
            while (i < size && infile[i] != '\"' && infile[i] != '\n')
                i++;

            if (i >= size || infile[i] == '\n')
                I_Error("Quotes unclosed in response file '%s'", filename);

            // Cut off the string at the closing quote
            infile[i++] = '\0';
            newargv[newargc++] = M_StringDuplicate(argstart);
        }
        else
        {
            char    *argstart;

            // Read in the next argument until a space is reached
            argstart = &infile[i];

            while (i < size && !isspace(infile[i]))
                i++;

            // Cut off the end of the argument at the first space
            infile[i++] = '\0';
            newargv[newargc++] = M_StringDuplicate(argstart);
        }
    }

    // Add arguments following the response file argument
    for (i = argv_index + 1; i < (size_t)myargc; i++)
    {
        newargv[newargc++] = myargv[i];
        myargv[i] = NULL;
    }

    // Free any old strings in myargv which were not moved to newargv
    for (i = 0; i < (size_t)myargc; i++)
        if (myargv[i])
        {
            free(myargv[i]);
            myargv[i] = NULL;
        }

    free(myargv);
    myargv = newargv;
    myargc = newargc;

    free(file);
}

//
// Find a Response File
//
void M_FindResponseFile(void)
{
    for (int i = 1; i < myargc; i++)
        if (myargv[i][0] == '@')
            LoadResponseFile(i, myargv[i] + 1);
}
