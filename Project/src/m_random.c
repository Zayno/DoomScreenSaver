

#include "m_fixed.h"
#include "m_random.h"
#include "tables.h"

// ============================================================================
// RNG STATE VARIABLES
// ============================================================================

// Gameplay RNG index - uses lookup table, starts at 0 like original DOOM
// This MUST be saved/restored for demo compatibility
unsigned int prndindex = 0;

// Visual RNG state - separate from gameplay, uses LCG
// Does NOT need to be saved for demos
unsigned int visual_rng_state = 12345;

// ============================================================================
// RNG STATE MANAGEMENT
// ============================================================================

//
// M_ClearRandom
// Reset gameplay RNG to initial state.
// Call this at level start for deterministic demo playback.
//
void M_ClearRandom(void)
{
    //prndindex = 0;
    // Note: We don't reset visual_rng_state as it doesn't affect gameplay
}


//
// M_SetRNGState
// Restore gameplay RNG state when loading demos/savegames.
//
void M_SetRNGState(unsigned int state)
{
    prndindex = state & 255;
}

// ============================================================================
// MBF21 RANDOM FUNCTIONS
// ============================================================================

//
// P_RandomHitscanAngle
// Outputs a random angle between (-spread, spread), as an int.
//  spread: Maximum angle (degrees, in fixed point -- not BAM!)
//
int P_RandomHitscanAngle(const fixed_t spread)
{
    return (int)(((int64_t)FixedToAngle(ABS(spread)) * M_SubRandom()) / 255);
}

//
// P_RandomHitscanSlope
// Outputs a random angle between (-spread, spread), converted to slope values.
//  spread: Maximum vertical angle (degrees, in fixed point -- not BAM!)
//
int P_RandomHitscanSlope(const fixed_t spread)
{
    const int angle = P_RandomHitscanAngle(spread);

    return finetangent[(angle > ANG90 ? 0 : 
           (-angle > ANG90 ? FINEANGLES / 2 - 1 : 
           (ANG90 - angle) >> ANGLETOFINESHIFT))];
}
