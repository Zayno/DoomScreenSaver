

#pragma once

#include "doomtype.h"
#include "m_config.h"

#define MAXALIASES          256

#define DIVIDERSTRING       "----------------------------------------------------------------------------------------------------"

#define UNITSPERFOOT        16
#define FEETPERMETER        3.28084f
#define METERSPERKILOMETER  1000
#define FEETPERMILE         5280

typedef enum
{
    keyboardcontrol   = 1,
    mousecontrol      = 2,
    controllercontrol = 3
} controltype_t;

typedef struct
{
    char            *control;
    controltype_t   type;
    int             value;
} control_t;

typedef struct
{
    char    *action;
    bool    hideconsole;
    bool    automaponly;
    void    (*func)(void);
    void    *keyboard1;
    void    *keyboard2;
    void    *mouse1;
    void    *controller1;
    void    *controller2;
} action_t;

typedef enum
{
    CT_CCMD  = 1,
    CT_CVAR  = 2,
    CT_CHEAT = 3
} cmdtype_t;

enum
{
    CF_NONE         =    0,
    CF_BOOLEAN      =    1,
    CF_FLOAT        =    2,
    CF_INTEGER      =    4,
    CF_PERCENT      =    8,
    CF_STRING       =   16,
    CF_TIME         =   32,
    CF_OTHER        =   64,
    CF_READONLY     =  128,
    CF_MAPRESET     =  256,
    CF_NEXTMAP      =  512,
    CF_PISTOLSTART  = 1024
};

typedef struct
{
    char        *name;
    char        *altspelling;
    char        *alternate;
    bool        (*func1)(char *cmd, char *parms);
    void        (*func2)(char *cmd, char *parms);
    bool        parameters;
    cmdtype_t   type;
    int         flags;
    int         length;
    void        *variable;
    int         aliases;
    int         minimumvalue;
    int         maximumvalue;
    char        *format;
    char        *description;
    float       defaultnumber;
    char        *defaultstring;
} consolecmd_t;

typedef struct
{
    char    name[128];
    char    string[128];
} alias_t;

extern action_t         actions[];
extern const control_t  controls[];
extern consolecmd_t     consolecmds[];
extern alias_t          aliases[MAXALIASES];
extern bool             executingalias;
extern bool             healthcvar;
extern bool             massacre;
extern bool             nobindoutput;
extern bool             parsingcfgfile;
extern bool             quitcmd;
extern bool             resettingcvar;
extern bool             togglingcvar;
extern bool             togglingvanilla;
extern bool             vanilla;

void alias_func2(char *cmd, char *parms);
void bind_func2(char *cmd, char *parms);
void exec_func2(char *cmd, char *parms);

bool IsControlBound(const controltype_t type, const int control);
char *C_LookupAliasFromValue(const int value, const valuealiastype_t valuealiastype);
int C_GetIndex(const char *cmd);
bool C_ExecuteAlias(const char *alias);
char *C_DistanceTraveled(double feet, bool allowzero);
