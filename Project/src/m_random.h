

#pragma once

#include "m_fixed.h"

// ============================================================================
// DUAL RNG SYSTEM FOR DETERMINISTIC DEMO PLAYBACK
// ============================================================================
//
// GAMEPLAY RNG (prndindex):
//   - Uses classic DOOM lookup table approach
//   - MUST be used for ALL game logic: AI decisions, damage, movement, etc.
//   - State is saved/restored for demos
//   - Reset at level start
//
// VISUAL RNG (visual_rng_state):
//   - Linear congruential generator
//   - Use for cosmetic effects: blood positions, particle effects, fuzz, etc.
//   - Does NOT affect demo sync
//
// ============================================================================

// Gameplay RNG index (0-255) - saved/restored for demos
extern unsigned int prndindex;

// Visual RNG state - separate from gameplay
extern unsigned int visual_rng_state;

// Classic DOOM random table
static const unsigned char rndtable[] =
{
      0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66,  74,  21,
    211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36,  95, 110,  85,  48,
    212, 140, 211, 249,  22,  79, 200,  50,  28, 188,  52, 140, 202, 120,  68, 145,
     62,  70, 184, 190,  91, 197, 152, 224, 149, 104,  25, 178, 252, 182, 202, 182,
    141, 197,   4,  81, 181, 242, 145,  42,  39, 227, 156, 198, 225, 193, 219,  93,
    122, 175, 249,   0, 175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,
      2, 235,  25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113,
     94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75, 136, 156,
     11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196, 135, 106,  63, 197,
    195,  86,  96, 203, 113, 101, 170, 247, 181, 113,  80, 250, 108,   7, 255, 237,
    129, 226,  79, 107, 112, 166, 103, 241,  24, 223, 239, 120, 198,  58,  60,  82,
    128,   3, 184,  66, 143, 224, 145, 224,  81, 206, 163,  45,  63,  90, 168, 114,
     59,  33, 159,  95,  28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14,
    109, 226,  71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36,
     17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106, 197, 242,
     98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136, 120, 163, 236, 249
};

// ============================================================================
// RNG STATE MANAGEMENT - For demo recording/playback
// ============================================================================

// Reset gameplay RNG to initial state (call at level/game start)
void M_ClearRandom(void);

// Get current RNG state for saving in demos
unsigned int M_GetRNGState(void);


// ============================================================================
// GAMEPLAY RANDOM FUNCTIONS - Use these for game logic
// ============================================================================

// Core gameplay random (0-255)
#define P_Random()    (rndtable[(prndindex = (prndindex + 1) & 0xFF)])

// Standard gameplay random (0-255)
static inline int M_Random(void)
{
    return P_Random();
}

// Returns difference of two random values (-255 to 255)
static inline int M_SubRandom(void)
{
    const int r = P_Random();
    return (r - P_Random());
}

// Random integer in range [lower, upper]
static inline int M_RandomInt(const int lower, const int upper)
{
    return (P_Random() % (upper - lower + 1) + lower);
}

// Random integer in range, avoiding previous value
static inline int M_RandomIntNoRepeat(const int lower, const int upper, const int previous)
{
    int result;
    while ((result = P_Random() % (upper - lower + 1) + lower) == previous);
    return result;
}

// Extended range random using LCG on table value
static inline int M_BigRandom(void)
{
    return ((214013 * P_Random() + 2531011) >> 16);
}

static inline int M_BigSubRandom(void)
{
    return ((M_BigRandom() & 510) - 255);
}

static inline int M_BigRandomInt(const int lower, const int upper)
{
    return (M_BigRandom() % (upper - lower + 1) + lower);
}

static inline int M_BigRandomIntNoRepeat(const int lower, const int upper, const int previous)
{
    int result;
    while ((result = M_BigRandom() % (upper - lower + 1) + lower) == previous);
    return result;
}

// ============================================================================
// VISUAL/COSMETIC RANDOM FUNCTIONS - Use for non-gameplay effects
// These do NOT affect demo sync
// ============================================================================

// Visual random (0-255) - for cosmetic effects only
static inline int M_VisualRandom(void)
{
    visual_rng_state = 214013 * visual_rng_state + 2531011;
    return (visual_rng_state >> 16) & 255;
}

// Visual random integer in range
static inline int M_VisualRandomInt(const int lower, const int upper)
{
    return (M_VisualRandom() % (upper - lower + 1) + lower);
}

// Fuzz effect randoms - use visual RNG (cosmetic only)
static inline int M_Fuzz1RandomInt(const int lower, const int upper)
{
    return M_VisualRandomInt(lower, upper);
}

static inline int M_Fuzz2RandomInt(const int lower, const int upper)
{
    return M_VisualRandomInt(lower, upper);
}

// ============================================================================
// MBF21 FUNCTIONS
// ============================================================================

int P_RandomHitscanAngle(const fixed_t spread);
int P_RandomHitscanSlope(const fixed_t spread);
void M_SetRNGState(unsigned int state);
