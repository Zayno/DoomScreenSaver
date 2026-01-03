

#pragma once

// We are referring to patches.
#include "r_defs.h"

// font stuff
#define HU_MAXLINELENGTH    512

#define HU_ALTHUDMSGX       (OVERLAYTEXTX - 1)
#define HU_ALTHUDMSGY       17

//
// Typedefs of widgets
//

// Text Line widget
//  (parent of Scrolling Text and Input Text widgets)
typedef struct
{
    // left-justified position of scrolling text window
    int             x, y;

    patch_t         **f;                            // font
    int             sc;                             // start character
    char            l[HU_MAXLINELENGTH + 1];        // line of text
    int             len;                            // current line length
    int             width;                          // pixel width of line

    // whether this line needs to be updated
    int             needsupdate;
} hu_textline_t;

// Scrolling Text window widget
//  (child of Text Line widget)
typedef struct
{
    hu_textline_t   l;                              // text line to draw

    // pointer to bool stating whether to update window
    bool            *on;
    bool            laston;                         // last value of *->on.
} hu_stext_t;

//
// Widget creation, access, and update routines
//

//
// textline code
//

void HUlib_InitTextLine(hu_textline_t *t, int x, int y, patch_t **f, int sc);

// returns success
bool HUlib_AddCharToTextLine(hu_textline_t *t, char ch);

// draws text line
void HUlib_DrawAutomapTextLine(hu_textline_t *l, bool external);
void HUlib_DrawAltAutomapTextLine(hu_textline_t *l, bool external);

// erases text line
void HUlib_EraseTextLine(hu_textline_t *l);

//
// Scrolling Text window widget routines
//

void HUlib_InitSText(hu_stext_t *s, int x, int y, patch_t **font, int startchar, bool *on);

void HUlib_AddMessageToSText(hu_stext_t *s, const char *msg);

// draws stext
void HUlib_DrawSText(hu_stext_t *s, bool external);

// erases all stext lines
void HUlib_EraseSText(hu_stext_t *s);

extern void (*althudtextfunc)(int, int, byte *, patch_t *, bool, int, int, int, const byte *);

extern bool s_STSTR_BEHOLD2;
extern byte tempscreen[MAXSCREENAREA];
