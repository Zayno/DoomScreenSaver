

#pragma once

#if defined(__GNUG__)
#pragma interface
#endif

//
// Experimental stuff.
// To compile this as "ANSI C with classes"
//  we will need to handle the various
//  action functions cleanly.
//

// killough 11/98: convert back to C instead of C++
typedef void (*actionf_t)();

// Historically, "think_t" is yet another
//  function pointer to a routine to handle
//  an actor.
typedef actionf_t think_t;

// Doubly linked list of actors.
typedef struct thinker_s
{
    struct thinker_s    *prev;
    struct thinker_s    *next;
    think_t             function;

    // Next, previous thinkers in same class
    struct thinker_s    *cprev;
    struct thinker_s    *cnext;

    // killough 11/98: count of how many other objects reference
    // this one using pointers. Used for garbage collection.
    unsigned int        references;

    // [BH] active during menu
    bool                menu;
} thinker_t;
