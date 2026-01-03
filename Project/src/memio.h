

#pragma once

#include <stdbool.h>

typedef enum
{
    MODE_READ,
    MODE_WRITE
} memfile_mode_t;

typedef struct
{
    unsigned char   *buf;
    size_t          buflen;
    size_t          alloced;
    unsigned int    position;
    bool            readeof;
    bool            eof;
    memfile_mode_t  mode;
} MEMFILE;

typedef enum
{
    MEM_SEEK_SET,
    MEM_SEEK_CUR,
    MEM_SEEK_END
} mem_rel_t;

MEMFILE *mem_fopen_read(void *buf, size_t buflen);
size_t mem_fread(void *buf, size_t size, size_t nmemb, MEMFILE *stream);
MEMFILE *mem_fopen_write(void);
size_t mem_fwrite(const void *ptr, size_t size, size_t nmemb, MEMFILE *stream);
char *mem_fgets(char *str, int count, MEMFILE *stream);
int mem_fgetc(MEMFILE *stream);
void mem_get_buf(MEMFILE *stream, void **buf, size_t *buflen);
void mem_fclose(MEMFILE *stream);
long mem_ftell(MEMFILE *stream);
int mem_fseek(MEMFILE *stream, long position, mem_rel_t whence);
bool mem_feof(MEMFILE *stream);
