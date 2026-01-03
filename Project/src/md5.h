

#pragma once

#include <stdint.h>

#include "doomtype.h"

typedef struct
{
    uint32_t    buf[4];
    uint32_t    bytes[2];
    uint32_t    in[16];
} MD5Context;

void MD5Init(MD5Context *ctx);
void MD5Update(MD5Context *ctx, const byte *buf, unsigned int len);
void MD5Final(byte digest[16], MD5Context *ctx);
void MD5Transform(uint32_t buf[4], const uint32_t in[16]);
char *MD5(const char *filename);
