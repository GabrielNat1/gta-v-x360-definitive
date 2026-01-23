#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// --- Mission 1: Lester Heist ---

// Mission states
enum MissionState {
	MISSION_INIT = 0,
	MISSION_APPROACH,
	MISSION_INSIDE,
	MISSION_SHOOTOUT,
	MISSION_GOT_BOX,
	MISSION_REINFORCEMENTS,
	MISSION_CLEAR_ENEMIES,
	MISSION_GPS_TO_FACTORY,
	MISSION_AT_FACTORY,
	MISSION_DELIVER_BOX,
	MISSION_LEAVE_FACTORY,
	MISSION_COMPLETE,
	MISSION_FAILED
};

enum MissionState missionState = MISSION_INIT; // Corrigido: adicionado 'enum'

// Mission locations
vector3 houseOutside = {1400.2178f, 3599.8728f, 35.0367f};
vector3 houseInside1 = {1393.3156f, 3602.5510f, 38.9419f};
vector3 houseInside2 = {1389.8719f, 3601.0034f, 38.9424f};
vector3 boxPos      = {1392.5600f, 3601.1020f, 38.9403f};
// Altere aqui para a nova posição de entrega:
vector3 factoryPos  = {706.18f, -961.12f, 30.0f}; 

// NPC/prop handles
int npcOutside = 0;
int npcInside1 = 0;
int npcInside2 = 0;
int boxProp = 0;
int reinfVehicles[4] = {0};
int reinfPeds[12] = {0};
int factoryBlip = 0;
int factoryBoxProp = 0;

// Control flags
bool npcsSpawned = false;
bool boxSpawned = false;
bool reinfSpawned = false;
bool boxTaken = false;
bool gpsEnabled = false;
bool atFactory = false;
bool delivered = false;
bool playerDead = false;
bool missionBlocked = false;

// Utility
float GetPlayerDistToV(vector3 pos) {
	int player = PLAYER_PED_ID();
	vector3 p = GET_ENTITY_COORDS(player, 1);
	float dx = p.x - pos.x;
	float dy = p.y - pos.y;
	float dz = p.z - pos.z;
	return SQRT(dx*dx + dy*dy + dz*dz);
}

bool PlayerNearV(vector3 pos, float dist) {
	return GetPlayerDistToV(pos) < dist;
}

