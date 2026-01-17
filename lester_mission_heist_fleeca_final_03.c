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

int GetCurrentMoney()
{
    int stat = GetCurrentMoneyStat();
    int value = 0;
    STAT_GET_INT(stat, &value, -1);
    return value;
}

void main() {
	NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();

	while (true) {
		// Finaliza thread após alguns segundos e chama próximo script
		if (missionFinished) {
			tickAfterLoot++;
			if (tickAfterLoot > 150) {
				REQUEST_SCRIPT("lester_mission_heist_fleeca_final_03");
				while (!HAS_SCRIPT_LOADED("lester_mission_heist_fleeca_final_03")) WAIT(0);
				START_NEW_SCRIPT("lester_mission_heist_fleeca_final_03", 1024);
				SET_SCRIPT_AS_NO_LONGER_NEEDED("lester_mission_heist_fleeca_final_03");

				// Espera 5 minutos do game (~15000 ticks de 20ms)
				int tickWait = 0;
				while (tickWait < 15000) { WAIT(20); tickWait++; }

				// Lester agradece e transfere dinheiro
				ShowLesterMsg("Obrigado novamente! Transferi uma parte do dinheiro lavado para você. Fique preparado para mais golpes no futuro!", "Lester");
				AddMoneyToCurrentCharacter(250000); // valor exemplo, ajuste se quiser
				WAIT(4000);

				TERMINATE_THIS_THREAD();
			}
		}

		WAIT(20);
	}
}