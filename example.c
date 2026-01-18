#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Coordenadas principais
#define LESTER_FACTORY_X 707.85f
#define LESTER_FACTORY_Y -966.62f
#define LESTER_FACTORY_Z 30.40f

#define DELIVERY_X 713.01f
#define DELIVERY_Y -978.92f
#define DELIVERY_Z 23.73f

#define FLEECA_BLIP_X -2964.6704f
#define FLEECA_BLIP_Y  482.9354f
#define FLEECA_BLIP_Z   15.7068f

#define FLEECA_DOOR_X -2960.0f
#define FLEECA_DOOR_Y  484.0f
#define FLEECA_DOOR_Z   15.7f

#define FLEECA_VAULT_X -2960.0f
#define FLEECA_VAULT_Y  484.0f
#define FLEECA_VAULT_Z 	15.7f

#define ASSAULT_ROOM_X 710.0f
#define ASSAULT_ROOM_Y -965.0f
#define ASSAULT_ROOM_Z 30.40f

#define FLEECA_PARK_X -2973.6899f
#define FLEECA_PARK_Y  481.2466f
#define FLEECA_PARK_Z   14.8711f

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
    if (fleecaNpcsSpawned) return;

    vector3 npcsPos[3] = {
        {FLEECA_BLIP_X + 1.0f, FLEECA_BLIP_Y + 1.0f, FLEECA_BLIP_Z},
        {FLEECA_BLIP_X - 1.0f, FLEECA_BLIP_Y - 1.0f, FLEECA_BLIP_Z},
        {FLEECA_BLIP_X,        FLEECA_BLIP_Y + 2.0f, FLEECA_BLIP_Z}
    };

    Hash pedHash = GET_HASH_KEY("a_m_y_business_01");
    REQUEST_MODEL(pedHash);
    while (!HAS_MODEL_LOADED(pedHash)) WAIT(0);

    for (int i = 0; i < 3; i++) {
        fleecaNpcs[i] = CREATE_PED(
            26,
            pedHash,
            npcsPos[i],
            180.0f,
            1,
            1
        );

        SET_ENTITY_AS_MISSION_ENTITY(fleecaNpcs[i], 1, 1);

        // 🔒 trava total
        SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(fleecaNpcs[i], 1);
        SET_PED_FLEE_ATTRIBUTES(fleecaNpcs[i], 0, 0);
        SET_PED_COMBAT_ATTRIBUTES(fleecaNpcs[i], 46, 1);
        SET_PED_CAN_RAGDOLL(fleecaNpcs[i], 0);
    }

    fleecaNpcsSpawned = true;
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

    // Remove NPCs
    for (int i = 0; i < 3; i++) {
        if (fleecaNpcs[i]) {
            DELETE_PED(&fleecaNpcs[i]);
            fleecaNpcs[i] = 0;
        }
    }

    // Remove blips / checkpoints
    RemoveCarBlip();
    RemoveFleecaBlip();
    RemoveDeliveryBlip();

    if (parkCheckpoint) {
        DELETE_CHECKPOINT(parkCheckpoint);
        parkCheckpoint = 0;
    }

    // Remove wanted level
    SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, 0);
    SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 1);
}

bool MissionFailed() {
    int player = PLAYER_PED_ID();

    if (IS_ENTITY_DEAD(player)) return true;
    if (GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) >= 4 &&
        IS_PLAYER_BEING_ARRESTED(PLAYER_ID(), 1)) return true;

    return false;
}

