

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memio.h"
#include "z_zone.h"

// Open a memory area for reading
MEMFILE *mem_fopen_read(void *buf, size_t buflen)
{
    MEMFILE *file = Z_Malloc(sizeof(MEMFILE), PU_STATIC, NULL);

    file->buf = (unsigned char *)buf;
    file->buflen = buflen;
    file->position = 0;
    file->readeof = false;
    file->eof = false;
    file->mode = MODE_READ;

    return file;
}

// Read bytes
size_t mem_fread(void *buf, size_t size, size_t nmemb, MEMFILE *stream)
{
    size_t  items = nmemb;

    if (stream->mode != MODE_READ)
        return -1;

    if (!size || !nmemb)
        return 0;

    // Trying to read more bytes than we have left?
    if (items * size > stream->buflen - stream->position)
    {
        if (stream->readeof)
            stream->eof = true;
        else
            stream->readeof = true;

        items = (stream->buflen - stream->position) / size;
    }

    // Copy bytes to buffer
    memcpy(buf, stream->buf + stream->position, items * size);

    // Update position
    stream->position += (unsigned int)(items * size);

    return items;
}

// Open a memory area for writing
MEMFILE *mem_fopen_write(void)
{
    MEMFILE *file = Z_Malloc(sizeof(MEMFILE), PU_STATIC, NULL);

    file->alloced = 1024;
    file->buf = Z_Malloc(file->alloced, PU_STATIC, NULL);
    file->buflen = 0;
    file->position = 0;
    file->readeof = false;
    file->eof = false;
    file->mode = MODE_WRITE;

    return file;
}

// Write bytes to stream
size_t mem_fwrite(const void *ptr, size_t size, size_t nmemb, MEMFILE *stream)
{
    size_t  bytes;

    if (stream->mode != MODE_WRITE)
        return -1;

    // More bytes than can fit in the buffer? If so, reallocate bigger.
    bytes = size * nmemb;

    while (stream->alloced - stream->position < bytes)
    {
        unsigned char   *newbuf = Z_Malloc(stream->alloced * 2, PU_STATIC, NULL);

        memcpy(newbuf, stream->buf, stream->alloced);
        Z_Free(stream->buf);
        stream->buf = newbuf;
        stream->alloced *= 2;
    }

    // Copy into buffer
    memcpy(stream->buf + stream->position, ptr, bytes);
    stream->position += (unsigned int)bytes;

    if (stream->position > stream->buflen)
        stream->buflen = stream->position;

    return nmemb;
}

char *mem_fgets(char *str, int count, MEMFILE *stream)
{
    int i;

    if (!str || count < 0)
        return NULL;

    for (i = 0; i < count - 1; i++)
    {
        byte    ch;

        if (mem_fread(&ch, 1, 1, stream) != 1)
        {
            if (mem_feof(stream))
                return NULL;

            break;
        }

        str[i] = ch;

        if (ch == '\0')
            return str;

        if (ch == '\n')
        {
            i++;
            break;
        }
    }

    str[i] = '\0';
    return str;
}

int mem_fgetc(MEMFILE *stream)
{
    byte    ch;

    if (mem_fread(&ch, 1, 1, stream) == 1)
        return (int)ch;

    return -1;  // EOF
}

void mem_get_buf(MEMFILE *stream, void **buf, size_t *buflen)
{
    *buf = stream->buf;
    *buflen = stream->buflen;
}

void mem_fclose(MEMFILE *stream)
{
    if (stream->mode == MODE_WRITE)
        Z_Free(stream->buf);

    Z_Free(stream);
}

long mem_ftell(MEMFILE *stream)
{
    return stream->position;
}

int mem_fseek(MEMFILE *stream, long position, mem_rel_t whence)
{
    unsigned int    newpos;

    switch (whence)
    {
        case MEM_SEEK_SET:
            newpos = (int)position;
            break;

        case MEM_SEEK_CUR:
            newpos = (int)(stream->position + position);
            break;

        case MEM_SEEK_END:
            newpos = (int)(stream->buflen + position);
            break;

        default:
            return -1;
    }

    if (newpos < stream->buflen)
    {
        stream->position = newpos;
        stream->readeof = false;
        stream->eof = false;
        return 0;
    }

    return -1;
}

bool mem_feof(MEMFILE *stream)
{
    return stream->eof;
}
