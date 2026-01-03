#include "SDL_timer.h"
#include "doomdef.h"

extern int gametime;


uint64_t I_GetTimeMS(void)
{
	//To make it 100% deterministic, we use gametime (which is a frame counter) as global timing base. Wiaam Suleiman
    return gametime;

}

int MS_To_Tics(int MS)
{
	float temp = (float)MS / TICRATE;
    temp++;

    int res = (int)temp;
	return res;
}
