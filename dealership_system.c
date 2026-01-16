#include "types.h"
#include "natives.h"
#include "common.h"
#include "GTAV/consts32.h"
#include "GTAV/natives32.h"


#define DEALERSHIP_X -47.0f
#define DEALERSHIP_Y -1116.0f
#define DEALERSHIP_Z 26.4f

Hash carModels[4] = {joaat("adder"), joaat("zentorno"), joaat("comet2"), joaat("buffalo2")};
vector3 carPositions[4] = {
    {-42.1360f, -1116.2430f, 26.4351f},
    {-44.8640f, -1116.6132f, 26.0546f},
    {-47.9091f, -1116.3789f, 26.0551f},
    {-50.9464f, -1116.4383f, 26.0551f}
};
char* carNames[4] = {"Super Car Adder", "Super Car Zentorno", "Super Car Comet", "Super Car Buffalo"};
char* carPricesText[4] = {"R$ 1.200.000", "R$ 1.500.000", "R$ 1.300.000", "R$ 1.100.000"};
int carPrices[4] = {1200000, 1500000, 1300000, 1100000};

Hash npcModels[1] = {joaat("s_m_m_autoshop_01")};
vector3 npcPositions[1] = {{-46.0f, -1114.0f, 26.4f}};

int dealershipActive = 0;
int carsBought[4] = {0,0,0,0};
int carBuyTime[4] = {0,0,0,0};

Vehicle cars[4];
Ped npcs[1];
Blip dealershipBlip;

float spawnDistance = 160.0f;
float despawnDistance = 220.0f;

float Distance(vector3 a, vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return SQRT(dx*dx + dy*dy + dz*dz);
}

void Draw3DText(vector3 pos, char* text, RGBA mainColor, RGBA outlineColor, float scale) {
    vector2 screen;
    if(GET_SCREEN_COORD_FROM_WORLD_COORD(pos, &screen.x, &screen.y)) {
        SET_TEXT_FONT(7);
        SET_TEXT_SCALE(scale, scale);
        SET_TEXT_COLOUR(outlineColor);
        SET_TEXT_DROPSHADOW(2, outlineColor);
        SET_TEXT_EDGE(1, outlineColor);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
        END_TEXT_COMMAND_DISPLAY_TEXT(screen);

        SET_TEXT_FONT(7);
        SET_TEXT_SCALE(scale, scale);
        SET_TEXT_COLOUR(mainColor);
        SET_TEXT_DROPSHADOW(2, outlineColor);
        SET_TEXT_EDGE(1, outlineColor);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
        END_TEXT_COMMAND_DISPLAY_TEXT(screen);
    }
}

