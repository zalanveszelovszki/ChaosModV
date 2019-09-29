#include "stdafx.h"
#include "Effects.h"
#include "Main.h"

std::array<Ped, 256> GetAllPeds()
{
	static std::array<Ped, 256> peds;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		peds.fill(0);
		worldGetAllPeds(peds.data(), 256);
	}

	return peds;
}

std::array<Vehicle, 256> GetAllVehs()
{
	static std::array<Vehicle, 256> vehs;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		vehs.fill(0);
		worldGetAllVehicles(vehs.data(), 256);
	}

	return vehs;
}

void LoadModel(Hash model)
{
	if (IS_MODEL_VALID(model))
	{
		REQUEST_MODEL(model);
		while (!HAS_MODEL_LOADED(model))
		{
			scriptWait(0);
		}
	}
}

void TeleportPlayer(float x, float y, float z, float heading)
{
	Ped playerPed = PLAYER_PED_ID();
	bool isInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	SET_ENTITY_COORDS(isInVeh ? playerVeh : playerPed, x, y, z, false, false, false, false);
	SET_ENTITY_HEADING(isInVeh ? playerVeh : playerPed, heading);
}

void CreateTempVehicle(Hash model, float x, float y, float z, float heading)
{
	LoadModel(model);
	Vehicle veh = CREATE_VEHICLE(model, x, y, z, heading, true, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(model);
	SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
}

void Effects::StartEffect(EffectType effectType)
{
	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	float playerHeading = GET_ENTITY_HEADING(playerPed);
	bool isPlayerInVeh = IS_PED_IN_ANY_VEHICLE(playerPed, false);
	Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	Vector3 playerVehPos = GET_ENTITY_COORDS(playerVeh, false);
	float playerVehHeading = GET_ENTITY_HEADING(playerVeh);

	if (Effect.at(effectType).IsTimed)
	{
		m_effectActive[effectType] = true;
	}

	switch (effectType)
	{
	case EFFECT_KILL:
		SET_ENTITY_HEALTH(playerPed, 0, 0);
		break;
	case EFFECT_PLUS_2_STARS:
		SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_5_STARS:
		SET_PLAYER_WANTED_LEVEL(player, 5, false);
		SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		break;
	case EFFECT_CLEAR_STARS:
		CLEAR_PLAYER_WANTED_LEVEL(player);
		break;
	case EFFECT_STRIP_WEAPONS:
		for (Ped ped : GetAllPeds())
		{
			if (ped)
			{
				REMOVE_ALL_PED_WEAPONS(ped, false);
			}
		}
		break;
	case EFFECT_GIVE_RPG:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_RPG"), 9999, false, true);
		break;
	case EFFECT_GIVE_MINIGUN:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_MINIGUN"), 9999, false, true);
		break;
	case EFFECT_GIVE_PARACHUTE:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PARACHUTE"), 9999, false, true);
		break;
	case EFFECT_GIVE_PISTOL:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_PISTOL"), 9999, false, true);
		break;
	case EFFECT_GIVE_TAZER:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_TAZER"), 9999, false, true);
		break;
	case EFFECT_GIVE_RAILGUN:
		GIVE_WEAPON_TO_PED(playerPed, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, false, true);
		break;
	case EFFECT_HEAL:
		SET_ENTITY_HEALTH(playerPed, GET_PED_MAX_HEALTH(playerPed), 0);
		break;
	case EFFECT_ARMOR:
		ADD_ARMOUR_TO_PED(playerPed, 200);
		break;
	case EFFECT_IGNITE:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_ENGINE_HEALTH(playerVeh, -1.f);
			SET_VEHICLE_PETROL_TANK_HEALTH(playerVeh, -1.f);
			SET_VEHICLE_BODY_HEALTH(playerVeh, -1.f);
		}
		else
		{
			START_ENTITY_FIRE(playerPed);
		}
		break;
	case EFFECT_ANGRY_JESUS:
		Hash modelHash;
		modelHash = -835930287;
		LoadModel(modelHash);
		Ped ped;
		ped = CREATE_PED(4, modelHash, playerPos.x, playerPos.y, playerPos.z, 0.f, true, false);
		GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RAILGUN"), 9999, true, true);
		TASK_COMBAT_PED(ped, playerPed, 0, 16);
		SET_MODEL_AS_NO_LONGER_NEEDED(modelHash);
		SET_PED_AS_NO_LONGER_NEEDED(&ped);
		break;
	case EFFECT_IGNITE_PEDS:
		for (Ped ped : GetAllPeds())
		{
			if (ped && !IS_PED_A_PLAYER(ped))
			{
				START_ENTITY_FIRE(ped);
			}
		}
		break;
	case EFFECT_EXPLODE_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh && veh != playerVeh)
			{
				EXPLODE_VEHICLE(veh, true, false);
			}
		}
		break;
	case EFFECT_PLAYER_LAUNCH:
		SET_ENTITY_VELOCITY(playerPed, .0f, .0f, 10000.f);
		break;
	case EFFECT_PLAYER_VEH_LAUNCH:
		if (isPlayerInVeh)
		{
			SET_ENTITY_VELOCITY(playerVeh, .0f, .0f, 10000.f);
		}
		break;
	case EFFECT_PLAYER_VEH_LOCK:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_DOORS_LOCKED(playerVeh, 4);
		}
		break;
	case EFFECT_KILL_ENGINE:
		if (isPlayerInVeh)
		{
			SET_VEHICLE_ENGINE_HEALTH(playerVeh, 0.f);
		}
		break;
	case EFFECT_TIME_MORNING:
		SET_CLOCK_TIME(8, 0, 0);
		break;
	case EFFECT_TIME_DAY:
		SET_CLOCK_TIME(12, 0, 0);
		break;
	case EFFECT_TIME_EVENING:
		SET_CLOCK_TIME(18, 0, 0);
		break;
	case EFFECT_TIME_NIGHT:
		SET_CLOCK_TIME(0, 0, 0);
		break;
	case EFFECT_WEATHER_SUNNY:
		SET_WEATHER_TYPE_NOW("CLEAR");
		break;
	case EFFECT_WEATHER_EXTRASUNNY:
		SET_WEATHER_TYPE_NOW("EXTRASUNNY");
		break;
	case EFFECT_WEATHER_RAINY:
		SET_WEATHER_TYPE_NOW("RAIN");
		break;
	case EFFECT_WEATHER_THUNDER:
		SET_WEATHER_TYPE_NOW("THUNDER");
		break;
	case EFFECT_WEATHER_FOGGY:
		SET_WEATHER_TYPE_NOW("FOGGY");
		break;
	case EFFECT_WEATHER_XMAS:
		SET_WEATHER_TYPE_NOW("XMAS");
		break;
	case EFFECT_TP_LSAIRPORT:
		TeleportPlayer(-1388.6f, -3111.61f, 13.94f, 329.3f);
		break;
	case EFFECT_TP_MAZETOWER:
		TeleportPlayer(-75.7f, -818.62f, 326.16f, 228.09f);
		break;
	case EFFECT_TP_FORTZANCUDO:
		TeleportPlayer(-2360.3f, 3244.83f, 92.9f, 150.23f);
		break;
	case EFFECT_TP_MOUNTCHILLIAD:
		TeleportPlayer(501.77f, 5604.85f, 797.91f, 174.7f);
		break;
	case EFFECT_SET_INTO_CLOSEST_VEH:
		if (isPlayerInVeh)
		{
			return;
		}
		Vehicle closestVeh;
		closestVeh = -1;
		float closestDist;
		closestDist = 9999.f;
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				Vector3 coords;
				coords = GET_ENTITY_COORDS(veh, false);
				float dist;
				dist = GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerPos.x, playerPos.y, playerPos.z, true);
				if (dist < closestDist)
				{
					closestVeh = veh;
					closestDist = dist;
				}
			}
		}
		if (closestVeh != -1)
		{
			SET_PED_INTO_VEHICLE(playerPed, closestVeh, -1);
		}
		break;
	case EFFECT_EXIT_CURRENT_VEH:
		if (isPlayerInVeh)
		{
			TASK_LEAVE_VEHICLE(playerPed, playerVeh, 4160);
		}
	case EFFECT_PEDS_RIOT:
		Hash groupHash;
		ADD_RELATIONSHIP_GROUP("_RIOT", &groupHash);
		break;
	case EFFECT_SPAWN_TANK:
		CreateTempVehicle(GET_HASH_KEY("RHINO"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_ADDER:
		CreateTempVehicle(GET_HASH_KEY("ADDER"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_DUMP:
		CreateTempVehicle(GET_HASH_KEY("DUMP"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_MONSTER:
		CreateTempVehicle(GET_HASH_KEY("MONSTER"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_BMX:
		CreateTempVehicle(GET_HASH_KEY("BMX"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_TUG:
		CreateTempVehicle(GET_HASH_KEY("TUG"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_SPAWN_CARGO:
		CreateTempVehicle(GET_HASH_KEY("CARGOPLANE"), playerPos.x, playerPos.y, playerPos.z, playerHeading);
		break;
	case EFFECT_EXPLODE_CUR_VEH:
		if (isPlayerInVeh)
		{
			EXPLODE_VEHICLE(playerVeh, true, false);
		}
	}
}

void Effects::StopEffect(EffectType effectType)
{
	if (Effect.at(effectType).IsTimed)
	{
		m_effectActive[effectType] = false;
	}

	switch (effectType)
	{
	case EFFECT_NO_PHONE:
		REQUEST_SCRIPT("cellphone_controller");
		while (!HAS_SCRIPT_LOADED("cellphone_controller"))
		{
			scriptWait(0);
		}
		START_NEW_SCRIPT("cellphone_controller", 1424); // START_NEW_SCRIPT
		SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED("cellphone_controller");
		break;
	case EFFECT_GAMESPEED_X02:
	case EFFECT_GAMESPEED_X05:
		SET_TIME_SCALE(1.f);
		break;
	case EFFECT_SLIPPERY_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_REDUCE_GRIP(veh, false);
			}
		}
		break;
	case EFFECT_NO_GRAV_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_GRAVITY(veh, true);
			}
		}
		break;
	case EFFECT_PLAYER_INVINCIBLE:
		SET_PLAYER_INVINCIBLE(PLAYER_ID(), false);
		break;
	case EFFECT_2XENGINE_VEHS:
	case EFFECT_10XENGINE_VEHS:
	case EFFECT_05XENGINE_VEHS:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 1.f);
				_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 1.f);
			}
		}
		break;
	case EFFECT_PEDS_INVISIBLE:
		for (Ped ped : GetAllPeds())
		{
			if (ped)
			{
				RESET_ENTITY_ALPHA(ped);
			}
		}
		break;
	case EFFECT_VEHS_INVISIBLE:
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				RESET_ENTITY_ALPHA(veh);
			}
		}
		break;
	case EFFECT_SUPER_RUN:
		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.f);
		break;
	}
}

