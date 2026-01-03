

#include "m_random.h"
#include "v_video.h"

//
// SCREEN WIPE PACKAGE
//

static int      y[MAXWIDTH];
static short    src[MAXSCREENAREA];

// Separate RNG state for wipe effect - doesn't affect gameplay RNG
static unsigned int wipe_rng_state = 12345;

static int Wipe_Random(void)
{
    wipe_rng_state = 214013 * wipe_rng_state + 2531011;
    return (wipe_rng_state >> 16) & 0x7FFF;
}

static void Wipe_ShittyColMajorXform(short *dest)
{
    for (int yy = 0; yy < SCREENHEIGHT; yy++)
        for (int xx = 0; xx < SCREENWIDTH / 2; xx++)
            src[yy + xx * SCREENHEIGHT] = dest[yy * SCREENWIDTH / 2 + xx];

    memcpy(dest, src, SCREENAREA);
}

static void Wipe_InitMelt(void)
{
    // Reset wipe RNG to fixed seed for consistent visual appearance
    wipe_rng_state = 12345;
    
    // makes this wipe faster (in theory) to have stuff in column-major format
    Wipe_ShittyColMajorXform((short *)screens[2]);
    Wipe_ShittyColMajorXform((short *)screens[3]);

    // setup initial column positions (y < 0 => not ready to scroll yet)
    // Use separate wipe RNG instead of game RNG
    y[0] = y[1] = -(Wipe_Random() & 15);

    for (int i = 2; i < SCREENWIDTH - 1; i += 2)
        y[i] = y[i + 1] = BETWEEN(-15, y[i - 1] + Wipe_Random() % 3 - 1, 0);
}

static void Wipe_Melt(const int i, const int dy)
{
    short   *s = &((short *)screens[3])[i * SCREENHEIGHT + y[i]];
    short   *d = &((short *)screens[0])[y[i] * SCREENWIDTH / 2 + i];

    for (int j = 0, k = dy; k > 0; k--, j += SCREENWIDTH / 2)
        d[j] = *s++;

    y[i] += dy;
    s = &((short *)screens[2])[i * SCREENHEIGHT];
    d = &((short *)screens[0])[y[i] * SCREENWIDTH / 2 + i];

    for (int j = 0, k = SCREENHEIGHT - y[i]; k > 0; k--, j += SCREENWIDTH / 2)
        d[j] = *s++;
}

static bool Wipe_DoMelt(void)
{
    bool    done = true;

    for (int i = 0; i < SCREENWIDTH / 2; i++)
        if (y[i] < 0)
        {
            y[i]++;
            done = false;
        }
        else if (y[i] < 16)
        {
            Wipe_Melt(i, y[i] + 1);
            done = false;
        }
        else if (y[i] < SCREENHEIGHT)
        {
            Wipe_Melt(i, MIN(SCREENHEIGHT / 16, SCREENHEIGHT - y[i]));
            done = false;
        }

    return done;
}

void Wipe_StartScreen(void)
{
    memcpy(screens[2], screens[0], SCREENAREA);
}

void Wipe_EndScreen(void)
{
    memcpy(screens[3], screens[0], SCREENAREA);
    memcpy(screens[0], screens[2], SCREENAREA);
}

bool Wipe_ScreenWipe(void)
{
    // when false, stop the wipe
    static bool go;

    // initial stuff
    if (!go)
    {
        go = true;
        Wipe_InitMelt();
    }

    // do a piece of wipe-in
    if (Wipe_DoMelt())
        go = false;

    return !go;
}