void DrawNotification(char* Foto, char* Remetente, char* TextoPrincipal, char* TextoFinal, char* ClanTag) {
    _SET_NOTIFICATION_TEXT_ENTRY("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(TextoFinal);
    _SET_NOTIFICATION_MESSAGE_CLAN_TAG_2(Foto, Foto, 1, 7, Remetente, TextoPrincipal, 5.0f, ClanTag, 2);
    _DRAW_NOTIFICATION(1, 1);
}

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
    dealershipBlip = ADD_BLIP_FOR_COORD(DEALERSHIP_X, DEALERSHIP_Y, DEALERSHIP_Z);
    SET_BLIP_SPRITE(dealershipBlip, 225);
    SET_BLIP_AS_SHORT_RANGE(dealershipBlip, 1);
    SET_BLIP_NAME_FROM_TEXT_FILE(dealershipBlip, "DEALERSHIP");
    
    Ped player = PLAYER_PED_ID();
    SET_ENTITY_COORDS(player, DEALERSHIP_X, DEALERSHIP_Y, DEALERSHIP_Z, 0, 0, 0, 0);

    while(1) {
        WAIT(0);
        player = PLAYER_PED_ID();
        vector3 playerPos = GET_ENTITY_COORDS(player, 0);
        float distToDealer = Distance(playerPos, (vector3){DEALERSHIP_X, DEALERSHIP_Y, DEALERSHIP_Z});

        if(distToDealer < spawnDistance && dealershipActive == 0) {
            for(int i=0;i<4;i++) {
                REQUEST_MODEL(carModels[i]);
                while(!HAS_MODEL_LOADED(carModels[i])) WAIT(0);
                cars[i] = CREATE_VEHICLE(carModels[i], carPositions[i], 0.0f, 1, 1);
                SET_ENTITY_INVINCIBLE(cars[i], 1);
                SET_VEHICLE_UNDRIVEABLE(cars[i], 1);
                // Tranca as portas ao spawnar
                SET_VEHICLE_DOORS_LOCKED(cars[i], 2); // 2 = locked
            }

            for(int i=0;i<1;i++) {
                REQUEST_MODEL(npcModels[i]);
                while(!HAS_MODEL_LOADED(npcModels[i])) WAIT(0);
                npcs[i] = CREATE_PED(26, npcModels[i], npcPositions[i], 0.0f, 1, 1);
                SET_PED_CAN_RAGDOLL(npcs[i], 0);
                SET_ENTITY_INVINCIBLE(npcs[i], 1);
            }

            dealershipActive = 1;
        }

        if(distToDealer > despawnDistance && dealershipActive == 1) {
                for(int i=0;i<4;i++) {
                    if(carsBought[i] == 0) {
                        if(DOES_ENTITY_EXIST(cars[i])) DELETE_ENTITY(&cars[i]);
                    }
                }
            for(int i=0;i<1;i++) {
                if(DOES_ENTITY_EXIST(npcs[i])) DELETE_ENTITY(&npcs[i]);
            }
            dealershipActive = 0;
        }

        int closestCarIndex = -1;
        float closestDist = 10.0f;
        
        for(int i=0;i<4;i++) {
            if(carsBought[i]==0) {
                float distToCar = Distance(playerPos, carPositions[i]);
                if(distToCar < closestDist) {
                    closestDist = distToCar;
                    closestCarIndex = i;
                }
            }
        }

        if(closestCarIndex != -1) {
            vector3 textPos = carPositions[closestCarIndex];
            textPos.z += 1.2f;
            Draw3DText((vector3){textPos.x, textPos.y, textPos.z+0.3f}, carNames[closestCarIndex], (RGBA){255,255,255,255}, (RGBA){0,0,0,255}, 0.55f);
            
            Draw3DText((vector3){textPos.x, textPos.y, textPos.z+0.10f}, carPricesText[closestCarIndex], (RGBA){0,255,80,255}, (RGBA){0,0,0,255}, 0.38f);
            Draw3DText((vector3){textPos.x, textPos.y, textPos.z-0.15f}, "Pressione X + Y para adquirir", (RGBA){100,200,255,255}, (RGBA){0,0,0,180}, 0.25f);

            if(IS_CONTROL_PRESSED(0, INPUT_FRONTEND_X) && IS_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_Y)) {
                int playerMoney = GetCurrentMoney();
                int carPrice = carPrices[closestCarIndex];
                
                if(playerMoney >= carPrice) {
                    carsBought[closestCarIndex] = 1;
                    carBuyTime[closestCarIndex] = GET_GAME_TIMER();
                    SET_VEHICLE_UNDRIVEABLE(cars[closestCarIndex], 0);
                    SET_VEHICLE_LIGHTS(cars[closestCarIndex], 0); 
                    
                    SET_VEHICLE_DOORS_LOCKED(cars[closestCarIndex], 1); // 1 = unlocked
                    PLAY_SOUND_FRONTEND(-1, "PURCHASE", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1);
                    SET_PED_INTO_VEHICLE(player, cars[closestCarIndex], -1);
                    AddMoneyToCurrentCharacter(-carPrice);
                } else {
                    DrawNotification("CHAR_LS_CUSTOMS", "Concessionaria", "Saldo insuficiente!", "Desculpe, voce nao tem dinheiro suficiente para comprar este veiculo!", "DEALER");
                }
            }
        }

        for(int i=0;i<4;i++) {
            if(carsBought[i]==1 && carBuyTime[i] > 0) {
                int timePassed = GET_GAME_TIMER() - carBuyTime[i];
                if(timePassed >= 3000) {
                    DrawNotification("CHAR_LS_CUSTOMS", "Concessionaria", "Muito obrigado!", "Obrigado por contar com nossa concessionaria! Sempre que precisar estaremos aqui. Espero que goste do seu novo carro. Tenha um bom dia!", "DEALER");
                    carBuyTime[i] = 0;
                }
            }
        }
    }
}