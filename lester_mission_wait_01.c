#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// Coordenadas da fábrica do Lester (ajustadas)
#define LESTER_FACTORY_X 706.73f
#define LESTER_FACTORY_Y -965.00f
#define LESTER_FACTORY_Z 30.0f

enum Buttons { Button_B = 0xC3 };

bool missionContinued = false;

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

bool IsPlayerNearFactory() {
    int player = PLAYER_PED_ID();
    vector3 ppos = GET_ENTITY_COORDS(player, 1);
    float dx = ppos.x - LESTER_FACTORY_X;
    float dy = ppos.y - LESTER_FACTORY_Y;
    float dz = ppos.z - LESTER_FACTORY_Z;
    float dist = SQRT(dx*dx + dy*dy + dz*dz);
    return (dist < 2.5f);
}

void main() {
    SET_MISSION_FLAG(1);
    NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

    ShowLesterMsg("Tudo preparado para continuarmos os preparativos.", "Lester");
    WAIT(4000);
    ShowLesterMsg("Vá até a fábrica para continuar os planejamentos.", "Lester");
    WAIT(2000);

    while (1) {
        // Desenha marker tipo 0 (cone invertido) no local enquanto não continuou
        if (!missionContinued) {
            vector3 markerPos = {LESTER_FACTORY_X, LESTER_FACTORY_Y, LESTER_FACTORY_Z - 1.0f};
            vector3 zero = {0.0f, 0.0f, 0.0f};
            vector3 scale = {2.0f, 2.0f, 1.0f};
            RGBA color = {0, 255, 0, 120};
            DRAW_MARKER(0, markerPos, zero, zero, scale, color, 0, 0, 2, 0, 0, 0, 0);
        }

        if (!missionContinued && IsPlayerNearFactory()) {
            FloatingHelpText("Aperte B para continuar o planejamento");
            if (IS_CONTROL_JUST_PRESSED(0, Button_B)) {
                FloatingHelpText("");
                missionContinued = true;
                WAIT(2000);
                REQUEST_SCRIPT("lester_mission_heist_02");
                while (!HAS_SCRIPT_LOADED("lester_mission_heist_02")) WAIT(0);
                START_NEW_SCRIPT("lester_mission_heist_02", 1024);
                SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_02");
                TERMINATE_THIS_THREAD();
            }
        } else if (!missionContinued) {
            FloatingHelpText("");
        }
        WAIT(20);
    }
}
