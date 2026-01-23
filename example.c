// ============================================
// Lester Heist Script
// Autor: hibridslayer 
// TODO:
// - Script for heist fleeca
// - x360 script
// ============================================

#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

enum LesterHeistStatesScript01 {
	STATE_INIT_SCRIPT01, // start mission - mission wait for player start
	STATE_INIT_SCRIPT02, // start mission 01
	STATE_INIT_SCRIPT03, // start wait mission 01
	STATE_INIT_SCRIPT04, // start mission 02
	STATE_INIT_SCRIPT05, // start wait mission 02
	STATE_INIT_SCRIPT06,  // start final mission
	STATE_COMPLETE_SCRIPTS // final state - end scripts
};

enum LesterSequence {
	SEQ_NONE = 0,
	SEQ_SCRIPT01,
	SEQ_SCRIPT02,
	SEQ_SCRIPT03,
	SEQ_SCRIPT04,
	SEQ_SCRIPT05,
	SEQ_SCRIPT06,
	SEQ_SCRIPT06_FINAL
};

int g_CurrentSequence = SEQ_SCRIPT01;

bool script01Started = false;
bool script01Finished = false;
bool script02Started = false;
bool script02Finished = false;
bool script03Started = false;
bool script03Finished = false;
bool script04Started = false;
bool script04Finished = false;
bool script05Started = false;
bool script05Finished = false;
bool script06Started = false;
bool script06Finished = false;
bool script06FinalStarted = false;
bool script06FinalFinished = false;

// -------------------------------------
// inicio script 01
// -------------------------------------

#define LESTER_FACTORY_X_SCRIPT01 706.73f
#define LESTER_FACTORY_Y_SCRIPT01 -965.00f
#define LESTER_FACTORY_Z_SCRIPT01 30.0f

enum ButtonsScript01{
	Button_A = 0xC1,
	Button_B = 0xC3,
	Button_X = 0xC2
};

bool LesterBlipCreatedScript01 = false;
bool NearLesterHeistScript01 = false;
bool LesterHeistStartedScript01 = false;
int WaitToCheckDistanceScript01 = 0;
bool missionContinuedScript01 = false;

