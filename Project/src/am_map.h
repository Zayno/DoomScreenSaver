

#pragma once

#include "d_event.h"
#include "m_fixed.h"

#define MAPBITS         12
#define FRACTOMAPBITS   (FRACBITS - MAPBITS)
#define NUMBREADCRUMBS  1024

typedef struct
{
    fixed_t x, y;
} mpoint_t;

// Called by main loop.
bool AM_Responder(const event_t *ev);

// Called by main loop.
void AM_Ticker(void);

// Called by main loop, called instead of view drawer if automap active.
void AM_Drawer(void);
void AM_ClearFB(void);

void AM_InitPixelSize(void);
void AM_Start(const bool mainwindow);
void AM_ClearMarks(void);
void AM_ToggleFollowMode(const bool value);
void AM_ToggleGrid(void);
void AM_AddMark(void);
void AM_TogglePath(const bool value);
void AM_ToggleRotateMode(const bool value);
void AM_ToggleZoomOut(void);
void AM_ToggleZoomIn(void);
void AM_ToggleMaxZoom(void);

// Called to force the automap to quit if the level is completed while it is up.
void AM_Stop(void);

void AM_SetAutomapSize(const int screensize);

void AM_Init(void);
void AM_SetColors(void);
void AM_GetGridSize(void);
void AM_DropBreadCrumb(void);

typedef struct
{
    mpoint_t    center;
    fixed_t     sin;
    fixed_t     cos;
    fixed_t     bbox[4];
    int         angle;
} am_frame_t;

extern int          lastlevel;
extern int          lastepisode;

extern mpoint_t     *mark;
extern int          nummarks;
extern int          maxmarks;

extern mpoint_t     *breadcrumb;
extern int          numbreadcrumbs;
extern int          maxbreadcrumbs;

extern am_frame_t   am_frame;
