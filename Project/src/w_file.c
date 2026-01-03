

#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

wadfile_t *W_OpenFile(const char *path)
{
    wadfile_t   *result;
    FILE        *fstream = fopen(path, "rb");

    if (!fstream)
        return NULL;

    // Create a new wadfile_t to hold the file handle.
    result = Z_Malloc(sizeof(wadfile_t), PU_STATIC, NULL);
    result->fstream = fstream;

    return result;
}

void W_CloseFile(wadfile_t *wad)
{
    fclose(wad->fstream);
    Z_Free(wad);
}

// Read data from the specified position in the file into the
// provided buffer. Returns the number of bytes read.
size_t W_Read(wadfile_t *wad, unsigned int offset, void *buffer, size_t buffer_len)
{
    // Jump to the specified position in the file.
    fseek(wad->fstream, offset, SEEK_SET);

    // Read into the buffer.
    return fread(buffer, 1, buffer_len, wad->fstream);
}

bool W_WriteFile(char const *name, const void *source, size_t length)
{
    FILE    *fstream = fopen(name, "wb");

    if (!fstream)
        return false;

    length = (fwrite(source, 1, length, fstream) == length);
    fclose(fstream);

    if (!length)
        remove(name);

    return !!length;
}

size_t W_FileLength(FILE *handle)
{
    long savedpos;
    long length;

    // save the current position in the file
    savedpos = ftell(handle);

    // jump to the end and find the length
    fseek(handle, 0, SEEK_END);
    length = ftell(handle);

    // go back to the old location
    fseek(handle, savedpos, SEEK_SET);

    return length;
}