void ShowLesterMsg(const char* msg, const char* title) {
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpText(char* text) {
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

// --- Spawning/Despawning ---

void SpawnMissionNPCsAndProps() {
	if (npcsSpawned) return;
	// Outside NPC: 
	npcOutside = CREATE_PED(26, GET_HASH_KEY("a_f_y_vinewood_04"), houseOutside, 90.0f, 1, 1);
	if (npcOutside) TASK_START_SCENARIO_IN_PLACE(npcOutside, "WORLD_HUMAN_SMOKING", 0, 1);
	// Inside NPCs
	npcInside1 = CREATE_PED(26, GET_HASH_KEY("a_m_m_hillbilly_01"), houseInside1, 180.0f, 1, 1);
	npcInside2 = CREATE_PED(26, GET_HASH_KEY("a_m_m_hillbilly_02"), houseInside2, 270.0f, 1, 1);
	// NPCs internos já agressivos
	int player = PLAYER_PED_ID();
	if (npcInside1) {
		GIVE_WEAPON_TO_PED(npcInside1, GET_HASH_KEY("WEAPON_PISTOL"), 60, 0, 1);
		TASK_COMBAT_PED(npcInside1, player, 0, 16);
	}
	if (npcInside2) {
		GIVE_WEAPON_TO_PED(npcInside2, GET_HASH_KEY("WEAPON_PISTOL"), 60, 0, 1);
		TASK_COMBAT_PED(npcInside2, player, 0, 16);
	}
	// Box prop (ajusta z para não flutuar)
	boxProp = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_box_wood01a"), boxPos.x, boxPos.y, boxPos.z - 0.25f, 1, 1, 0);
	SET_ENTITY_AS_MISSION_ENTITY(boxProp, 1, 1);
	SET_ENTITY_DYNAMIC(boxProp, 1); // Ativa física
	boxSpawned = true;
	npcsSpawned = true;
}

void DespawnMissionNPCsAndProps() {
	if (npcOutside) { DELETE_PED(&npcOutside); npcOutside = 0; }
	if (npcInside1) { DELETE_PED(&npcInside1); npcInside1 = 0; }
	if (npcInside2) { DELETE_PED(&npcInside2); npcInside2 = 0; }
	if (boxProp) { DELETE_OBJECT(&boxProp); boxProp = 0; }
	npcsSpawned = false;
	boxSpawned = false;
}

void SpawnReinforcements() {
	if (reinfSpawned) return;
	vector3 spawnPoints[4] = {
		{1390.0f, 3588.0f, 34.0f},
		{1408.0f, 3587.0f, 34.0f},
		{1410.0f, 3610.0f, 34.0f},
		{1385.0f, 3612.0f, 34.0f}
	};
	for (int i = 0; i < 4; ++i) {
		reinfVehicles[i] = CREATE_VEHICLE(GET_HASH_KEY("granger"), spawnPoints[i], 0.0f, 1, 1);
		for (int j = 0; j < 3; ++j) {
			int ped = CREATE_PED_INSIDE_VEHICLE(reinfVehicles[i], 26, GET_HASH_KEY("g_m_y_lost_01"), j, 1, 1);
			if (ped) {
				GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MICROSMG"), 120, 0, 1);
				reinfPeds[i*3+j] = ped;
			}
		}
	}
	reinfSpawned = true;
}

void CommandReinforcementsAttack() {
	for (int i = 0; i < 12; ++i) {
		if (reinfPeds[i]) {
			TASK_LEAVE_VEHICLE(reinfPeds[i], 0, 256);
			TASK_COMBAT_PED(reinfPeds[i], PLAYER_PED_ID(), 0, 16);
		}
	}
}

void DespawnReinforcements() {
	for (int i = 0; i < 12; ++i) {
		if (reinfPeds[i]) { DELETE_PED(&reinfPeds[i]); reinfPeds[i] = 0; }
	}
	for (int i = 0; i < 4; ++i) {
		if (reinfVehicles[i]) { DELETE_VEHICLE(&reinfVehicles[i]); reinfVehicles[i] = 0; }
	}
	reinfSpawned = false;
}

bool AllReinforcementsDead() {
	for (int i = 0; i < 12; ++i) {
		if (reinfPeds[i] && !IS_PED_DEAD_OR_DYING(reinfPeds[i], 1)) return false;
	}
	return true;
}

// --- Mission Logic ---

void ResetMission() {
	DespawnMissionNPCsAndProps();
	DespawnReinforcements();
	if (factoryBlip) { REMOVE_BLIP(&factoryBlip); factoryBlip = 0; }
	if (factoryBoxProp) { DELETE_OBJECT(&factoryBoxProp); factoryBoxProp = 0; }
	boxTaken = false;
	gpsEnabled = false;
	atFactory = false;
	delivered = false;
	playerDead = false;
	missionBlocked = false;
	missionState = MISSION_INIT;
}

void main()
{
	// Proteção contra múltiplas instâncias usando o nativo disponível
	if (_GET_NUMBER_OF_INSTANCES_OF_SCRIPT_WITH_NAME_HASH(GET_HASH_KEY("lester_mission_heist_01")) > 1)
	{
		TERMINATE_THIS_THREAD();
	}

	SET_MISSION_FLAG(1);
	WAIT(0); // Essencial para troca de contexto
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	// Mensagem inicial cortada
	ShowLesterMsg("Descobri um esquema de quadrilhas perto de Sandy Shores. Marquei um local aproximado no mapa.", "Atenção!");
	WAIT(2000);
	ShowLesterMsg("Roube a caixa, mas tome cuidado: a área é perigosa!", "Atenção!");

	// Cria blip aproximado (sem GPS)
	int houseBlip = ADD_BLIP_FOR_COORD(houseOutside.x, houseOutside.y, houseOutside.z);
	SET_BLIP_SPRITE(houseBlip, 51); // Alterado de 280 para 51
	SET_BLIP_COLOUR(houseBlip, 1);
	SET_BLIP_SCALE(houseBlip, 0.9f);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Local suspeito");
	END_TEXT_COMMAND_SET_BLIP_NAME(houseBlip);

	missionState = MISSION_APPROACH;

	while (true)
	{
		int player = PLAYER_PED_ID();

		// Reset mission if player dies
		if (IS_ENTITY_DEAD(player)) { // Corrigido: apenas um argumento
			playerDead = true;
			missionState = MISSION_FAILED;
		}

		switch (missionState) {
		case MISSION_INIT:
			// (Opcional: lógica de inicialização, se necessário)
			break;
		case MISSION_APPROACH:
			// Spawn NPCs/props only if player is close
			if (PlayerNearV(houseOutside, 80.0f)) {
				SpawnMissionNPCsAndProps();
			} else {
				DespawnMissionNPCsAndProps();
			}
			// Enter house triggers next state
			if (PlayerNearV(boxPos, 5.0f)) {
				missionState = MISSION_INSIDE;
			}
			break;

		case MISSION_INSIDE:
			// Draw marker on box
			if (boxSpawned && boxProp) {
				vector3 markerPos = {boxPos.x, boxPos.y, boxPos.z+0.5f};
				vector3 zero = {0.0f, 0.0f, 0.0f};
				vector3 scale = {0.4f, 0.4f, 0.4f};
				RGBA yellow = {255, 255, 0, 180};   
				DRAW_MARKER(1, markerPos, zero, zero, scale, yellow, 0, 0, 2, 0, 0, 0, 0);
			}
			// NPCs já estão agressivos no spawn, não precisa reatribuir aqui
			missionState = MISSION_SHOOTOUT;
			break;

		case MISSION_SHOOTOUT:
			// Allow player to pick up box if close and NPCs are dead
			if (boxSpawned && boxProp && PlayerNearV(boxPos, 1.5f) &&
				(!npcInside1 || IS_PED_DEAD_OR_DYING(npcInside1, 1)) &&
				(!npcInside2 || IS_PED_DEAD_OR_DYING(npcInside2, 1))) {
				FloatingHelpText("Pressione ~INPUT_FRONTEND_X~ para pegar a caixa");
				if (IS_CONTROL_JUST_PRESSED(0, 0xD8)) { // X
					boxTaken = true;
					DELETE_OBJECT(&boxProp); boxProp = 0;
					ShowLesterMsg("Boa, cara! Mas cuidado... aparentemente eles chamaram reforços.", "Atenção!");
					missionState = MISSION_REINFORCEMENTS;
				}
			}
			break;

		case MISSION_REINFORCEMENTS:
			// Spawn enemy vehicles and peds
			if (!reinfSpawned) {
				SpawnReinforcements();
				WAIT(1000);
				CommandReinforcementsAttack();
				missionState = MISSION_CLEAR_ENEMIES;
			}
			break;

		case MISSION_CLEAR_ENEMIES:
			// Wait until all reinforcements are dead
			if (AllReinforcementsDead()) {
				ShowLesterMsg("Boa, cara. Sempre acreditei em você. Agora volte para a fábrica e me entregue a caixa.", "Sucesso!");
				missionState = MISSION_GPS_TO_FACTORY;
			}
			break;

		case MISSION_GPS_TO_FACTORY:
			// Enable GPS/blip to factory
			if (!gpsEnabled) {
				factoryBlip = ADD_BLIP_FOR_COORD(factoryPos.x, factoryPos.y, factoryPos.z);
				SET_BLIP_SPRITE(factoryBlip, 473);
				SET_BLIP_COLOUR(factoryBlip, 2);
				SET_BLIP_ROUTE(factoryBlip, 1);
				BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
				ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Fábrica do Lester");
				END_TEXT_COMMAND_SET_BLIP_NAME(factoryBlip);
				gpsEnabled = true;
				missionBlocked = true; // Block other missions/events
			}
			if (PlayerNearV(factoryPos, 30.0f)) {
				missionState = MISSION_AT_FACTORY;
			}
			break;

		case MISSION_AT_FACTORY:
			// Draw marker for delivery
			{
				vector3 markerPos = {factoryPos.x, factoryPos.y, factoryPos.z+1.0f};
				vector3 zero = {0.0f, 0.0f, 0.0f};
				vector3 scale = {0.7f, 0.7f, 0.7f};
				RGBA blue = {0, 120, 255, 180};
				DRAW_MARKER(1, markerPos, zero, zero, scale, blue, 0, 0, 2, 0, 0, 0, 0);
			}
			if (PlayerNearV(factoryPos, 2.0f)) {
				FloatingHelpText("Pressione ~INPUT_FRONTEND_B~ para entregar a caixa");
				if (IS_CONTROL_JUST_PRESSED(0, 0xC3)) { // B
					// Drop box animation (simplificado)
					factoryBoxProp = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_box_wood01a"), factoryPos.x, factoryPos.y, factoryPos.z, 1, 1, 0);
					SET_ENTITY_AS_MISSION_ENTITY(factoryBoxProp, 1, 1);
					SET_ENTITY_DYNAMIC(factoryBoxProp, 1);
					FREEZE_ENTITY_POSITION(factoryBoxProp, 1); // Congela a caixa entregue
					delivered = true;
					missionState = MISSION_DELIVER_BOX;
				}
			}
			break;

		case MISSION_DELIVER_BOX:
			// Objetivo: sair do local
			FloatingHelpText("Saia do local");
			if (!PlayerNearV(factoryPos, 60.0f)) {
				missionState = MISSION_LEAVE_FACTORY;
			}
			break;

		case MISSION_LEAVE_FACTORY:
			ShowLesterMsg("Muito obrigado. Aguente firme, mais tarde te mandarei mensagem quando tiver novos avanços.", "Missão Completa");
			// Clean up
			if (factoryBlip) { REMOVE_BLIP(&factoryBlip); factoryBlip = 0; }
			if (factoryBoxProp) { DELETE_OBJECT(&factoryBoxProp); factoryBoxProp = 0; }
			missionBlocked = false;
			missionState = MISSION_COMPLETE;
			break;

		case MISSION_COMPLETE:
			ShowLesterMsg("Missão concluída.", "Sucesso");
			WAIT(3000);
			START_NEW_SCRIPT("lester_mission_wait_01", 1024);
			TERMINATE_THIS_THREAD();
			break;

		case MISSION_FAILED:
			ShowLesterMsg("Você morreu! Missão reiniciada.", "Falha");
			WAIT(2000);
			ResetMission();
			// Remove blips
			if (houseBlip) { REMOVE_BLIP(&houseBlip); houseBlip = 0; }
			// Recria blip inicial
			houseBlip = ADD_BLIP_FOR_COORD(houseOutside.x, houseOutside.y, houseOutside.z);
			SET_BLIP_SPRITE(houseBlip, 51); // Alterado de 280 para 51
			SET_BLIP_COLOUR(houseBlip, 1);
			SET_BLIP_SCALE(houseBlip, 0.9f);
			BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Local suspeito");
			END_TEXT_COMMAND_SET_BLIP_NAME(houseBlip);
			break;
		default:
			break;
		}

		WAIT(20);
	}
}