void Effects::UpdateEffects()
{
	if (m_effectActive[EFFECT_NO_PHONE])
	{
		TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("cellphone_controller"); // TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME
	}
	if (m_effectActive[EFFECT_GAMESPEED_X02])
	{
		SET_TIME_SCALE(.2f);
	}
	if (m_effectActive[EFFECT_GAMESPEED_X05])
	{
		SET_TIME_SCALE(.5f);
	}
	if (m_effectActive[EFFECT_PEDS_RIOT])
	{
		static Hash riotGroupHash = GET_HASH_KEY("_RIOT");
		static Hash playerGroupHash = GET_HASH_KEY("PLAYER");
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, riotGroupHash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, playerGroupHash);
		SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, riotGroupHash);

		for (Ped ped : GetAllPeds())
		{
			if (ped && !IS_PED_A_PLAYER(ped))
			{
				SET_PED_RELATIONSHIP_GROUP_HASH(ped, riotGroupHash);
			}
		}
	}
	if (m_effectActive[EFFECT_RED_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 255, 0, 0);
				SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 255, 0, 0);
			}
		}
	}
	if (m_effectActive[EFFECT_BLUE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 0, 255);
				SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 0, 255);
			}
		}
	}
	if (m_effectActive[EFFECT_GREEN_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, 0, 255, 0);
				SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, 0, 255, 0);
			}
		}
	}
	if (m_effectActive[EFFECT_RAINBOW_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				static ULONG cnt = 0;
				static float freq = .01f;
				int r = (int)std::sin(freq * cnt) * 127 + 128;
				int g = (int)std::sin(freq * cnt + 2) * 127 + 128;
				int b = (int)std::sin(freq * cnt + 4) * 127 + 128;
				if (++cnt >= (ULONG)-1)
				{
					cnt = 0;
				}
				SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, r, g, b);
				SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, r, g, b);
			}
		}
	}
	if (m_effectActive[EFFECT_FORCED_FP])
	{
		SET_FOLLOW_PED_CAM_VIEW_MODE(4);
		SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(4);
	}
	if (m_effectActive[EFFECT_SLIPPERY_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			static bool toggle = true;
			if (veh)
			{
				SET_VEHICLE_REDUCE_GRIP(veh, toggle);
				toggle = !toggle;
			}
		}
	}
	if (m_effectActive[EFFECT_NO_GRAV_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_VEHICLE_GRAVITY(veh, false);
			}
		}
	}
	if (m_effectActive[EFFECT_PLAYER_INVINCIBLE])
	{
		SET_PLAYER_INVINCIBLE(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_2XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 2.f);
				_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 2.f);
			}
		}
	}
	if (m_effectActive[EFFECT_10XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 10.f);
				_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 10.f);
			}
		}
	}
	if (m_effectActive[EFFECT_05XENGINE_VEHS])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, .5f);
				_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, .5f);
			}
		}
	}
	if (m_effectActive[EFFECT_NEVER_WANTED])
	{
		SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
		SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
	}
	if (m_effectActive[EFFECT_NO_VEHS])
	{
		SET_AMBIENT_VEHICLE_RANGE_MULTIPLIER_THIS_FRAME(.0f);
		SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
	}
	if (m_effectActive[EFFECT_NO_PEDS])
	{
		SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f);
		SET_SCENARIO_PED_DENSITY_MULTIPLIER_THIS_FRAME(.0f, .0f);
	}
	if (m_effectActive[EFFECT_PEDS_INVISIBLE])
	{
		for (Ped ped : GetAllPeds())
		{
			if (ped)
			{
				SET_ENTITY_ALPHA(ped, 0, 0);
			}
		}
	}
	if (m_effectActive[EFFECT_VEHS_INVISIBLE])
	{
		for (Vehicle veh : GetAllVehs())
		{
			if (veh)
			{
				SET_ENTITY_ALPHA(veh, 0, 0);
			}
		}
	}
	if (m_effectActive[EFFECT_NO_RADAR])
	{
		_DISABLE_RADAR_THIS_FRAME();
	}
	if (m_effectActive[EFFECT_NO_HUD])
	{
		HIDE_HUD_AND_RADAR_THIS_FRAME();
	}
	if (m_effectActive[EFFECT_SUPER_RUN])
	{
		SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(PLAYER_ID(), 1.49f);
	}
	if (m_effectActive[EFFECT_SUPER_JUMP])
	{
		SET_SUPER_JUMP_THIS_FRAME(PLAYER_ID());
	}
}