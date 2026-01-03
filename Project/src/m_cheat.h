

#pragma once

//
// CHEAT SEQUENCE PACKAGE
//

// declaring a cheat
#define CHEAT(value, parameters, longtimeout)   { value, parameters, longtimeout, 0, 0, "", 0 }

typedef struct
{
    // settings for this cheat
    char    *sequence;
    int     parameter_chars;
    bool    longtimeout;

    // state used during the game
    size_t  chars_read;
    int     param_chars_read;
    char    parameter_buf[5];

    int     timeout;

    bool    movekey;
} cheatseq_t;

bool cht_CheckCheat(cheatseq_t *cht, unsigned char key);

void cht_GetParam(const cheatseq_t *cht, char *buffer);

extern char         cheatkey;

extern cheatseq_t   cheat_mus;
extern cheatseq_t   cheat_mus_xy;
extern cheatseq_t   cheat_god;
extern cheatseq_t   cheat_ammo;
extern cheatseq_t   cheat_ammonokey;
extern cheatseq_t   cheat_noclip;
extern cheatseq_t   cheat_commercial_noclip;
extern cheatseq_t   cheat_powerup[7];
extern cheatseq_t   cheat_choppers;
extern cheatseq_t   cheat_buddha;
extern cheatseq_t   cheat_clev;
extern cheatseq_t   cheat_clev_xy;
extern cheatseq_t   cheat_mypos;
extern cheatseq_t   cheat_amap;
