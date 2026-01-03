

#pragma once

#include <stdio.h>

#include <io.h>
#include <sys/stat.h>
#include <direct.h>

#if !defined(MAX_PATH)
#define MAX_PATH    260
#endif

typedef struct
{
    FILE    *fstream;
    bool    freedoom;
    char    path[MAX_PATH];
    int     type;
} wadfile_t;

// Open the specified file. Returns a pointer to a new wadfile_t
// handle for the WAD file, or NULL if it could not be opened.
wadfile_t *W_OpenFile(const char *path);

// Close the specified WAD file.
void W_CloseFile(wadfile_t *wad);

// Read data from the specified file into the provided buffer. The
// data is read from the specified offset from the start of the file.
// Returns the number of bytes read.
size_t W_Read(wadfile_t *wad, unsigned int offset, void *buffer, size_t buffer_len);

bool W_WriteFile(char const *name, const void *source, size_t length);
size_t W_FileLength(FILE *handle);
