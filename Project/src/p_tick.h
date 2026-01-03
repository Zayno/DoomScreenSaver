

#pragma once

#if defined(__GNUG__)
#pragma interface
#endif

void P_Ticker(void);

void P_InitThinkers(void);
void P_AddThinker(thinker_t *thinker);
void P_RemoveThinker(thinker_t *thinker);
void P_RemoveThinkerDelayed(thinker_t *thinker);

void P_UpdateThinker(thinker_t *thinker);               // killough 08/29/98

void P_SetTarget(mobj_t **mop, mobj_t *targ);           // killough 11/98

// killough 08/29/98: threads of thinkers, for more efficient searches
// cph 01/13/02: for consistency with the main thinker list, keep objects
// pending deletion on a class list too
enum
{
    th_mobj,
    th_misc,
    th_delete,
    NUMTHCLASS,
    th_all = NUMTHCLASS
};

extern thinker_t    thinkers[];
