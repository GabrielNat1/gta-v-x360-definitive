#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Nova posição da fábrica para início da missão
#define LESTER_FACTORY_X 706.73f
#define LESTER_FACTORY_Y -965.00f
#define LESTER_FACTORY_Z 30.0f

enum Buttons
{
	Button_A = 0xC1,
	Button_B = 0xC3
};

bool LesterBlipCreated = false;
bool NearLesterHeist = false;
bool LesterHeistStarted = false;
int WaitToCheckDistance = 0;
bool missionContinued = false;

void FloatingHelpText(char* text)
{
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void CheckLesterHeistProximity()
{
	int player = PLAYER_PED_ID();
	vector3 pos = GET_ENTITY_COORDS(player, 1);
	float dx = pos.x - LESTER_FACTORY_X;
	float dy = pos.y - LESTER_FACTORY_Y;
	float dz = pos.z - LESTER_FACTORY_Z;
	float dist = SQRT(dx*dx + dy*dy + dz*dz);
	NearLesterHeist = (dist < 3.0f); // 3 metros de distância
}

void LesterHeistMessage()
{
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Estudei o banco Fleeca e descobri uma chance de hackear e roubar tudo. Vamos planejar isso juntos!");
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", "Plano do Fleeca", 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);

	WAIT(2000);
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Venha até minha fábrica quando estiver pronto!");
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", "Fábrica", 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void StartLesterMissionScript()
{
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("[DEBUG] Solicitando script 'lester_mission_heist_01'...");
	_DRAW_NOTIFICATION(3000, 1);

	REQUEST_SCRIPT("lester_mission_heist_01");
	int timeout = 250; // 5 segundos (250*20ms)
	while (!HAS_SCRIPT_LOADED("lester_mission_heist_01") && timeout > 0)
	{
		WAIT(20);
		timeout--;
	}

	if (!HAS_SCRIPT_LOADED("lester_mission_heist_01")) {
		_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("[ERRO] Missão não carregou!");
		_DRAW_NOTIFICATION(5000, 1);
		return;
	}

	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("[DEBUG] Script 'lester_mission_heist_01' carregado. Iniciando...");
	_DRAW_NOTIFICATION(3000, 1);

	int scriptId = START_NEW_SCRIPT("lester_mission_heist_01", 1024);

	if (scriptId == 0) {
		_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("[ERRO] Falha ao iniciar missão!");
		_DRAW_NOTIFICATION(5000, 1);
		return;
	}

	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("[DEBUG] Missão iniciada! (scriptId > 0)");
	_DRAW_NOTIFICATION(3000, 1);

	SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_01");

	// Termina o loader após iniciar a missão
	TERMINATE_THIS_THREAD();
}

void LesterHeistLoop()
{
	CheckLesterHeistProximity();

	// Desenha marker tipo cone invertido no local enquanto não continuou
	if (!missionContinued) {
		vector3 markerPos = {LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z - 1.0f};
		vector3 zero = {0.0f, 0.0f, 0.0f};
		vector3 scale = {2.0f, 2.0f, 1.0f};
		RGBA color = {0, 255, 0, 120};
		DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
	}

	if (!missionContinued && NearLesterHeist)
	{
		FloatingHelpText("Aperte ~INPUT_FRONTEND_CANCEL~ para iniciar a missão");
		if (IS_CONTROL_JUST_PRESSED(0, Button_B))
		{
			if (LesterHeistStarted)
				return; // Protege contra múltiplos starts

			missionContinued = true;
			LesterHeistStarted = true;
			StartLesterMissionScript();
		}
	}
}

// Este arquivo deve ser o loader (blip, marker, botão, etc), NÃO o dispatcher!
// Main Loop
void main()
{
	// NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME(); // ❌ REMOVER

	// Envia as mensagens do Lester assim que o script inicia
	LesterHeistMessage();

	while (true)
	{
		LesterHeistLoop();
		WAIT(20);
	}
}