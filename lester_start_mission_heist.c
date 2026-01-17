#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Lester's Factory Heist Point
#define LESTER_HEIST_X 707.85f
#define LESTER_HEIST_Y -966.62f
#define LESTER_HEIST_Z 30.40f

enum Buttons
{
	Button_A = 0xC1
};

bool LesterBlipCreated = false;
bool NearLesterHeist = false;
bool LesterHeistStarted = false;
bool LesterScriptStarted = false;
int WaitToCheckDistance = 0;

void FloatingHelpText(char* text)
{
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void CreateLesterBlip()
{
	if (!LesterBlipCreated)
	{
		Blip blip = ADD_BLIP_FOR_COORD(LESTER_HEIST_X, LESTER_HEIST_Y, LESTER_HEIST_Z);
		SET_BLIP_SPRITE(blip, 77); // Lester's Factory icon
		SET_BLIP_COLOUR(blip, 5);  // Yellow
		SET_BLIP_NAME_FROM_TEXT_FILE(blip, "Lester's Factory");
		LesterBlipCreated = true;
	}
}

void CheckLesterHeistProximity()
{
	int player = PLAYER_PED_ID();
	vector3 pos = GET_ENTITY_COORDS(player, 1);
	float dx = pos.x - LESTER_HEIST_X;
	float dy = pos.y - LESTER_HEIST_Y;
	float dist = SQRT(dx*dx + dy*dy);
	NearLesterHeist = (dist < 3.0f); // 3 metros de distância
}

void LesterHeistMessage()
{
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Estudei o banco Fleeca e descobri uma chance de hackear e roubar tudo. Encontre um cartão que pode ajudar a abrir a porta. Tenha cuidado!");
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", "Plano do Fleeca", 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void StartLesterMissionScript()
{
	if (!LesterScriptStarted)
	{
		REQUEST_SCRIPT("lester_mission_heist_1");
		while (!HAS_SCRIPT_LOADED("lester_mission_heist_1"))
		{
			WAIT(0);
		}
		START_NEW_SCRIPT("lester_mission_heist_1", 1024);
		SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_1");
		LesterScriptStarted = true;
		WaitToCheckDistance = 0;
	}
}

void LesterHeistLoop()
{
	CreateLesterBlip();
	CheckLesterHeistProximity();
	if (!LesterHeistStarted)
	{
		if (NearLesterHeist)
		{
			FloatingHelpText("Aperte ~INPUT_FRONTEND_A~ para começar o golpe do Fleeca!");
			if (IS_CONTROL_JUST_PRESSED(0, Button_A))
			{
				LesterHeistStarted = true;
				LesterHeistMessage();
				StartLesterMissionScript();
			}
		}
	}
	else if (LesterScriptStarted)
	{
		// Aguarda alguns segundos antes de começar a checar distância
		if (WaitToCheckDistance < 200) // ~4 segundos (200*20ms)
		{
			WaitToCheckDistance++;
		}
		else
		{
			int player = PLAYER_PED_ID();
			vector3 pos = GET_ENTITY_COORDS(player, 1);
			float dx = pos.x - LESTER_HEIST_X;
			float dy = pos.y - LESTER_HEIST_Y;
			float dist = SQRT(dx*dx + dy*dy);
			if (dist > 50.0f)
			{
				// Use the correct native for script termination in your SDK
				TERMINATE_THIS_THREAD();
			}
		}
	}
}

// Main Loop
void main()
{
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();
	while (true)
	{
		LesterHeistLoop();
		WAIT(20);
	}
}