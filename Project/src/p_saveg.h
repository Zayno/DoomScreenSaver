

#pragma once

// maximum size of a savegame description
#define SAVESTRINGSIZE          256

#define SAVESTRINGPIXELWIDTH    186
#define VERSIONSIZE             24

enum
{
    tc_end,
    tc_mobj,
    tc_bloodsplat
};

enum
{
    tc_ceiling,
    tc_door,
    tc_floor,
    tc_plat,
    tc_flash,
    tc_strobe,
    tc_glow,
    tc_elevator,    // jff 02/22/98 new elevator type thinker
    tc_scroll,      // killough 03/07/98: new scroll effect thinker
    tc_pusher,      // phares 03/22/98: new push/pull effect thinker
    tc_fireflicker, // killough 10/04/98
    tc_button,
    tc_endspecials
};

// temporary filename to use while saving
char *P_TempSaveGameFile(void);

// filename to use for a savegame slot
char *P_SaveGameFile(int slot);

// Savegame file header read/write functions
bool P_ReadSaveGameHeader(char *description);
void P_WriteSaveGameHeader(const char *description);

// Savegame end-of-file read/write functions
bool P_ReadSaveGameEOF(void);
void P_WriteSaveGameEOF(void);

// Savegame file footer read/write functions
void P_ReadSaveGameFooter(void);
void P_WriteSaveGameFooter(void);

// Persistent storage/archiving.
// These are the load/save game routines.
void P_ArchivePlayer(void);
void P_UnarchivePlayer(void);
void P_ArchiveWorld(void);
void P_UnarchiveWorld(void);
void P_ArchiveThinkers(void);
void P_UnarchiveThinkers(void);
void P_ArchiveSpecials(void);
void P_UnarchiveSpecials(void);
void P_ArchiveMap(void);
void P_UnarchiveMap(void);

void P_RestoreTargets(void);

extern FILE *save_stream;
