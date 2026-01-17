#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Locais secretos
typedef struct {
	float x, y, z;
	const char* name;
	int blip;
	bool itemTaken;
} SecretSpot;

#define SPOT_COUNT 6
SecretSpot spots[SPOT_COUNT] = {
	{1985.8f, 3049.1f, 47.2f, "Bar do Trevor", 0, false},         // Bar do Trevor (Missão banheira gelo)
	{170.0f, -1660.0f, 29.8f, "Dardos", 0, false},                // Dardos
	{-695.0f, 967.0f, 232.0f, "Epsilon", 0, false},               // Epsilon
	{642.0f, -2797.0f, 6.0f, "Submarino", 0, false},              // Submarino
	{501.0f, 5604.0f, 797.0f, "Monte Chiliad", 0, false},         // Monte Chiliad
	{-1500.0f, 4390.0f, 20.0f, "Montanhas Trevor", 0, false}      // Lugar das montanhas (Trevor matava pessoas)
};

int activeSpot = -1;
bool itemCollected = false;
bool npcsSpawned = false;
bool propsSpawned = false;
bool shootoutStarted = false;
int npcs[6] = {0};
int props[4] = {0};
int itemObj = 0;
int tickSinceItem = 0;
vector3 lastItemPos = {0};
bool waitingToCallNext = false;

enum Buttons { Button_A = 0xC1 };

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

void CreateBlips() {
	for (int i = 0; i < SPOT_COUNT; ++i) {
		if (spots[i].blip == 0) {
			spots[i].blip = ADD_BLIP_FOR_COORD(spots[i].x, spots[i].y, spots[i].z);
			SET_BLIP_SPRITE(spots[i].blip, 280); // Blip tipo ponto de interesse
			SET_BLIP_COLOUR(spots[i].blip, 3);
			SET_BLIP_SCALE(spots[i].blip, 0.9f);
			BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(spots[i].name);
			END_TEXT_COMMAND_SET_BLIP_NAME(spots[i].blip);
		}
	}
}

float GetPlayerDistTo(float x, float y, float z) {
	int player = PLAYER_PED_ID();
	vector3 pos = GET_ENTITY_COORDS(player, 1);
	float dx = pos.x - x;
	float dy = pos.y - y;
	float dz = pos.z - z;
	return SQRT(dx*dx + dy*dy + dz*dz);
}

void SpawnNPCsAndProps(vector3 pos) {
	if (npcsSpawned) return;
	vector3 v0 = {pos.x+2.0f, pos.y+1.0f, pos.z};
	vector3 v1 = {pos.x-2.0f, pos.y-1.0f, pos.z};
	vector3 v2 = {pos.x+1.5f, pos.y-2.0f, pos.z};
	npcs[0] = CREATE_PED(26, GET_HASH_KEY("a_m_m_hillbilly_01"), v0, 90.0f, 1, 1);
	npcs[1] = CREATE_PED(26, GET_HASH_KEY("a_m_m_hillbilly_02"), v1, 180.0f, 1, 1);
	npcs[2] = CREATE_PED(26, GET_HASH_KEY("a_f_y_vinewood_04"), v2, 270.0f, 1, 1);
	TASK_START_SCENARIO_IN_PLACE(npcs[0], "WORLD_HUMAN_SMOKING", 0, 1);
	TASK_START_SCENARIO_IN_PLACE(npcs[1], "WORLD_HUMAN_PARTYING", 0, 1);
	TASK_START_SCENARIO_IN_PLACE(npcs[2], "WORLD_HUMAN_CHEERING", 0, 1);
	props[0] = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_box_wood02a_pu"), pos.x+1.0f, pos.y+1.0f, pos.z, 1, 1, 0);
	props[1] = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_box_wood01a"), pos.x-1.0f, pos.y-1.0f, pos.z, 1, 1, 0);
	props[2] = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_chair_01a"), pos.x+0.5f, pos.y-1.5f, pos.z, 1, 1, 0);
	props[3] = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_beer_bottle"), pos.x-0.5f, pos.y+1.5f, pos.z+0.5f, 1, 1, 0);
	npcsSpawned = true;
	propsSpawned = true;
}

