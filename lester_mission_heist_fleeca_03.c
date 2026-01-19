#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Coordenadas principais
#define LESTER_FACTORY_X 707.85f
#define LESTER_FACTORY_Y -966.62f
#define LESTER_FACTORY_Z 30.40f

// Altere aqui para a nova posição de entrega:
#define DELIVERY_X 706.73f
#define DELIVERY_Y -961.12f
#define DELIVERY_Z 30.0f

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

// Dicionários de animação usados

// Animações diferentes para cada NPC
#define ANIM_IDLE_1 "amb@world_human_stand_mobile@male@standing@call@base"
#define ANIM_IDLE_2 "amb@world_human_hang_out_street@male_b@idle_a"
#define ANIM_IDLE_3 "amb@world_human_clipboard@male@idle_a"

#define ANIM_HANDSUP "random@arrests"
#define ANIM_HANDSUP_NAME "idle_a"

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
int deliveryMoneyProp = 0;
bool hostagesReacted = false;
int alarmSequenceTimer = 0;

// Controle para ajoelhar/deitar após 10s
bool hostagesKneel[2] = {false, false};
int hostagesKneelTimer = 0;

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
        LESTER_FACTORY_X,
        LESTER_FACTORY_Y,
        LESTER_FACTORY_Z
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
	RemoveMoneyBags();
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
	fleecaNpcsSpawned = false;
	hostagesHandsUp = false;
	alarmSequenceStarted = false;
	deliveryCheckpoint = 0;
	canDeliverLoot = false;
	deliveryMoneyProp = 0;
	hostagesReacted = false;
	alarmSequenceTimer = 0;
	hostagesKneel[0] = false;
	hostagesKneel[1] = false;
	hostagesKneelTimer = 0;
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

void main() {
	SET_MISSION_FLAG(1);
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

retry:
	ShowLesterMsg("Está tudo pronto, venha até a fábrica e pegue o carro!", "Lester");
	WAIT(2000);
	ShowLesterMsg("Desculpe não estar aí, estava ocupado hackeando as câmeras do banco em casa.", "Lester");

	SpawnGetawayCar();
	CreateCarBlip();

	while (true) {

		BlockAmbientEvents();

		if (MissionFailed()) {
			WAIT(30000);
			ShowLesterMsg("Droga! Deu tudo errado! tente novamente", "Lester");
			WAIT(20000);

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
			WAIT(3000);
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

		// NPCs só spawnam quando player está próximo do banco
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
			ShowLesterMsg(
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
			ShowLesterMsg(
				"Vai rápido! Abre o cofre com o cartão que eu te dei, rápido, rápido!",
				"Lester"
			);

			WAIT(25000);

			// 🎧 Lester – segunda fala
			ShowLesterMsg(
				"Não posso segurar a polícia por muito tempo, anda logo!",
				"Lester"
			);

			WAIT(15000);

			// 🚓 Polícia chega
			SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 5, 0);
			SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), 1);

			// 🎧 Lester – última fala
			ShowLesterMsg(
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

					// 🔹 spawn prop de dinheiro
					Hash moneyHash = GET_HASH_KEY("prop_money_bag_01");
					REQUEST_MODEL(moneyHash);
					while (!HAS_MODEL_LOADED(moneyHash)) WAIT(0);

					deliveryMoneyProp = CREATE_OBJECT(
						moneyHash,
						DELIVERY_X,
						DELIVERY_Y,
						DELIVERY_Z - 1.0f,
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

					ShowLesterMsg(
						"Perfeito. Agora deixa comigo.",
						"Lester"
					);
				}
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