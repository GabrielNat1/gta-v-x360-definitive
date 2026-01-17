#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Coordenadas da fábrica do Lester
#define LESTER_FACTORY_X 707.85f
#define LESTER_FACTORY_Y -966.62f
#define LESTER_FACTORY_Z 30.40f

// Coordenadas para entregar o carro (pode ajustar para o local exato desejado)
#define DELIVERY_X 713.0f
#define DELIVERY_Y -968.0f
#define DELIVERY_Z 30.40f

enum Buttons { Button_A = 0xC1 };

int getawayCar = 0;
int getawayDriver = 0;
int carBlip = 0;
int deliveryBlip = 0;
bool carStolen = false;
bool deliveryActive = false;
bool deliveryDone = false;
bool messageShown = false;
bool waitingToFinish = false;
int tickSinceDelivery = 0;

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
	// Carro aleatório: "buffalo"
	Hash carHash = GET_HASH_KEY("buffalo");
	REQUEST_MODEL(carHash);
	while (!HAS_MODEL_LOADED(carHash)) WAIT(0);
	vector3 spawn = {LESTER_FACTORY_X + 300.0f, LESTER_FACTORY_Y + 800.0f, LESTER_FACTORY_Z + 1.0f}; // spawn longe da fábrica
	getawayCar = CREATE_VEHICLE(carHash, spawn, 180.0f, 1, 1);
	SET_VEHICLE_ON_GROUND_PROPERLY(getawayCar);
	SET_VEHICLE_NUMBER_PLATE_TEXT(getawayCar, "HEIST");
	SET_VEHICLE_IS_WANTED(getawayCar, 0);
	SET_VEHICLE_ENGINE_ON(getawayCar, 1, 1, 0);
	SET_VEHICLE_FORWARD_SPEED(getawayCar, 40.0f);

	// NPC dirigindo
	Hash pedHash = GET_HASH_KEY("a_m_y_roadcyc_01");
	REQUEST_MODEL(pedHash);
	while (!HAS_MODEL_LOADED(pedHash)) WAIT(0);
	getawayDriver = CREATE_PED_INSIDE_VEHICLE(getawayCar, 26, pedHash, -1, 1, 1);
	TASK_VEHICLE_DRIVE_WANDER(getawayDriver, getawayCar, 40.0f, 786603); // Alta velocidade

	// Blip do carro
	carBlip = ADD_BLIP_FOR_ENTITY(getawayCar);
	SET_BLIP_SPRITE(carBlip, 225);
	SET_BLIP_COLOUR(carBlip, 5);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Carro do Plano");
	END_TEXT_COMMAND_SET_BLIP_NAME(carBlip);
}

void RemoveGetawayCar() {
	if (carBlip) { REMOVE_BLIP(&carBlip); carBlip = 0; }
	if (getawayDriver) { DELETE_PED(&getawayDriver); getawayDriver = 0; }
	if (getawayCar) { DELETE_VEHICLE(&getawayCar); getawayCar = 0; }
}

void CreateDeliveryBlip() {
	if (deliveryBlip) return;
	deliveryBlip = ADD_BLIP_FOR_COORD(DELIVERY_X, DELIVERY_Y, DELIVERY_Z);
	SET_BLIP_SPRITE(deliveryBlip, 1);
	SET_BLIP_COLOUR(deliveryBlip, 2);
	SET_BLIP_ROUTE(deliveryBlip, 1);
	BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Entregar Carro");
	END_TEXT_COMMAND_SET_BLIP_NAME(deliveryBlip);
}

void RemoveDeliveryBlip() {
	if (deliveryBlip) { REMOVE_BLIP(&deliveryBlip); deliveryBlip = 0; }
}

bool IsPlayerInGetawayCar() {
	int player = PLAYER_PED_ID();
	return (IS_PED_IN_VEHICLE(player, getawayCar, 0));
}

float GetDistToDelivery() {
	vector3 pos = GET_ENTITY_COORDS(getawayCar, 1);
	float dx = pos.x - DELIVERY_X;
	float dy = pos.y - DELIVERY_Y;
	float dz = pos.z - DELIVERY_Z;
	return SQRT(dx*dx + dy*dy + dz*dz);
}

void main() {
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	ShowLesterMsg("É urgente! Desculpe não avisar antes. Um carro essencial para o plano está rondando Los Santos. Capture-o e leve até minha fábrica!", "URGENTE");
	WAIT(2000);

	SpawnGetawayCar();

	while (true) {
		if (!carStolen && getawayCar && IsPlayerInGetawayCar()) {
			carStolen = true;
			if (carBlip) { SET_BLIP_ROUTE(carBlip, 0); }
			ShowLesterMsg("Leve o carro até a fábrica. O local está marcado no GPS!", "Plano");
			CreateDeliveryBlip();
			deliveryActive = true;
		}

		if (deliveryActive && getawayCar) {
			// Mostra marker no local de entrega
			vector3 markerPos = {DELIVERY_X, DELIVERY_Y, DELIVERY_Z};
			vector3 zero = {0.0f, 0.0f, 0.0f};
			vector3 scale = {2.5f, 2.5f, 1.0f};
			RGBA blue = {0, 150, 255, 120};
			DRAW_MARKER(1, markerPos, zero, zero, scale, blue, 0, 0, 2, 0, 0, 0, 0);

			// Se carro está em cima do marker
			if (!deliveryDone && GetDistToDelivery() < 3.0f && IsPlayerInGetawayCar()) {
				SET_VEHICLE_ENGINE_ON(getawayCar, 0, 1, 0);
				TASK_LEAVE_VEHICLE(PLAYER_PED_ID(), getawayCar, 0);
				FloatingHelpText("Saia do carro!");
				deliveryDone = true;
				RemoveDeliveryBlip();
				if (carBlip) { REMOVE_BLIP(&carBlip); carBlip = 0; }
				waitingToFinish = true;
				tickSinceDelivery = 0;
			}
		}

		// Após sair do carro e se afastar, remove carro/blip/mensagem e finaliza
		if (waitingToFinish && deliveryDone) {
			tickSinceDelivery++;
			int player = PLAYER_PED_ID();
			vector3 ppos = GET_ENTITY_COORDS(player, 1);
			float dx = ppos.x - DELIVERY_X;
			float dy = ppos.y - DELIVERY_Y;
			float dz = ppos.z - DELIVERY_Z;
			float dist = SQRT(dx*dx + dy*dy + dz*dz);
			if (dist > 15.0f) {
				RemoveGetawayCar();
				FloatingHelpText(""); // limpa helptext
				ShowLesterMsg("Perfeito! Obrigado, agora estamos prontos para a próxima etapa.", "Lester");
				WAIT(2000);
				REQUEST_SCRIPT("lester_mission_heist_fleeca_03");
				while (!HAS_SCRIPT_LOADED("lester_mission_heist_fleeca_03")) WAIT(0);
				START_NEW_SCRIPT("lester_mission_heist_fleeca_03", 1024);
				SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_fleeca_03");
				TERMINATE_THIS_THREAD();
			}
		}

		WAIT(20);
	}
}