void FloatingHelpText(const char* text)
{
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void CheckLesterHeistProximityScript01()
{
	int player = PLAYER_PED_ID();
	vector3 pos = GET_ENTITY_COORDS(player, 1);
	float dx = pos.x - LESTER_FACTORY_X_SCRIPT01;
	float dy = pos.y - LESTER_FACTORY_Y_SCRIPT01;
	float dz = pos.z - LESTER_FACTORY_Z_SCRIPT01;
	float dist = SQRT(dx*dx + dy*dy + dz*dz);
	NearLesterHeistScript01 = (dist < 3.0f); // 3metros de distancia
}

void LesterHeistMessageScript01()
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

// corrigir aqui depois
void StartScript01()
{
	// load script and start it
	if (!script01Started) {
        script01Started = true;
        LesterHeistMessageScript01(); 
    }
}

void LesterHeistLoopScript01()
{
	CheckLesterHeistProximityScript01();

	if (!missionContinuedScript01) {
		vector3 markerPos = {LESTER_FACTORY_X_SCRIPT01, LESTER_FACTORY_Y_SCRIPT01, LESTER_FACTORY_Z_SCRIPT01 - 1.0f};
		vector3 zero = {0.0f, 0.0f, 0.0f};
		vector3 scale = {2.0f, 2.0f, 1.0f};
		RGBA color = {0, 255, 0, 120};
		DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
	}

	if (!missionContinuedScript01 && NearLesterHeistScript01)
	{
		FloatingHelpText("Aperte ~INPUT_FRONTEND_CANCEL~ para iniciar a missão");
		if (IS_CONTROL_JUST_PRESSED(0, Button_B))
		{
			if (LesterHeistStartedScript01)
				return; 

			missionContinuedScript01 = true;
			LesterHeistStartedScript01 = true;
			StartScript01();
		}
	}
}

// -------------------------------------
// inicio script 02
// -------------------------------------
enum MissionStateScript02 {
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

enum MissionStateScript02 missionState = MISSION_INIT;

// localização
int houseBlip = 0; 
vector3 houseOutside = {1400.2178f, 3599.8728f, 35.0367f};
vector3 houseInside1 = {1393.3156f, 3602.5510f, 38.9419f};
vector3 houseInside2 = {1389.8719f, 3601.0034f, 38.9424f};
vector3 boxPos      = {1392.5600f, 3601.1020f, 38.6000f};
vector3 factoryPos  = {706.18f, -961.12f, 30.0f}; 

// prop
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

void ShowLesterMsgScript02(const char* msg, const char* title) {
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpTextScript02(char* text) {
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void SpawnMissionNPCsAndProps() {
    if (npcsSpawned) return;
    // Outside NPC: 
    Hash outsideHash = GET_HASH_KEY("a_f_y_vinewood_04");
    Hash inside1Hash = GET_HASH_KEY("a_m_m_hillbilly_01");
    Hash inside2Hash = GET_HASH_KEY("a_m_m_hillbilly_02");

    REQUEST_MODEL(outsideHash);
    REQUEST_MODEL(inside1Hash);
    REQUEST_MODEL(inside2Hash);

    while (!HAS_MODEL_LOADED(outsideHash) ||
           !HAS_MODEL_LOADED(inside1Hash) ||
           !HAS_MODEL_LOADED(inside2Hash)) {
        WAIT(0);
    }

    npcOutside = CREATE_PED(26, outsideHash, houseOutside, 90.0f, 1, 1);
    if (npcOutside) TASK_START_SCENARIO_IN_PLACE(npcOutside, "WORLD_HUMAN_SMOKING", 0, 1);

    npcInside1 = CREATE_PED(26, inside1Hash, houseInside1, 180.0f, 1, 1);
    npcInside2 = CREATE_PED(26, inside2Hash, houseInside2, 270.0f, 1, 1);

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
    boxProp = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_ld_case_01"), boxPos.x, boxPos.y, boxPos.z - 0.25f, 1, 1, 0);
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

void ResetMission() {
    DespawnMissionNPCsAndProps();
    DespawnReinforcements();
    if (factoryBlip) { REMOVE_BLIP(&factoryBlip); factoryBlip = 0; }
    if (factoryBoxProp) { DELETE_OBJECT(&factoryBoxProp); factoryBoxProp = 0; }
    if (houseBlip) { REMOVE_BLIP(&houseBlip); houseBlip = 0; } // <-- Adicionado
    boxTaken = false;
    gpsEnabled = false;
    atFactory = false;
    delivered = false;
    playerDead = false;
    missionBlocked = false;
    missionState = MISSION_INIT;
}

void StartScript02(){
	if (!script02Started) {
        script02Started = true;
        
        SET_MISSION_FLAG(1);
        WAIT(0); // Essencial para troca de contexto

        // Mensagem inicial cortada
        ShowLesterMsgScript02("Descobri um esquema de quadrilhas perto de Sandy Shores. Marquei um local aproximado no mapa.", "Atenção!");
        WAIT(2000);
        ShowLesterMsgScript02("Roube a maleta, eu irei precisar. mas tome cuidado: a área é perigosa!", "Atenção!");

        // Cria blip aproximado (sem GPS)
        houseBlip = ADD_BLIP_FOR_COORD(houseOutside.x, houseOutside.y, houseOutside.z);
        SET_BLIP_SPRITE(houseBlip, 51); // Alterado de 280 para 51
        SET_BLIP_COLOUR(houseBlip, 0);
        SET_BLIP_SCALE(houseBlip, 0.9f);
        BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Local suspeito");
        END_TEXT_COMMAND_SET_BLIP_NAME(houseBlip);

        missionState = MISSION_APPROACH;
    } 
}

// -------------------------------------
// inicio script 03
// -------------------------------------

#define LESTER_FACTORY_X_SCRIPT03 706.73f
#define LESTER_FACTORY_Y_SCRIPT03 -965.00f
#define LESTER_FACTORY_Z_SCRIPT03 30.0f

bool missionContinuedScript03 = false;

void ShowLesterMsgScript03(const char* msg, const char* title) {
    _SET_NOTIFICATION_TEXT_ENTRY("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
    _SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
    _DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpTextScript03(const char* text) {
    BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

bool IsPlayerNearFactoryScript03() {
    int player = PLAYER_PED_ID();
    vector3 ppos = GET_ENTITY_COORDS(player, 1);
    float dx = ppos.x - LESTER_FACTORY_X_SCRIPT03;
    float dy = ppos.y - LESTER_FACTORY_Y_SCRIPT03;
    float dz = ppos.z - LESTER_FACTORY_Z_SCRIPT03;
    float dist = SQRT(dx*dx + dy*dy + dz*dz);
    return (dist < 2.5f);
}

void StartScript03() {
    if (!script03Started) {
        script03Started = true;
        SET_MISSION_FLAG(1);
        NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

        ShowLesterMsgScript03("Tudo preparado para continuarmos os preparativos.", "Lester");
        WAIT(4000);
        ShowLesterMsgScript03("Vá até a fábrica para continuar os planejamentos.", "Lester");
        WAIT(2000);
        // Remova o while (true) e toda a lógica contínua daqui!
    }
}

// -------------------------------------
// inicio script 04
// -------------------------------------

#define LESTER_FACTORY_X_SCRIPT04 707.85f
#define LESTER_FACTORY_Y_SCRIPT04 -966.62f
#define LESTER_FACTORY_Z_SCRIPT04 30.40f

#define DELIVERY_X_SCRIPT04 713.0f
#define DELIVERY_Y_SCRIPT04 -978.92f
#define DELIVERY_Z_SCRIPT04 23.73f

int getawayCarScript04 = 0;
int getawayDriverScript04 = 0;
int carBlipScript04 = 0;
int deliveryBlipScript04 = 0;
int assaultBlipScript04 = 0;
bool carStolenScript04 = false;
bool deliveryActiveScript04 = false;
bool deliveryDoneScript04 = false;
bool messageShownScript04 = false;
bool waitingToFinishScript04 = false;
int tickSinceDeliveryScript04 = 0;

void ShowLesterMsgScript04(const char* msg, const char* title) {
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpTextScript04(const char* text) {
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void SpawnGetawayCarScript04() {
	if (getawayCarScript04) return;
	// Carro aleatório: "buffalo"
	Hash carHash = GET_HASH_KEY("buffalo");
	REQUEST_MODEL(carHash);
	while (!HAS_MODEL_LOADED(carHash)) WAIT(0);
	vector3 spawn = {LESTER_FACTORY_X_SCRIPT04 + 300.0f, LESTER_FACTORY_Y_SCRIPT04 + 800.0f, LESTER_FACTORY_Z_SCRIPT04 + 1.0f}; // spawn longe da fábrica
	getawayCarScript04 = CREATE_VEHICLE(carHash, spawn, 180.0f, 1, 1);
	SET_VEHICLE_ON_GROUND_PROPERLY(getawayCarScript04);
	SET_VEHICLE_NUMBER_PLATE_TEXT(getawayCarScript04, "HEIST");
	SET_VEHICLE_IS_WANTED(getawayCarScript04, 0);
	SET_VEHICLE_ENGINE_ON(getawayCarScript04, 1, 1, 0);
	SET_VEHICLE_FORWARD_SPEED(getawayCarScript04, 40.0f);

	// NPC dirigindo
	Hash pedHash = GET_HASH_KEY("a_m_y_roadcyc_01");
	REQUEST_MODEL(pedHash);
	while (!HAS_MODEL_LOADED(pedHash)) WAIT(0);
	getawayDriverScript04 = CREATE_PED_INSIDE_VEHICLE(getawayCarScript04, 26, pedHash, -1, 1, 1);
	TASK_VEHICLE_DRIVE_WANDER(getawayDriverScript04, getawayCarScript04, 40.0f, 786603); // Alta velocidade

	// Blip do carro
	carBlipScript04 = ADD_BLIP_FOR_ENTITY(getawayCarScript04);
	SET_BLIP_SPRITE(carBlipScript04, 225);
	SET_BLIP_COLOUR(carBlipScript04, 5);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Carro do Plano");
	END_TEXT_COMMAND_SET_BLIP_NAME(carBlipScript04);
}

void RemoveGetawayCarScript04() {
	if (carBlipScript04) { REMOVE_BLIP(&carBlipScript04); carBlipScript04 = 0; }
	if (getawayDriverScript04) { DELETE_PED(&getawayDriverScript04); getawayDriverScript04 = 0; }
	if (getawayCarScript04) { DELETE_VEHICLE(&getawayCarScript04); getawayCarScript04 = 0; }
}

void CreateDeliveryBlipScript04() {
	if (deliveryBlipScript04) return;
	deliveryBlipScript04 = ADD_BLIP_FOR_COORD(DELIVERY_X_SCRIPT04, DELIVERY_Y_SCRIPT04, DELIVERY_Z_SCRIPT04);
	SET_BLIP_SPRITE(deliveryBlipScript04, 1);
	SET_BLIP_COLOUR(deliveryBlipScript04, 2);
	SET_BLIP_ROUTE(deliveryBlipScript04, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Entregar Carro");
	END_TEXT_COMMAND_SET_BLIP_NAME(deliveryBlipScript04);
}

void RemoveDeliveryBlipScript04() {
	if (deliveryBlipScript04) { REMOVE_BLIP(&deliveryBlipScript04); deliveryBlipScript04 = 0; }
}

bool IsPlayerInGetawayCarScript04() {
	int player = PLAYER_PED_ID();
	return (IS_PED_IN_VEHICLE(player, getawayCarScript04, 0));
}

float GetDistToDeliveryScript04() {
	vector3 pos = GET_ENTITY_COORDS(getawayCarScript04, 1);
	float dx = pos.x - DELIVERY_X_SCRIPT04;
	float dy = pos.y - DELIVERY_Y_SCRIPT04;
	float dz = pos.z - DELIVERY_Z_SCRIPT04;
	return SQRT(dx*dx + dy*dy + dz*dz);
}

void StartScript04(){
	if (!script04Started) {
		script04Started = true;
		SET_MISSION_FLAG(1);
			WAIT(0);
			//NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

			WAIT(2000);
			ShowLesterMsgScript02("É urgente! Desculpe não avisar antes. Um carro essencial para o plano está rondando Los Santos. Capture-o e leve até minha fábrica!", "URGENTE");

			WAIT(2000);
			ShowLesterMsgScript02("Irei marcar assim que possivel!", "URGENTE");
			WAIT(10000);
			SpawnGetawayCarScript04();
	}
}

// -------------------------------------
// inicio script 05
// -------------------------------------

#define LESTER_FACTORY_X_SCRIPT05 706.73f
#define LESTER_FACTORY_Y_SCRIPT05 -965.00f
#define LESTER_FACTORY_Z_SCRIPT05 30.0f

bool missionContinuedScript05 = false;

void ShowLesterMsgScript05(const char* msg, const char* title) {
    _SET_NOTIFICATION_TEXT_ENTRY("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
    _SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
    _DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpTextScript05(char* text) {
    BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

bool IsPlayerNearFactoryScript05() {
    int player = PLAYER_PED_ID();
    vector3 ppos = GET_ENTITY_COORDS(player, 1);
    float dx = ppos.x - LESTER_FACTORY_X_SCRIPT05;
    float dy = ppos.y - LESTER_FACTORY_Y_SCRIPT05;
    float dz = ppos.z - LESTER_FACTORY_Z_SCRIPT05;
    float dist = SQRT(dx*dx + dy*dy + dz*dz);
    return (dist < 2.5f);
}

void StartScript05(){ 
    if (!script05Started) {
        script05Started = true;

        ShowLesterMsgScript02("Tudo pronto para continuarmos. Passe na fábrica para dar andamento ao plano!", "Lester");
        WAIT(2000);
    }
}

// -------------------------------------
// inicio script 06
// -------------------------------------

// Coordenadas principais
#define LESTER_FACTORY_X_SCRIPT06 707.85f
#define LESTER_FACTORY_Y_SCRIPT06 -966.62f
#define LESTER_FACTORY_Z_SCRIPT06 30.40f

// Altere aqui para a nova posição de entrega:
#define DELIVERY_X_SCRIPT06 706.73f
#define DELIVERY_Y_SCRIPT06 -961.12f
#define DELIVERY_Z_SCRIPT06 30.0f

#define FLEECA_BLIP_X_SCRIPT06 -2964.6704f
#define FLEECA_BLIP_Y_SCRIPT06  482.9354f
#define FLEECA_BLIP_Z_SCRIPT06   15.7068f

#define FLEECA_DOOR_X_SCRIPT06 -2960.0f
#define FLEECA_DOOR_Y_SCRIPT06  484.0f
#define FLEECA_DOOR_Z_SCRIPT06   15.7f
#define FLEECA_VAULT_X_SCRIPT06 -2960.0f
#define FLEECA_VAULT_Y_SCRIPT06  484.0f
#define FLEECA_VAULT_Z_SCRIPT06 	15.7f

#define ASSAULT_ROOM_X_SCRIPT06 710.0f
#define ASSAULT_ROOM_Y_SCRIPT06 -965.0f
#define ASSAULT_ROOM_Z_SCRIPT06 30.40f

#define FLEECA_PARK_X_SCRIPT06 -2973.6899f
#define FLEECA_PARK_Y_SCRIPT06  481.2466f
#define FLEECA_PARK_Z_SCRIPT06   14.8711f

// Dicionários de animação usados

// Animações diferentes para cada NPC
#define ANIM_IDLE_1 "amb@world_human_stand_mobile@male@standing@call@base"
#define ANIM_IDLE_2 "amb@world_human_hang_out_street@male_b@idle_a"
#define ANIM_IDLE_3 "amb@world_human_clipboard@male@idle_a"

#define ANIM_HANDSUP "random@arrests"
#define ANIM_HANDSUP_NAME "idle_a"

// variables
int getawayCar = 0;
int carBlip = 0;
int fleecaBlip = 0;
int deliveryBlip = 0;
int assaultBlip = 0;
bool carReady = false;
bool atFleeca = false;
bool alarmTriggered = false;
bool hasFiveStars = false;
bool lootPlaced = false;
bool missionFinished = false;
bool playerDelivered = false;
bool bagsSpawned = false;
bool waitingStart = true;
int tick = 0;
int tickAfterLoot = 0;
int tickAfterEscape = 0;
bool escapeStarted = false;
bool escapeComplete = false;
bool showPlaceLoot = false;
bool doorScriptStarted = false;
bool parkedNearBank = false;
bool maskEquipped = false;
bool robberyStarted = false;
bool enteredBank = false;
bool aimingAtHostages = false;
int fleecaNpcs[3] = {0};
int parkCheckpoint = 0;
int parkedTimer = 0;
bool parkedConfirmed = false;
bool fleecaNpcsSpawned = false;
bool hostagesHandsUp = false;
bool alarmSequenceStarted = false;
int deliveryCheckpoint = 0;
bool canDeliverLoot = false;
int deliveryMoneyProp = 0;
bool hostagesReacted = false;
int alarmSequenceTimer = 0;
bool hostagesKneel[2] = {false, false};
int hostagesKneelTimer = 0;

void ShowLesterMsgScript06(const char* msg, const char* title) {
	_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
	_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_LESTER", "CHAR_LESTER", 1, 7, "Lester", title, 1, "", 8);
	_DRAW_NOTIFICATION(5000, 1);
}

void FloatingHelpTextScript06(char* text) {
	BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
	END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

void StartDoorScript() {
	if (doorScriptStarted) return;

	REQUEST_SCRIPT("door");
	while (!HAS_SCRIPT_LOADED("door")) WAIT(0);

	START_NEW_SCRIPT("door", 1024);
	SET_SCRIPT_AS_NO_LONGER_NEEDED("door");

	doorScriptStarted = true;
}

void SpawnGetawayCar() {
	if (getawayCar) return;
	Hash carHash = GET_HASH_KEY("buffalo");
	REQUEST_MODEL(carHash);
	while (!HAS_MODEL_LOADED(carHash)) WAIT(0);
	vector3 spawn = {DELIVERY_X_SCRIPT06, DELIVERY_Y_SCRIPT06, DELIVERY_Z_SCRIPT06};
	getawayCar = CREATE_VEHICLE(carHash, spawn, 180.0f, 1, 1);
	SET_VEHICLE_ON_GROUND_PROPERLY(getawayCar);
	SET_VEHICLE_NUMBER_PLATE_TEXT(getawayCar, "HEIST");
	SET_VEHICLE_IS_WANTED(getawayCar, 0);
	SET_VEHICLE_ENGINE_ON(getawayCar, 1, 1, 0);
}

void RemoveGetawayCar() {
	if (carBlip) { REMOVE_BLIP(&carBlip); carBlip = 0; }
	if (getawayCar) { DELETE_VEHICLE(&getawayCar); getawayCar = 0; }
}

void CreateCarBlip() {
	if (carBlip) return;
	carBlip = ADD_BLIP_FOR_ENTITY(getawayCar);
	SET_BLIP_SPRITE(carBlip, 225);
	SET_BLIP_COLOUR(carBlip, 2);
	SET_BLIP_ROUTE(carBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Carro do Assalto");
	END_TEXT_COMMAND_SET_BLIP_NAME(carBlip);
}

void RemoveCarBlip() {
	if (carBlip) { REMOVE_BLIP(&carBlip); carBlip = 0; }
}

void CreateFleecaBlip() {
	if (fleecaBlip) return;
	fleecaBlip = ADD_BLIP_FOR_COORD(FLEECA_BLIP_X_SCRIPT06, FLEECA_BLIP_Y_SCRIPT06, FLEECA_BLIP_Z_SCRIPT06);
	SET_BLIP_SPRITE(fleecaBlip, 108); // Blip 108
	SET_BLIP_COLOUR(fleecaBlip, 5);   // Amarelo igual aos outros
	SET_BLIP_ROUTE(fleecaBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Banco Fleeca");
	END_TEXT_COMMAND_SET_BLIP_NAME(fleecaBlip);
}

void RemoveFleecaBlip() {
	if (fleecaBlip) { REMOVE_BLIP(&fleecaBlip); fleecaBlip = 0; }
}

void CreateLesterFactoryBlip() {
    if (assaultBlip) return;

    assaultBlip = ADD_BLIP_FOR_COORD(
        LESTER_FACTORY_X_SCRIPT06,
        LESTER_FACTORY_Y_SCRIPT06,
        LESTER_FACTORY_Z_SCRIPT06
    );

    SET_BLIP_SPRITE(assaultBlip, 1);      // waypoint padrão
    SET_BLIP_COLOUR(assaultBlip, 5);      // amarelo
    SET_BLIP_ROUTE(assaultBlip, 1);

    BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Fábrica do Lester");
    END_TEXT_COMMAND_SET_BLIP_NAME(assaultBlip);
}

void CreateDeliveryBlip() {
	if (deliveryBlip) return;
	deliveryBlip = ADD_BLIP_FOR_COORD(DELIVERY_X_SCRIPT06, DELIVERY_Y_SCRIPT06, DELIVERY_Z_SCRIPT06);
	SET_BLIP_SPRITE(deliveryBlip, 1);
	SET_BLIP_COLOUR(deliveryBlip, 2);
	SET_BLIP_ROUTE(deliveryBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Entregar Dinheiro");
	END_TEXT_COMMAND_SET_BLIP_NAME(deliveryBlip);
}

void RemoveDeliveryBlip() {
	if (deliveryBlip) { REMOVE_BLIP(&deliveryBlip); deliveryBlip = 0; }
}

void CreateAssaultBlip() {
	if (assaultBlip) return;
	assaultBlip = ADD_BLIP_FOR_COORD(ASSAULT_ROOM_X_SCRIPT06, ASSAULT_ROOM_Y_SCRIPT06, ASSAULT_ROOM_Z_SCRIPT06);
	SET_BLIP_SPRITE(assaultBlip, 1);
	SET_BLIP_COLOUR(assaultBlip, 5);
	SET_BLIP_ROUTE(assaultBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Depositar Dinheiro");
	END_TEXT_COMMAND_SET_BLIP_NAME(assaultBlip);
}

void RemoveAssaultBlip() {
	if (assaultBlip) { REMOVE_BLIP(&assaultBlip); assaultBlip = 0; }
}

bool IsPlayerInGetawayCar() {
	int player = PLAYER_PED_ID();
	return (IS_PED_IN_VEHICLE(player, getawayCar, 0));
}

float GetDist(vector3 a, float x, float y, float z) {
	float dx = a.x - x;
	float dy = a.y - y;
	float dz = a.z - z;
	return SQRT(dx*dx + dy*dy + dz*dz);
}

void RequestAnimDict(const char* dict) {
	REQUEST_ANIM_DICT(dict);
	while (!HAS_ANIM_DICT_LOADED(dict)) WAIT(0);
}

void SpawnFleecaNPCs() {
	if (fleecaNpcsSpawned) return;

	vector3 npcsPos[3] = {
		{-2962.59f, 485.86f, 15.69f},
		{-2964.90f, 479.48f, 16.69f},
		{-2960.92f, 483.06f, 15.69f}
	};
	float headings[3] = {180.0f, 90.0f, 270.0f};
	const char* animDicts[3] = { ANIM_IDLE_1, ANIM_IDLE_2, ANIM_IDLE_3 };
	const char* animNames[3] = { "base", "idle_a", "idle_a" };

	Hash pedHash = GET_HASH_KEY("a_m_y_business_01");
	REQUEST_MODEL(pedHash);
	while (!HAS_MODEL_LOADED(pedHash)) WAIT(0);

	for (int i = 0; i < 3; i++) {
		RequestAnimDict(animDicts[i]);
		fleecaNpcs[i] = CREATE_PED(
			26,
			pedHash,
			npcsPos[i],
			headings[i],
			1,
			1
		);
		SET_ENTITY_AS_MISSION_ENTITY(fleecaNpcs[i], 1, 1);
		SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(fleecaNpcs[i], 1);
		SET_PED_FLEE_ATTRIBUTES(fleecaNpcs[i], 0, 0);
		SET_PED_CAN_RAGDOLL(fleecaNpcs[i], 0);
		CLEAR_PED_TASKS_IMMEDIATELY(fleecaNpcs[i]);
		// Força animação idle correta para cada NPC
		RequestAnimDict(animDicts[i]);
		TASK_PLAY_ANIM(
			fleecaNpcs[i],
			animDicts[i],
			animNames[i],
			8.0f,
			-8.0f,
			-1,
			1, // loop
			0,
			0,
			0,
			0
		);
	}
	fleecaNpcsSpawned = true;
}

void RemoveFleecaNPCs() {
	for (int i = 0; i < 3; i++) {
		if (fleecaNpcs[i]) {
			DELETE_PED(&fleecaNpcs[i]);
			fleecaNpcs[i] = 0;
		}
	}
	fleecaNpcsSpawned = false;
}

void RestartMission() {
	RemoveGetawayCar();
	RemoveCarBlip();
	RemoveFleecaBlip();
	RemoveDeliveryBlip();
	RemoveAssaultBlip();
	RemoveFleecaNPCs(); // Remover NPCs do banco
	carReady = false;
	atFleeca = false;
	alarmTriggered = false;
	hasFiveStars = false;
	lootPlaced = false;
	missionFinished = false;
	playerDelivered = false;
	bagsSpawned = false;
	waitingStart = true;
	tick = 0;
	tickAfterLoot = 0;
	tickAfterEscape = 0;
	escapeStarted = false;
	escapeComplete = false;
	showPlaceLoot = false;
	doorScriptStarted = false;
	parkedNearBank = false;
	maskEquipped = false;
	robberyStarted = false;
	enteredBank = false;
	aimingAtHostages = false;
	parkCheckpoint = 0;
	parkedTimer = 0;
	parkedConfirmed = false;
	// Spawn carro na fábrica novamente
	SpawnGetawayCar();
	CreateCarBlip();
}

void ResetMissionState() {
	alarmTriggered = false;
	alarmSequenceStarted = false;
	escapeComplete = false;
	missionFinished = false;
	carReady = false;
	atFleeca = false;
	parkedNearBank = false;
	parkedConfirmed = false;
	maskEquipped = false;
	enteredBank = false;
	hostagesHandsUp = false;
	fleecaNpcsSpawned = false;
	RemoveFleecaNPCs(); // Remover NPCs do banco
	RemoveGetawayCar();
	RemoveCarBlip();
	RemoveFleecaBlip();
	RemoveDeliveryBlip();
	if (parkCheckpoint) {
		DELETE_CHECKPOINT(parkCheckpoint);
		parkCheckpoint = 0;
	}
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, 0);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 1);
	// Spawn carro na fábrica novamente
	SpawnGetawayCar();
	CreateCarBlip();
}

bool MissionFailed() {
    int player = PLAYER_PED_ID();

    if (IS_ENTITY_DEAD(player)) return true;
    if (GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) >= 4 &&
        IS_PLAYER_BEING_ARRESTED(PLAYER_ID(), 1)) return true;

    return false;
}

void BlockAmbientEvents() {
	// Desativa eventos aleatórios e peds não relacionados à missão
	SET_CREATE_RANDOM_COPS(0);
	SET_CREATE_RANDOM_COPS_NOT_ON_SCENARIOS(0);
	SET_CREATE_RANDOM_COPS_ON_SCENARIOS(0);
	SET_RANDOM_EVENT_FLAG(0);
	SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	SET_GARBAGE_TRUCKS(0);
	SET_RANDOM_BOATS(0);
	SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	SET_PARKED_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0f);
	SET_SCENARIO_TYPE_ENABLED("WORLD_HUMAN_PROSTITUTE_HIGH_CLASS", 0);
	SET_SCENARIO_TYPE_ENABLED("WORLD_HUMAN_PROSTITUTE_LOW_CLASS", 0);
	// Adicione outros cenários conforme necessário
}

void MakeHostagesRaiseHands() {
	RequestAnimDict(ANIM_HANDSUP);
	for (int i = 0; i < 3; i++) {
		if (fleecaNpcs[i]) {
			CLEAR_PED_TASKS_IMMEDIATELY(fleecaNpcs[i]);
			// Força o carregamento do dicionário antes de animar
			RequestAnimDict(ANIM_HANDSUP);
			TASK_PLAY_ANIM(
				fleecaNpcs[i],
				ANIM_HANDSUP,
				ANIM_HANDSUP_NAME,
				8.0f,
				-8.0f,
				-1,
				2, // upper body only, não loop
				0,
				0,
				0,
				0
			);
			SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(fleecaNpcs[i], 1);
		}
	}
	hostagesHandsUp = true;
	hostagesReacted = true;
	hostagesKneel[0] = false;
	hostagesKneel[1] = false;
	hostagesKneelTimer = 0;
}

void StartScript06(){
	if (!script06Started) {
		script06Started = true;
		SET_MISSION_FLAG(1);
		NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	//retry:
		ShowLesterMsgScript02("Está tudo pronto, venha até a fábrica e pegue o carro!", "Lester");
		WAIT(2000);
		ShowLesterMsgScript02("Desculpe não estar aí, estava ocupado hackeando as câmeras do banco em casa.", "Lester");

		SpawnGetawayCar();
		CreateCarBlip();
	}
}

// script 06 final
// ============================================================================
// FINANCE FUNCTIONS
// ============================================================================
int GetCurrentMoneyStat()
{
    Ped p = PLAYER_PED_ID();
    Hash model = GET_ENTITY_MODEL(p);

    if (model == GET_HASH_KEY("player_zero")) return GET_HASH_KEY("SP0_TOTAL_CASH");
    if (model == GET_HASH_KEY("player_one"))  return GET_HASH_KEY("SP1_TOTAL_CASH");
    if (model == GET_HASH_KEY("player_two"))  return GET_HASH_KEY("SP2_TOTAL_CASH");

    return GET_HASH_KEY("SP0_TOTAL_CASH");
}

void AddMoneyToCurrentCharacter(int amount)
{
    int stat = GetCurrentMoneyStat();
    int value = 0;
    STAT_GET_INT(stat, &value, -1);
    STAT_SET_INT(stat, value + amount, 1);
}

// ============================================================================
// UI FUNCTIONS - USANDO BEGIN_TEXT_COMMAND_PRINT
// ============================================================================
void ShowLesterMsgFinal(const char* msg, int duration)
{
    // Inicia o comando de impressão de texto na tela (legenda em baixo)
    BEGIN_TEXT_COMMAND_PRINT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
    // Parâmetros: (Duração em ms, Ativar loop/Scroll)
    END_TEXT_COMMAND_PRINT(duration, 1);
}

bool missionFinished06 = false;
int tickAfterLoot06 = 0;

void StartScript06Final(){
	if (!script06FinalStarted) {
        script06FinalStarted = true;
        // Apenas inicialização
    }
}

void UpdateScript01()
{
    CheckLesterHeistProximityScript01();

    if (!missionContinuedScript01) {
        vector3 markerPos = {LESTER_FACTORY_X_SCRIPT01, LESTER_FACTORY_Y_SCRIPT01, LESTER_FACTORY_Z_SCRIPT01 - 1.0f};
        vector3 zero = {0.0f, 0.0f, 0.0f};
        vector3 scale = {2.0f, 2.0f, 1.0f};
        RGBA color = {0, 255, 0, 120};
        DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
    }

    if (!missionContinuedScript01 && NearLesterHeistScript01)
    {
        FloatingHelpText("Aperte ~INPUT_FRONTEND_CANCEL~ para iniciar a missão");
        if (IS_CONTROL_JUST_PRESSED(0, Button_B))
        {
            if (LesterHeistStartedScript01)
                return; 

            missionContinuedScript01 = true;
            LesterHeistStartedScript01 = true;
            StartScript01();
        }
    }

    if (missionContinuedScript01) {
        script01Finished = true;
    }
}

void UpdateScript02()
{
	// pump existing mission state machine once per-frame by reusing code blocks
	// (reuse functions and globals already defined acima)
	{
        int player = PLAYER_PED_ID();

        if (IS_ENTITY_DEAD(player)) {
            playerDead = true;
            missionState = MISSION_FAILED;
        }

        // --- NOVA LÓGICA: Spawn/despawn NPCs enquanto não pegou a maleta ---
        if (missionState == MISSION_APPROACH || missionState == MISSION_INSIDE || missionState == MISSION_SHOOTOUT) {
            if (PlayerNearV(houseOutside, 80.0f)) {
                SpawnMissionNPCsAndProps();
            } else {
                DespawnMissionNPCsAndProps();
            }
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
			// Marker horizontal circle fat embaixo da maleta
			if (boxSpawned && boxProp) {
				vector3 markerPos = {boxPos.x, boxPos.y, boxPos.z - 0.3f};
				vector3 zero = {0.0f, 0.0f, 0.0f};
				vector3 scale = {1.0f, 1.0f, 0.5f};
				RGBA yellow = {255, 255, 0, 180};
				DRAW_MARKER(23, markerPos, zero, zero, scale, yellow, 0, 0, 2, 0, 0, 0, 0);
			}
			// Permite pegar a maleta mesmo com NPCs vivos
			if (boxSpawned && boxProp && PlayerNearV(boxPos, 1.5f)) {
				FloatingHelpText("Pressione ~INPUT_FRONTEND_X~ para pegar a maleta");
				if (IS_CONTROL_JUST_PRESSED(0, Button_X)) { // X
					boxTaken = true;
					DELETE_OBJECT(&boxProp); boxProp = 0;
					ShowLesterMsgScript02("Boa, cara! Mas cuidado... aparentemente eles chamaram reforços.", "Atenção!");
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
				ShowLesterMsgScript02("Boa, cara. Sempre acreditei em você. Agora volte para a fábrica e me entregue a caixa.", "Sucesso!");
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
			if (houseBlip) { REMOVE_BLIP(&houseBlip); houseBlip = 0; }
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
				if (IS_CONTROL_JUST_PRESSED(0, Button_X)) { // B
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
			ShowLesterMsgScript02("Muito obrigado. Aguente firme, mais tarde te mandarei mensagem quando tiver novos avanços.", "Missão Completa");
			// Clean up
			if (factoryBlip) { REMOVE_BLIP(&factoryBlip); factoryBlip = 0; }
			if (factoryBoxProp) { DELETE_OBJECT(&factoryBoxProp); factoryBoxProp = 0; }
			missionBlocked = false;
			missionState = MISSION_COMPLETE;
			break;

		case MISSION_COMPLETE:
			ShowLesterMsgScript02("Missão concluída.", "Sucesso");
			// mark finished so sequencer advances
			missionState = MISSION_COMPLETE; // keep stable
			missionBlocked = false;
			script02Finished = true;
			break;
		case MISSION_FAILED:
			ShowLesterMsgScript02("Você morreu! Missão reiniciada.", "Falha");
			ResetMission();
			script02Finished = false; // can decide to restart or mark finished; here leave false to allow retry
			break;
		default:
			break;
		}

		WAIT(20);
	}
}

// Start/Update for Script03 (wrapper that uses existing StartScript03 internals)

void UpdateScript03()
{
    if (!missionContinuedScript03) {
        vector3 markerPos = {LESTER_FACTORY_X_SCRIPT03, LESTER_FACTORY_Y_SCRIPT03, LESTER_FACTORY_Z_SCRIPT03 - 1.0f};
        vector3 zero = {0.0f, 0.0f, 0.0f};
        vector3 scale = {2.0f, 2.0f, 1.0f};
        RGBA color = {0, 255, 0, 120};
        DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
    }
    if (!missionContinuedScript03 && IsPlayerNearFactoryScript03()) {
        FloatingHelpTextScript03("Aperte B para continuar o planejamento");
        if (IS_CONTROL_JUST_PRESSED(0, Button_B)) {
            missionContinuedScript03 = true;
            script03Finished = true;
        }
    }
}

// Start/Update for Script04

void UpdateScript04()
{
	// pump original logic per-frame (delivery detection, markers, finish)
	if (!carStolenScript04 && getawayCarScript04 && IsPlayerInGetawayCarScript04()) {
		carStolenScript04 = true;
		if (carBlipScript04) { SET_BLIP_ROUTE(carBlipScript04, 0); }
		ShowLesterMsgScript04("Leve o carro até a fábrica. O local está marcado no GPS!", "Plano");
		CreateDeliveryBlipScript04();
		deliveryActiveScript04 = true;
	}
	if (deliveryActiveScript04 && getawayCarScript04) {
		// Mostra marker no local de entrega
		vector3 markerPos = {DELIVERY_X_SCRIPT04, DELIVERY_Y_SCRIPT04, DELIVERY_Z_SCRIPT04};
		vector3 zero = {0.0f, 0.0f, 0.0f};
		vector3 scale = {2.5f, 2.5f, 1.0f};
		RGBA blue = {0, 150, 255, 120};
		DRAW_MARKER(1, markerPos, zero, zero, scale, blue, 0, 0, 2, 0, 0, 0, 0);
		// Se carro está em cima do marker
		if (!deliveryDoneScript04 && GetDistToDeliveryScript04() < 3.0f && IsPlayerInGetawayCarScript04()) {
			SET_VEHICLE_ENGINE_ON(getawayCarScript04, 0, 1, 0);
			TASK_LEAVE_VEHICLE(PLAYER_PED_ID(), getawayCarScript04, 0);
			FloatingHelpTextScript04("Saia do carro!");
			deliveryDoneScript04 = true;
			RemoveDeliveryBlipScript04();
			   if (carBlipScript04) { REMOVE_BLIP(&carBlipScript04); carBlipScript04 = 0; }
			   waitingToFinishScript04 = true;
			   tickSinceDeliveryScript04 = 0;
			   FloatingHelpTextScript04("Saia do local!");
		}
	}
	if (waitingToFinishScript04 && deliveryDoneScript04) {
		tickSinceDeliveryScript04++;
		int player = PLAYER_PED_ID();
		vector3 ppos = GET_ENTITY_COORDS(player, 1);
		float dx = ppos.x - DELIVERY_X_SCRIPT04;
		float dy = ppos.y - DELIVERY_Y_SCRIPT04;
		float dz = ppos.z - DELIVERY_Z_SCRIPT04;
		float dist = SQRT(dx*dx + dy*dy + dz*dz);
		if (dist > 30.0f) {
			   RemoveGetawayCarScript04();
			   FloatingHelpTextScript04(""); // limpa helptext (remove mensagem "Saia do local!")
			   ShowLesterMsgScript02("Perfeito! Obrigado, agora estamos prontos para a próxima etapa.", "Lester");
			   WAIT(2000);
			   script04Finished = true;
		}
	}
}

// Start/Update for Script05


void UpdateScript05()
{
	// similar to previous factory-continue logic
	if (!missionContinuedScript05) {
		vector3 markerPos = {LESTER_FACTORY_X_SCRIPT05, LESTER_FACTORY_Y_SCRIPT05, LESTER_FACTORY_Z_SCRIPT05 - 1.0f};
		vector3 zero = {0.0f, 0.0f, 0.0f};
		vector3 scale = {2.0f, 2.0f, 1.0f};
		RGBA color = {0, 255, 0, 120};
		DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
	}
	if (!missionContinuedScript05  && IsPlayerNearFactoryScript05()) {
		FloatingHelpTextScript05("Aperte B para continuar o golpe");
		if (IS_CONTROL_JUST_PRESSED(0, Button_B)) {
			missionContinuedScript05 = true;
			WAIT(2000);
			script05Finished = true;
		}
	} else if (!missionContinuedScript05) {
		FloatingHelpTextScript05("");
	}
}

// Start/Update for Script06 (Fleeca) — init only, core per-frame pumping done in UpdateScript06

void UpdateScript06()
{
	BlockAmbientEvents();

			if (MissionFailed()) {
				WAIT(30000);
				ShowLesterMsgScript02("Droga! Deu tudo errado! tente novamente", "Lester");
				WAIT(20000);

				ResetMissionState();
				//goto retry;
			}

			int player = PLAYER_PED_ID();
			vector3 ppos = GET_ENTITY_COORDS(player, 1);

			for (int i = 0; i < 3; i++) {
				if (fleecaNpcs[i]) {
					SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(fleecaNpcs[i], 1);
				}
			}

			// Inicia script de portas ao entrar no carro da missão
			if (IsPlayerInGetawayCar() && !doorScriptStarted) {
				StartDoorScript();
			}

			// Falha da missão
			if (MissionFailed()) {
				ShowLesterMsgScript02("Você falhou! Tente novamente.", "Lester");
				WAIT(3000);
				RestartMission();
				//goto retry;
			}

			// Player entra no carro da missão
			if (!carReady && IsPlayerInGetawayCar()) {
				carReady = true;
				RemoveCarBlip();
				CreateFleecaBlip();
				ShowLesterMsgScript02("Dirija até o Banco Fleeca.", "Plano");
			}

			// NPCs só spawnam quando player está próximo do banco
			if (carReady && !fleecaNpcsSpawned &&
				GetDist(ppos, FLEECA_DOOR_X_SCRIPT06, FLEECA_DOOR_Y_SCRIPT06, FLEECA_DOOR_Z_SCRIPT06) < 30.0f) {

				SpawnFleecaNPCs();
			}

			// Chegou na área do Fleeca
			if (carReady && !atFleeca &&
				GetDist(ppos, FLEECA_PARK_X_SCRIPT06, FLEECA_PARK_Y_SCRIPT06, FLEECA_PARK_Z_SCRIPT06) < 40.0f) {

				atFleeca = true;
				vector3 parkPos = {FLEECA_PARK_X_SCRIPT06, FLEECA_PARK_Y_SCRIPT06, FLEECA_PARK_Z_SCRIPT06 - 0.5f};
				vector3 parkDir = {0.0f, 0.0f, 0.0f};
				RGBA color = {255, 255, 0, 180};

				parkCheckpoint = CREATE_CHECKPOINT(
					4,            // tipo pequeno no chão
					parkPos,
					parkDir,
					1.0f,         // raio pequeno
					color,
					0
				);

				ShowLesterMsgScript02("Estacione o carro no local marcado.", "Lester");
			}

			// ETAPA 1 — estacionou perto do banco
			if (atFleeca && !parkedConfirmed &&
				IsPlayerInGetawayCar() &&
				GetDist(ppos, FLEECA_PARK_X_SCRIPT06, FLEECA_PARK_Y_SCRIPT06, FLEECA_PARK_Z_SCRIPT06) < 3.5f &&
				GET_ENTITY_SPEED(getawayCar) < 0.5f) {

				parkedTimer++;

				// ~2 segundos (20ms * 100)
				if (parkedTimer > 40) {
					SET_VEHICLE_FORWARD_SPEED(getawayCar, 0.0f);
					vector3 zeroVel = {0.0f, 0.0f, 0.0f};
					SET_ENTITY_VELOCITY(getawayCar, zeroVel);
					SET_VEHICLE_ENGINE_ON(getawayCar, 0, 1, 1);
					SET_VEHICLE_HANDBRAKE(getawayCar, 1);

					parkedConfirmed = true;
					parkedNearBank = true;

					DELETE_CHECKPOINT(parkCheckpoint);
					parkCheckpoint = 0;

					ShowLesterMsgScript02("Pode ir. Coloque a máscara e entre no banco.", "Lester");
				}
			}
			else {
				parkedTimer = 0; // saiu do local, reseta
			}

			// ETAPA 2 — colocar a máscara dentro do carro
			if (parkedNearBank && !maskEquipped && IsPlayerInGetawayCar()) {
				FloatingHelpText("Aperte ~INPUT_CELLPHONE_RIGHT~ para colocar a máscara");

				if (IS_CONTROL_JUST_PRESSED(0, INPUT_CELLPHONE_RIGHT)) {
					SET_PED_COMPONENT_VARIATION(player, 1, 52, 0, 0); // máscara exemplo
					maskEquipped = true;
					FloatingHelpText("");
				}
			}

			if (parkedNearBank && !maskEquipped && IsPlayerInGetawayCar()) {
				DISABLE_CONTROL_ACTION(0, 75, 1); // sair do veículo
			}

			// ETAPA 3 — entrou no banco
			if (maskEquipped && !enteredBank &&
				!IsPlayerInGetawayCar() &&
				GetDist(ppos, FLEECA_DOOR_X_SCRIPT06, FLEECA_DOOR_Y_SCRIPT06, FLEECA_DOOR_Z_SCRIPT06) < 5.0f) {

				// Garante que os NPCs estejam presentes e animados
				if (!fleecaNpcsSpawned) {
					SpawnFleecaNPCs();
				} else {
					// Força animação idle para todos NPCs ao entrar no banco
					const char* animDicts[3] = { ANIM_IDLE_1, ANIM_IDLE_2, ANIM_IDLE_3 };
					const char* animNames[3] = { "base", "idle_a", "idle_a" };
					for (int i = 0; i < 3; i++) {
						if (fleecaNpcs[i]) {
							RequestAnimDict(animDicts[i]);
							CLEAR_PED_TASKS_IMMEDIATELY(fleecaNpcs[i]);
							TASK_PLAY_ANIM(
								fleecaNpcs[i],
								animDicts[i],
								animNames[i],
								8.0f,
								-8.0f,
								-1,
								1, // loop
								0,
								0,
								0,
								0
							);
						}
					}
				}
				ShowLesterMsgScript02(
					"Aponte a arma para os reféns e abra o cofre com o cartão.",
					"Lester"
				);

				enteredBank = true;
				alarmSequenceTimer = 0;
				hostagesReacted = false;
			}

			// SISTEMA DE REAÇÃO DOS REFÉNS
			if (enteredBank && !hostagesReacted) {
				bool aimingAtNpc = false;
				int player = PLAYER_PED_ID();

				for (int i = 0; i < 3; i++) {
					if (fleecaNpcs[i]) {
						// Verifica se o player está mirando em algum NPC
						if (IS_PLAYER_FREE_AIMING_AT_ENTITY(PLAYER_ID(), fleecaNpcs[i])) {
							aimingAtNpc = true;
							break;
						}
						// Se o player atirar próximo do NPC
						if (IS_PED_SHOOTING(player)) {
							vector3 npcPos = GET_ENTITY_COORDS(fleecaNpcs[i], 1);
							vector3 playerPos = GET_ENTITY_COORDS(player, 1);
							if (GetDist(playerPos, npcPos.x, npcPos.y, npcPos.z) < 8.0f) {
								aimingAtNpc = true;
								break;
							}
						}
					}
				}

				if (aimingAtNpc) {
					MakeHostagesRaiseHands();
					alarmSequenceStarted = true;
					alarmSequenceTimer = 0;
				} else {
					alarmSequenceTimer++;
					// Espera 5 segundos (~250 ticks de 20ms)
					if (alarmSequenceTimer > 250) {
						MakeHostagesRaiseHands();
						alarmSequenceStarted = true;
					}
				}
			}

			// Após 10 segundos (~500 ticks de 20ms), os dois primeiros reféns ajoelham/deitam
			if (hostagesHandsUp && !hostagesKneel[0] && !hostagesKneel[1]) {
				hostagesKneelTimer++;
				if (hostagesKneelTimer > 500) { // 10 segundos
					// Animação de ajoelhar/deitar
					const char* kneelDict = "rcmbarry";
					const char* kneelAnim = "bar_1_attack_idle_aln";
					RequestAnimDict(kneelDict);
					for (int i = 0; i < 2; i++) {
						if (fleecaNpcs[i]) {
							CLEAR_PED_TASKS_IMMEDIATELY(fleecaNpcs[i]);
							TASK_PLAY_ANIM(
								fleecaNpcs[i],
								kneelDict,
								kneelAnim,
								8.0f,
								-8.0f,
								-1,
								1, // loop
								0,
								0,
								0,
								0
							);
							hostagesKneel[i] = true;
						}
					}
				}
			}

			// ALARME E SEQUÊNCIA APÓS REFÉNS LEVANTAREM AS MÃOS
			if (alarmSequenceStarted && hostagesHandsUp && !alarmTriggered) {
				// 🔔 Alarme do banco
				START_ALARM("FLEECA_BANK_ALARMS", 0);

				// 🎧 Lester – primeira fala
				ShowLesterMsgScript02(
					"Vai rápido! Abre o cofre com o cartão que eu te dei, rápido, rápido!",
					"Lester"
				);

				WAIT(25000);

				// 🎧 Lester – segunda fala
				ShowLesterMsgScript02(
					"Não posso segurar a polícia por muito tempo, anda logo!",
					"Lester"
				);

				WAIT(15000);

				// 🚓 Polícia chega
				SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 5, 0);
				SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 1);

				// 🎧 Lester – última fala
				ShowLesterMsgScript02(
					"A polícia está a caminho, sai daí agora!",
					"Lester"
				);

				SET_VEHICLE_ENGINE_ON(getawayCar, 1, 1, 0);
				SET_VEHICLE_HANDBRAKE(getawayCar, 0);
				SET_VEHICLE_UNDRIVEABLE(getawayCar, 0);

				alarmTriggered = true;
			}


			// Quando perder a polícia, liberar entrega
			if (alarmTriggered && !escapeComplete &&
				GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) == 0) {

				escapeComplete = true;

				RemoveFleecaBlip();
				SET_WAYPOINT_OFF();
				CreateLesterFactoryBlip();

				vector3 dropPos = {DELIVERY_X_SCRIPT06, DELIVERY_Y_SCRIPT06, DELIVERY_Z_SCRIPT06 - 0.6f};
				vector3 dir = {0.0f, 0.0f, 0.0f};
				RGBA color = {0, 255, 0, 180};

				deliveryCheckpoint = CREATE_CHECKPOINT(
					4,        // pequeno, no chão
					dropPos,
					dir,
					1.2f,     // raio pequeno
					color,
					0
				);

				ShowLesterMsgScript02(
					"Entrega a grana no local marcado.",
					"Lester"
				);

				canDeliverLoot = true;
			}

			// Player chega na fábrica
			if (escapeComplete && !playerDelivered &&
				IsPlayerInGetawayCar() &&
				GetDist(ppos, DELIVERY_X_SCRIPT06, DELIVERY_Y_SCRIPT06, DELIVERY_Z_SCRIPT06) < 10.0f) {

				playerDelivered = true;
				RemoveDeliveryBlip();
				CreateAssaultBlip();

				ShowLesterMsgScript02(
					"Aproxime-se do local e aperte ~INPUT_FRONTEND_A~ para colocar o dinheiro.",
					"Lester"
				);

				showPlaceLoot = true;
			}


			// Depositar dinheiro
			if (canDeliverLoot && !lootPlaced) {
				float dist = GetDist(ppos, DELIVERY_X_SCRIPT06, DELIVERY_Y_SCRIPT06, DELIVERY_Z_SCRIPT06);
				if (dist < 2.5f) {
					FloatingHelpText("Aperte ~INPUT_FRONTEND_ACCEPT~ para entregar o dinheiro");
					if (IS_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_ACCEPT)) {
						lootPlaced = true;
						canDeliverLoot = false;
						FloatingHelpText("");

						// 🔹 spawn prop de dinheiro
						Hash moneyHash = GET_HASH_KEY("prop_money_bag_01");
						REQUEST_MODEL(moneyHash);
						while (!HAS_MODEL_LOADED(moneyHash)) WAIT(0);

						deliveryMoneyProp = CREATE_OBJECT(
							moneyHash,
							DELIVERY_X_SCRIPT06,
							DELIVERY_Y_SCRIPT06,
							DELIVERY_Z_SCRIPT06 - 1.0f,
							1, 1, 0
						);

						SET_ENTITY_AS_MISSION_ENTITY(deliveryMoneyProp, 1, 1);
						PLACE_OBJECT_ON_GROUND_PROPERLY(deliveryMoneyProp);
						FREEZE_ENTITY_POSITION(deliveryMoneyProp, 1);
						SET_ENTITY_COLLISION(deliveryMoneyProp, 0, 0);

						if (deliveryCheckpoint) {
							DELETE_CHECKPOINT(deliveryCheckpoint);
							deliveryCheckpoint = 0;
						}
						// Remove blip e GPS da missão
						RemoveAssaultBlip();
						SET_WAYPOINT_OFF();

						// Desliga e trava o carro usado
						if (getawayCar) {
							SET_VEHICLE_ENGINE_ON(getawayCar, 0, 1, 1);
							SET_VEHICLE_UNDRIVEABLE(getawayCar, 1);
							SET_VEHICLE_DOORS_LOCKED(getawayCar, 2); // 2 = locked
						}

						ShowLesterMsgScript02(
							"Perfeito. Agora deixa comigo.",
							"Lester"
						);
					}
				}
			}

			// Final da missão
			if (lootPlaced && !missionFinished &&
				GetDist(ppos, ASSAULT_ROOM_X_SCRIPT06, ASSAULT_ROOM_Y_SCRIPT06, ASSAULT_ROOM_Z_SCRIPT06) > 15.0f) {

				ShowLesterMsgScript02(
					"Bom trabalho. Vou lavar esse dinheiro e te mandar sua parte.",
					"Lester"
				);

				missionFinished = true;
				tickAfterLoot = 0;
				WAIT(2000);
				//MissionPassed("Assalto ao Fleeca");
			}

			// Encerrar script
			if (missionFinished) {
				tickAfterLoot++;
				if (tickAfterLoot > 150) {
					if (deliveryMoneyProp) {
						DELETE_OBJECT(&deliveryMoneyProp);
						deliveryMoneyProp = 0;
					}
					// Encerra todos os scripts "door"
					TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("door");
					WAIT(1000);
					script06Finished = true;
				}
			}

			WAIT(20);
		}

// Start/Update for Script06Final

void UpdateScript06Final()
{
	// watches missionFinished and after a short period grants money and terminates
	static int localTick = 0;
	if (missionFinished) {
		localTick++;
		if (localTick > 150) {
			// grant money and show final notifications (reusing helpers)
			ShowLesterMsgScript02("Lester: O dinheiro foi transferido! Fique atento para os proximos golpes!!", "Lester");
			AddMoneyToCurrentCharacter(250000);
			WAIT(5000);
			// final phone notification
			_SET_NOTIFICATION_TEXT_ENTRY("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("script feito com carinho para a comunidade 360!~n~creditos reservados~n~definitive edition 2026");
			_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2("CHAR_MP_FM_CONTACT", "CHAR_MP_FM_CONTACT", 1, 7, "Definitive", "Edição", 1, "", 8);
			_DRAW_NOTIFICATION(8000, 1);
			script06FinalFinished = true;
		}
	}
}

// ===== Main sequencer =====
void main()
{
	//SET_MISSION_FLAG(1);
	//NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	while (true)
	{
		switch (g_CurrentSequence)
		{
		case SEQ_SCRIPT01:
			if (!script01Started) StartScript01();
			UpdateScript01();
			if (script01Finished) {
				g_CurrentSequence = SEQ_SCRIPT02;
			}
			break;

		case SEQ_SCRIPT02:
			if (!script02Started) StartScript02();
			UpdateScript02();
			if (script02Finished) {
				g_CurrentSequence = SEQ_SCRIPT03;
			}
			break;

		case SEQ_SCRIPT03:
			if (!script03Started) StartScript03();
			UpdateScript03();
			if (script03Finished) {
				g_CurrentSequence = SEQ_SCRIPT04;
			}
			break;

		case SEQ_SCRIPT04:
			if (!script04Started) StartScript04();
			UpdateScript04();
			if (script04Finished) {
				g_CurrentSequence = SEQ_SCRIPT05;
			}
			break;

		case SEQ_SCRIPT05:
			if (!script05Started) StartScript05();
			UpdateScript05();
			if (script05Finished) {
				g_CurrentSequence = SEQ_SCRIPT06;
			}
			break;

		case SEQ_SCRIPT06:
			if (!script06Started) StartScript06();
			UpdateScript06();
			if (script06Finished) {
				g_CurrentSequence = SEQ_SCRIPT06_FINAL;
			}
			break;

		case SEQ_SCRIPT06_FINAL:
			if (!script06FinalStarted) StartScript06Final();
			UpdateScript06Final();
			if (script06FinalFinished) {
				TERMINATE_THIS_THREAD();
			}
			break;

		default:
			break;
		}

		WAIT(20);
	}
}