void main() {
	SET_MISSION_FLAG(1);
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	//while (tick < 9000) { WAIT(20); tick++; }

retry:
	ShowLesterMsg("Está tudo pronto, venha até a fábrica e pegue o carro!", "Lester");
	WAIT(2000);
	ShowLesterMsg("Desculpe não estar aí, estava ocupado hackeando as câmeras do banco em casa.", "Lester");

	SpawnGetawayCar();
	CreateCarBlip();

	while (true) {

		if (MissionFailed()) {
			ShowLesterMsg("Droga! Deu tudo errado. Volta pra fábrica e tenta de novo.", "Lester");
			WAIT(3000);

			ResetMissionState();
			goto retry;
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
			ShowLesterMsg("Você falhou! Tente novamente.", "Lester");
			WAIT(2000);
			RestartMission();
			goto retry;
		}

		// Player entra no carro da missão
		if (!carReady && IsPlayerInGetawayCar()) {
			carReady = true;
			RemoveCarBlip();
			CreateFleecaBlip();
			ShowLesterMsg("Dirija até o Banco Fleeca.", "Plano");
		}

		if (carReady && !fleecaNpcsSpawned &&
			GetDist(ppos, FLEECA_DOOR_X, FLEECA_DOOR_Y, FLEECA_DOOR_Z) < 30.0f) {

			SpawnFleecaNPCs();
		}

		// Chegou na área do Fleeca
		if (carReady && !atFleeca &&
			GetDist(ppos, FLEECA_PARK_X, FLEECA_PARK_Y, FLEECA_PARK_Z) < 40.0f) {

			atFleeca = true;
			vector3 parkPos = {FLEECA_PARK_X, FLEECA_PARK_Y, FLEECA_PARK_Z - 0.5f};
			vector3 parkDir = {0.0f, 0.0f, 0.0f};
			RGBA parkColor = {255, 255, 0, 180};

			parkCheckpoint = CREATE_CHECKPOINT(
				4,            // tipo pequeno no chão
				parkPos,
				parkDir,
				1.0f,         // raio pequeno
				parkColor,
				0
			);

			ShowLesterMsg("Estacione o carro no local marcado.", "Lester");
		}

		// ETAPA 1 — estacionou perto do banco
		if (atFleeca && !parkedConfirmed &&
			IsPlayerInGetawayCar() &&
			GetDist(ppos, FLEECA_PARK_X, FLEECA_PARK_Y, FLEECA_PARK_Z) < 3.5f &&
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

				ShowLesterMsg("Pode ir. Coloque a máscara e entre no banco.", "Lester");
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
			GetDist(ppos, FLEECA_DOOR_X, FLEECA_DOOR_Y, FLEECA_DOOR_Z) < 5.0f) {

			SpawnFleecaNPCs();
			ShowLesterMsg(
				"Aponte a arma para os reféns e abra o cofre com o cartão.",
				"Lester"
			);

			enteredBank = true;
		}

		if (!alarmSequenceStarted &&
			!IsPlayerInGetawayCar() &&
			GetDist(ppos, FLEECA_DOOR_X, FLEECA_DOOR_Y, FLEECA_DOOR_Z) < 4.0f) {

			alarmSequenceStarted = true;

			// NPCs levantam as mãos
			for (int i = 0; i < 3; i++) {
				if (fleecaNpcs[i]) {
					TASK_HANDS_UP(
						fleecaNpcs[i],
						-1,
						PLAYER_PED_ID(),
						-1,
						1
					);

					SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(fleecaNpcs[i], 1);
				}
			}

			// 🔔 Alarme do banco
			START_ALARM("FLEECA_BANK_ALARMS", 0);

			// 🎧 Lester – primeira fala
			ShowLesterMsg(
				"Vai rápido! Abre o cofre com o cartão que eu te dei, rápido, rápido!",
				"Lester"
			);

			WAIT(10000); // 4 segundos

			// 🎧 Lester – segunda fala
			ShowLesterMsg(
				"Não posso segurar a polícia por muito tempo, anda logo!",
				"Lester"
			);

			WAIT(15000); // 2 segundos

			// 🚓 Polícia chega
			SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 5, 0);
			SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 1);

			// 🎧 Lester – última fala
			ShowLesterMsg(
				"A polícia está a caminho, sai daí agora!",
				"Lester"
			);

			alarmTriggered = true;
		}

		// Quando perder a polícia, liberar entrega
		if (alarmTriggered && !escapeComplete &&
			GET_PLAYER_WANTED_LEVEL(PLAYER_ID()) == 0) {

			escapeComplete = true;

			vector3 dropPos = {DELIVERY_X, DELIVERY_Y, DELIVERY_Z - 0.6f};
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

			ShowLesterMsg(
				"Entrega a grana no local marcado.",
				"Lester"
			);

			canDeliverLoot = true;
		}

		// Player chega na fábrica
		if (escapeComplete && !playerDelivered &&
			IsPlayerInGetawayCar() &&
			GetDist(ppos, DELIVERY_X, DELIVERY_Y, DELIVERY_Z) < 10.0f) {

			playerDelivered = true;
			RemoveDeliveryBlip();
			CreateAssaultBlip();

			ShowLesterMsg(
				"Aproxime-se do local e aperte ~INPUT_FRONTEND_A~ para colocar o dinheiro.",
				"Lester"
			);

			showPlaceLoot = true;
		}

		// Depositar dinheiro
		if (canDeliverLoot && !lootPlaced) {
			float dist = GetDist(ppos, DELIVERY_X, DELIVERY_Y, DELIVERY_Z);

			if (dist < 2.5f) {

				FloatingHelpText("Aperte ~INPUT_FRONTEND_ACCEPT~ para entregar o dinheiro");

				if (IS_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_ACCEPT)) {

					lootPlaced = true;
					canDeliverLoot = false;

					FloatingHelpText("");

					if (deliveryCheckpoint) {
						DELETE_CHECKPOINT(deliveryCheckpoint);
						deliveryCheckpoint = 0;
					}

					ShowLesterMsg(
						"Perfeito. Agora sai daí.",
						"Lester"
					);
				}

			} else {
				FloatingHelpText("");
			}
		}

		// Final da missão
		if (lootPlaced && !missionFinished &&
			GetDist(ppos, ASSAULT_ROOM_X, ASSAULT_ROOM_Y, ASSAULT_ROOM_Z) > 15.0f) {

			RemoveMoneyBags();
			ShowLesterMsg(
				"Bom trabalho. Vou lavar esse dinheiro e te mandar sua parte.",
				"Lester"
			);

			missionFinished = true;
			tickAfterLoot = 0;
		}

		// Encerrar script
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