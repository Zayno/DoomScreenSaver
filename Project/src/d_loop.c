

#include "d_main.h"
#include "doomstat.h"
#include "g_game.h"
#include "i_timer.h"
#include "m_config.h"
#include "m_menu.h"
#include "s_sound.h"


ticcmd_t    CurrentFrameTicCommands;

void TryRunTics(void)
{
	memset(&CurrentFrameTicCommands, 0, sizeof(ticcmd_t));

	I_StartTic();

	G_BuildTiccmd(&CurrentFrameTicCommands);

	if (advancetitle)
		D_DoAdvanceTitle();

	if (menuactive)
		M_Ticker();

	G_Ticker();
	gametime++;

	if (CurrentFrameTicCommands.buttons & BT_SPECIAL)
		CurrentFrameTicCommands.buttons = 0;

	S_UpdateSounds();

}
