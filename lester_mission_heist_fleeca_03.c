#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Coordenadas principais
#define LESTER_FACTORY_X 707.85f
#define LESTER_FACTORY_Y -966.62f
#define LESTER_FACTORY_Z 30.40f

#define DELIVERY_X 713.0f
#define DELIVERY_Y -968.0f
#define DELIVERY_Z 30.40f

#define FLEECA_BLIP_X -2964.6704f
#define FLEECA_BLIP_Y  482.9354f
#define FLEECA_BLIP_Z   15.7068f

#define FLEECA_DOOR_X -2960.0f
#define FLEECA_DOOR_Y  484.0f
#define FLEECA_DOOR_Z   15.7f

#define ASSAULT_ROOM_X 710.0f
#define ASSAULT_ROOM_Y -965.0f
#define ASSAULT_ROOM_Z 30.40f

enum Buttons { Button_A = 0xC1 };

int getawayCar = 0;
int carBlip = 0;
int fleecaBlip = 0;
int deliveryBlip = 0;
int assaultBlip = 0;
int moneyBags[3] = {0};
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

void SpawnGetawayCar() {
	if (getawayCar) return;
	Hash carHash = GET_HASH_KEY("buffalo");
	REQUEST_MODEL(carHash);
	while (!HAS_MODEL_LOADED(carHash)) WAIT(0);
	vector3 spawn = {DELIVERY_X, DELIVERY_Y, DELIVERY_Z};
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
	SET_BLIP_COLOUR(carBlip, 5);
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
	fleecaBlip = ADD_BLIP_FOR_COORD(FLEECA_BLIP_X, FLEECA_BLIP_Y, FLEECA_BLIP_Z);
	SET_BLIP_SPRITE(fleecaBlip, 500);
	SET_BLIP_COLOUR(fleecaBlip, 1);
	SET_BLIP_ROUTE(fleecaBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Banco Fleeca");
	END_TEXT_COMMAND_SET_BLIP_NAME(fleecaBlip);
}

void RemoveFleecaBlip() {
	if (fleecaBlip) { REMOVE_BLIP(&fleecaBlip); fleecaBlip = 0; }
}

void CreateDeliveryBlip() {
	if (deliveryBlip) return;
	deliveryBlip = ADD_BLIP_FOR_COORD(DELIVERY_X, DELIVERY_Y, DELIVERY_Z);
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
	assaultBlip = ADD_BLIP_FOR_COORD(ASSAULT_ROOM_X, ASSAULT_ROOM_Y, ASSAULT_ROOM_Z);
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

void SpawnFleecaNPCs() {
	// Spawn NPCs do banco (exemplo: 3 civis)
	vector3 npcsPos[3] = {
		{FLEECA_BLIP_X+2.0f, FLEECA_BLIP_Y+1.0f, FLEECA_BLIP_Z},
		{FLEECA_BLIP_X-1.5f, FLEECA_BLIP_Y-2.0f, FLEECA_BLIP_Z},
		{FLEECA_BLIP_X+0.5f, FLEECA_BLIP_Y+2.0f, FLEECA_BLIP_Z}
	};
	for (int i = 0; i < 3; ++i) {
		Hash pedHash = GET_HASH_KEY("a_m_y_business_01");
		REQUEST_MODEL(pedHash);
		while (!HAS_MODEL_LOADED(pedHash)) WAIT(0);
		int npc = CREATE_PED(26, pedHash, npcsPos[i], 0.0f, 1, 1);
		TASK_HANDS_UP(npc, -1, PLAYER_PED_ID(), -1, 1);
	}
}

void SpawnMoneyBags() {
	if (bagsSpawned) return;
	vector3 bagPos[3] = {
		{ASSAULT_ROOM_X+0.5f, ASSAULT_ROOM_Y, ASSAULT_ROOM_Z},
		{ASSAULT_ROOM_X-0.5f, ASSAULT_ROOM_Y+0.5f, ASSAULT_ROOM_Z},
		{ASSAULT_ROOM_X, ASSAULT_ROOM_Y-0.5f, ASSAULT_ROOM_Z}
	};
	for (int i = 0; i < 3; ++i) {
		moneyBags[i] = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_money_bag_01"), bagPos[i].x, bagPos[i].y, bagPos[i].z, 1, 1, 0);
		SET_ENTITY_AS_MISSION_ENTITY(moneyBags[i], 1, 1);
	}
	bagsSpawned = true;
}

void RemoveMoneyBags() {
	for (int i = 0; i < 3; ++i) {
		if (moneyBags[i]) { DELETE_OBJECT(&moneyBags[i]); moneyBags[i] = 0; }
	}
	bagsSpawned = false;
}

void RestartMission() {
	RemoveGetawayCar();
	RemoveCarBlip();
	RemoveFleecaBlip();
	RemoveDeliveryBlip();
	RemoveAssaultBlip();
	RemoveMoneyBags();
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
}

bool MissionFailed() {
	int player = PLAYER_PED_ID();
	// Falha se player morreu
	if (IS_PLAYER_DEAD(player)) return true;
	// Falha se for preso
	if (IS_PLAYER_BEING_ARRESTED(PLAYER_ID(), 1)) return true;
	// Falha se getawayCar destruído
	if (getawayCar && !DOES_ENTITY_EXIST(getawayCar)) return true;
	return false;
}

void main() {
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	while (tick < 9000) { WAIT(20); tick++; }

retry:
	ShowLesterMsg("Está tudo pronto, venha até a fábrica e pegue o carro!", "Lester");
	WAIT(2000);
	ShowLesterMsg("Desculpe não estar aí, estava ocupado hackeando as câmeras do banco em casa.", "Lester");

	SpawnGetawayCar();
	CreateCarBlip();

	while (true) {
		int player = PLAYER_PED_ID();
		vector3 ppos = GET_ENTITY_COORDS(player, 1);

		// Falha: morreu, preso, carro destruído
		if (MissionFailed()) {
			ShowLesterMsg("Você falhou! Tente novamente.", "Lester");
			WAIT(2000);
			RestartMission();
			goto retry;
		}

		// Player entra no carro
		if (!carReady && IsPlayerInGetawayCar()) {
			carReady = true;
			RemoveCarBlip();
			CreateFleecaBlip();
			ShowLesterMsg("Dirija até o Banco Fleeca!", "Plano");
		}

		// Chegou no Fleeca
		if (carReady && !atFleeca && GetDist(ppos, FLEECA_BLIP_X, FLEECA_BLIP_Y, FLEECA_BLIP_Z) < 30.0f) {
			atFleeca = true;
			RemoveFleecaBlip();
			ShowLesterMsg("Estacione perto do banco!", "Plano");
		}

		// Player estaciona perto do banco
		if (atFleeca && !alarmTriggered && GetDist(ppos, FLEECA_DOOR_X, FLEECA_DOOR_Y, FLEECA_DOOR_Z) < 8.0f) {
			alarmTriggered = true;
			TRIGGER_ALARM("FLEECA_BANK_ALARMS"); // pode ser necessário ajustar o nome do alarme
			ADD_POLICE_WANTED_LEVEL(PLAYER_ID(), 5);
			APPLY_WANTED_LEVEL_CHANGE_NOW(PLAYER_ID());
			ShowLesterMsg("Está tudo pronto, invada! Seja rápido, a polícia está a caminho!", "Lester");
			SpawnFleecaNPCs();
		}

		// Quando pegar 5 estrelas, Lester manda mensagem
		if (alarmTriggered && !hasFiveStars && GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) >= 5) {
			hasFiveStars = true;
			ShowLesterMsg("Vai rápido, não temos tempo cara!", "Lester");
		}

		// Simula pegar o dinheiro e escapar
		if (alarmTriggered && !escapeStarted && GetDist(ppos, FLEECA_DOOR_X, FLEECA_DOOR_Y, FLEECA_DOOR_Z) < 8.0f && IsPlayerInGetawayCar()) {
			escapeStarted = true;
			ShowLesterMsg("Despiste a polícia!", "Plano");
		}

		// Quando perder a polícia, Lester manda mensagem para entregar na fábrica
		if (escapeStarted && !escapeComplete && GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) == 0) {
			escapeComplete = true;
			CreateDeliveryBlip();
			ShowLesterMsg("Entregue tudo na minha fábrica, preciso lavar esse dinheiro antes que a polícia nos pegue novamente!", "Lester");
		}

		// Player chega na fábrica para entregar
		if (escapeComplete && !playerDelivered && GetDist(ppos, DELIVERY_X, DELIVERY_Y, DELIVERY_Z) < 10.0f && IsPlayerInGetawayCar()) {
			playerDelivered = true;
			RemoveDeliveryBlip();
			CreateAssaultBlip();
			ShowLesterMsg("Aproxime-se do local e aperte ~INPUT_FRONTEND_A~ para colocar o dinheiro!", "Lester");
			showPlaceLoot = true;
		}

		// Player coloca o dinheiro
		if (showPlaceLoot && !lootPlaced && GetDist(ppos, ASSAULT_ROOM_X, ASSAULT_ROOM_Y, ASSAULT_ROOM_Z) < 3.0f) {
			FloatingHelpText("Aperte ~INPUT_FRONTEND_A~ para colocar o dinheiro aqui");
			if (IS_CONTROL_JUST_PRESSED(0, Button_A)) {
				lootPlaced = true;
				RemoveAssaultBlip();
				SpawnMoneyBags();
				FloatingHelpText("Saia!");
			}
		}

		// Player sai da fábrica
		if (lootPlaced && !missionFinished && GetDist(ppos, ASSAULT_ROOM_X, ASSAULT_ROOM_Y, ASSAULT_ROOM_Z) > 15.0f) {
			RemoveMoneyBags();
			FloatingHelpText("");
			ShowLesterMsg("Obrigado por tudo! Quando tudo estiver pronto, vou transferir uma parte do dinheiro para você.", "Lester");
			missionFinished = true;
			tickAfterLoot = 0;
		}

		// Finaliza thread após alguns segundos e chama próximo script
		if (missionFinished) {
			tickAfterLoot++;
			if (tickAfterLoot > 150) {
				REQUEST_SCRIPT("lester_mission_heist_fleeca_final_03");
				while (!HAS_SCRIPT_LOADED("lester_mission_heist_fleeca_final_03")) WAIT(0);
				START_NEW_SCRIPT("lester_mission_heist_fleeca_final_03", 1024);
				SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_fleeca_final_03");
				TERMINATE_THIS_THREAD();
			}
		}

		WAIT(20);
	}
}
