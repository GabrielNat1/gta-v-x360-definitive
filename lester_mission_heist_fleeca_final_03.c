#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

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
void ShowLesterMsg(const char* msg, int duration)
{
    // Inicia o comando de impressão de texto na tela (legenda em baixo)
    BEGIN_TEXT_COMMAND_PRINT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
    // Parâmetros: (Duração em ms, Ativar loop/Scroll)
    END_TEXT_COMMAND_PRINT(duration, 1);
}

// ============================================================================
// MAIN SCRIPT
// ============================================================================
bool missionFinished = false;
int tickAfterLoot = 0;

void main() {
    SET_MISSION_FLAG(1);
    NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

    while (true) {
        // Lógica de monitoramento
        if (missionFinished) {
            tickAfterLoot++;
            
            if (tickAfterLoot > 150) {
                // Carregamento do próximo script
                REQUEST_SCRIPT("lester_mission_heist_fleeca_final_03");
                while (!HAS_SCRIPT_LOADED("lester_mission_heist_fleeca_final_03")) {
                    WAIT(0);
                }
                
                START_NEW_SCRIPT("lester_mission_heist_fleeca_final_03", 1024);
                SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_fleeca_final_03");

                // Espera simulada (5 minutos de jogo)
                int tickWait = 0;
                while (tickWait < 15000) { 
                    WAIT(20); 
                    tickWait++; 
                }

                // Exibe a mensagem por 5 segundos (5000ms) e adiciona o dinheiro
                ShowLesterMsg("Lester: O dinheiro foi transferido! Fique atento para o proximo golpe.", 5000);
                AddMoneyToCurrentCharacter(250000); 
                
                WAIT(5000); // Espera a mensagem sumir
                TERMINATE_THIS_THREAD();
            }
        }

        WAIT(20);
    }
}