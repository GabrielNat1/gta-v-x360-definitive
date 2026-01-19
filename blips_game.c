#include "types.h"
#include "natives.h"

void main()
{
	vector3 lojas[] = {
		{1964.0f, 3744.0f, 32.0f},
		{-51.0f, -1755.0f, 29.0f},
		{28.0f, -1346.0f, 29.0f},
		{1138.0f, -981.0f, 46.0f},
		{1159.0f, -323.0f, 69.0f},
		{376.0f, 325.0f, 103.0f},
		{-1225.0f, -904.0f, 12.0f},
		{-1489.0f, -382.0f, 40.0f},
		{2557.0f, 385.0f, 108.0f},
		{-1823.0f, 789.0f, 138.0f},
		{-3040.0f, 589.0f, 7.0f},
		{-2971.0f, 390.0f, 15.0f},
		{-3242.0f, 1004.0f, 12.0f},
		{544.0f, 2671.0f, 42.0f},
		{1166.0f, 2705.0f, 38.0f},
		{2680.0f, 3283.0f, 55.0f},
		{1700.0f, 4928.0f, 42.0f},
		{1731.0f, 6413.0f, 35.0f}
	};

	int numLojas = sizeof(lojas) / sizeof(lojas[0]);
	for (int i = 0; i < numLojas; ++i) {
		int blip = ADD_BLIP_FOR_COORD(lojas[i].x, lojas[i].y, lojas[i].z);
		SET_BLIP_SPRITE(blip, 52); // Blip de loja de conveniência
		SET_BLIP_SCALE(blip, 0.8f);
		SET_BLIP_COLOUR(blip, 2);
		BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Loja de conveniência");
		END_TEXT_COMMAND_SET_BLIP_NAME(blip);
	}

	// Blip 252 (Cela)
	vector3 celaPolicia = {468.0f, -1014.0f, 3.0f};
	int blipCela = ADD_BLIP_FOR_COORD(celaPolicia.x, celaPolicia.y, celaPolicia.z);
	SET_BLIP_SPRITE(blipCela, 252);
	SET_BLIP_SCALE(blipCela, 0.9f);
	SET_BLIP_COLOUR(blipCela, 3);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Cela da Polícia");
	END_TEXT_COMMAND_SET_BLIP_NAME(blipCela);

	// Blip 207 (Banco Central)
	vector3 bancoCentral = {234.0f, 216.0f, 106.0f};
	int blipBanco = ADD_BLIP_FOR_COORD(bancoCentral.x, bancoCentral.y, bancoCentral.z);
	SET_BLIP_SPRITE(blipBanco, 207);
	SET_BLIP_SCALE(blipBanco, 1.0f);
	SET_BLIP_COLOUR(blipBanco, 5);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Banco Central");
	END_TEXT_COMMAND_SET_BLIP_NAME(blipBanco);

	while (1)
	{
		WAIT(1000);
	}
}