void DespawnNPCsAndProps() {
	for (int i = 0; i < 6; ++i) {
		if (npcs[i]) { DELETE_PED(&npcs[i]); npcs[i] = 0; }
	}
	for (int i = 0; i < 4; ++i) {
		if (props[i]) { DELETE_OBJECT(&props[i]); props[i] = 0; }
	}
	if (itemObj) { DELETE_OBJECT(&itemObj); itemObj = 0; }
	npcsSpawned = false;
	propsSpawned = false;
	shootoutStarted = false;
}

void SpawnItem(vector3 pos) {
	if (itemObj) return;
	itemObj = CREATE_OBJECT_NO_OFFSET(GET_HASH_KEY("prop_ld_keypad_01"), pos.x, pos.y, pos.z+0.1f, 1, 1, 0);
	SET_ENTITY_AS_MISSION_ENTITY(itemObj, 1, 1);
}

void RemoveItem() {
	if (itemObj) { DELETE_OBJECT(&itemObj); itemObj = 0; }
}

bool PlayerNear(vector3 pos, float dist) {
	return GetPlayerDistTo(pos.x, pos.y, pos.z) < dist;
}

void StartShootout() {
	if (shootoutStarted) return;
	for (int i = 0; i < 3; ++i) {
		GIVE_WEAPON_TO_PED(npcs[i], GET_HASH_KEY("WEAPON_PISTOL"), 100, 0, 1);
		TASK_COMBAT_PED(npcs[i], PLAYER_PED_ID(), 0, 16);
	}
	shootoutStarted = true;
}

void CheckMontanhasSpot() {
	vector3 pos = {spots[5].x, spots[5].y, spots[5].z};
	if (PlayerNear(pos, 80.0f)) {
		if (!npcsSpawned) {
			SpawnNPCsAndProps(pos);
			SpawnItem((vector3){pos.x, pos.y, pos.z+0.2f});
		}
		// DRAW_MARKER(int type, vector3 pos, vector3 dir, vector3 rot, vector3 scale, RGBA colour, BOOL bobUpAndDown, BOOL faceCamera, int p19, BOOL rotate, char* textureDict, char* textureName, BOOL drawOnEnts)
		vector3 markerPos = {pos.x, pos.y, pos.z+1.0f};
		vector3 zero = {0.0f, 0.0f, 0.0f};
		vector3 scale = {0.5f, 0.5f, 0.5f};
		RGBA yellow = {255, 255, 0, 180};
		DRAW_MARKER(1, markerPos, zero, zero, scale, yellow, 0, 0, 2, 0, 0, 0, 0);
		if (!itemCollected && PlayerNear(pos, 2.0f)) {
			FloatingHelpText("Pressione ~INPUT_FRONTEND_A~ para pegar o cartão");
			if (!shootoutStarted) StartShootout();
			if (IS_CONTROL_JUST_PRESSED(0, Button_A) && shootoutStarted) {
				itemCollected = true;
				RemoveItem();
				ShowLesterMsg("Boa, eu sempre acreditei que voce conseguiria! Agora leve ate o laboratorio novamente.", "Sucesso!");
				waitingToCallNext = true;
				lastItemPos = pos;
				DespawnNPCsAndProps();
			}
		}
		if (npcsSpawned && !itemCollected && !PlayerNear(pos, 100.0f)) {
			DespawnNPCsAndProps();
			RemoveItem();
		}
	} else {
		DespawnNPCsAndProps();
		RemoveItem();
	}
}

void main()
{
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	ShowLesterMsg("Adicionei alguns pontos no mapa. Verifique cada um, um deles terá o item que precisamos.", "Atenção!");
	WAIT(3000);
	ShowLesterMsg("Após pegar, volte para a fábrica, deixe o item lá e saia. Irei te chamar quando conseguir avanços.", "Instruções");

	CreateBlips();

	while (true)
	{
		CheckMontanhasSpot();

		if (waitingToCallNext && itemCollected) {
			tickSinceItem++;
			if (tickSinceItem > 150) {
				REQUEST_SCRIPT("lester_mission_heist_01_final");
				while (!HAS_SCRIPT_LOADED("lester_mission_heist_01_final")) WAIT(0);
				START_NEW_SCRIPT("lester_mission_heist_01_final", 1024);
				SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_01_final");
				waitingToCallNext = false;
			}
			if (!PlayerNear(lastItemPos, 60.0f)) {
				TERMINATE_THIS_THREAD();
			}
		}

		WAIT(20);
	}
}
