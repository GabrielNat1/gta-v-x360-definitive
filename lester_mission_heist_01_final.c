#include "game.h" // Inclua headers necessários para funções de GTA/Script

#define LESTER_FACTORY_X 123.0f // Coordenadas fictícias, ajuste conforme necessário
#define LESTER_FACTORY_Y 456.0f
#define LESTER_FACTORY_Z 78.0f
#define BLIP_RADIUS 2.0f
#define DISTANCE_TO_LEAVE 50.0f

int blipId;
int propId;
bool itemPlaced = false;
bool messageShown = false;
bool boxShown = false;
bool playerLeft = false;
unsigned long leaveTime = 0;

void main() {
    // Criar blip no chão na área dos golpes
    blipId = CREATE_BLIP_FOR_COORD(LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z);

    while (true) {
        WAIT(0);

        float px, py, pz;
        GET_PLAYER_COORDS(PLAYER_ID(), &px, &py, &pz);

        float dist = GET_DISTANCE_BETWEEN_COORDS(px, py, pz, LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z);

        if (!itemPlaced && dist < BLIP_RADIUS) {
            if (!messageShown) {
                SHOW_HELP_TEXT("Coloque o item aqui");
                messageShown = true;
            }
            if (IS_CONTROL_JUST_PRESSED(0, 51)) { // E (INPUT_CONTEXT)
                propId = CREATE_OBJECT("prop_box_wood02a", LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z, true, true, false);
                itemPlaced = true;
                boxShown = true;
                CLEAR_HELP();
                SHOW_HELP_TEXT("Saia");
            }
        }

        if (itemPlaced && !playerLeft && dist > BLIP_RADIUS + 2.0f) {
            CLEAR_HELP();
            REMOVE_BLIP(blipId);
            SEND_PLAYER_MESSAGE("Lester: Obrigado! Quando tudo estiver pronto para o próximo plano, te aviso.");
            playerLeft = true;
            leaveTime = GET_GAME_TIMER();
        }

        if (playerLeft) {
            float distLeave = GET_DISTANCE_BETWEEN_COORDS(px, py, pz, LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z);
            if (distLeave > DISTANCE_TO_LEAVE) {
                if (GET_GAME_TIMER() - leaveTime > 60000) { // 1 minuto
                    START_NEW_SCRIPT("lester_mission_heist_02", 1024);
                    TERMINATE_THIS_THREAD();
                }
            }
        }
    }
}