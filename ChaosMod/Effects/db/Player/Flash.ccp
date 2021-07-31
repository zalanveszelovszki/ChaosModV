#include <stdafx.h>

static void OnStop()
{
	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.f);
}

static void OnTick()
{
	Player player = PLAYER_ID();

	SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 4.f);
}

static RegisterEffect registerEffect(EFFECT_SUPERR_RUN, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Flash",
		.Id = "player_flash",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_FLASH }
	}
);
