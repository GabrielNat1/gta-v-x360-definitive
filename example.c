#include "types.h"
#include "natives.h"
#include "common.h"
#include "GTAV/consts32.h"
#include "GTAV/natives32.h"

// --- Enums de botões ---
enum ScaleformButtons
{
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_RIGHT,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_BLANK,
    BUTTON_DPAD_ALL,
    BUTTON_DPAD_UP_DOWN,
    BUTTON_DPAD_LEFT_RIGHT,
    BUTTON_LSTICK_UP,
    BUTTON_LSTICK_DOWN,
    BUTTON_LSTICK_LEFT,
    BUTTON_LSTICK_RIGHT,
    BUTTON_LSTICK,
    BUTTON_LSTICK_ALL,
    BUTTON_LSTICK_UP_DOWN,
    BUTTON_LSTICK_LEFT_RIGHT,
    BUTTON_LSTICK_ROTATE,
    BUTTON_RSTICK_UP,
    BUTTON_RSTICK_DOWN,
    BUTTON_RSTICK_LEFT,
    BUTTON_RSTICK_RIGHT,
    BUTTON_RSTICK,
    BUTTON_RSTICK_ALL,
    BUTTON_RSTICK_UP_DOWN,
    BUTTON_RSTICK_LEFT_RIGHT,
    BUTTON_RSTICK_ROTATE,
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_LB,
    BUTTON_LT,
    BUTTON_RB,
    BUTTON_RT,
    BUTTON_START,
    BUTTON_BACK,
    RED_BOX,
    RED_BOX_1,
    RED_BOX_2,
    RED_BOX_3,
    LOADING_HALF_CIRCLE_LEFT,
    ARROW_UP_DOWN,
    ARROW_LEFT_RIGHT,
    ARROW_ALL,
    LOADING_HALF_CIRCLE_LEFT_2,
    SAVE_HALF_CIRCLE_LEFT,
    LOADING_HALF_CIRCLE_RIGHT,
};

enum Buttons
{
    Button_A = 0xC1,
    Button_B = 0xC3,
    Button_X = 0xC2,
    Button_Y = 0xC0,
    Button_Back = 0xBF,
    Button_LB = 0xC4,
    Button_LT = 0xC6,
    Button_LS = 0xC8,
    Button_RB = 0xC5,
    Button_RT = 0xC7,
    Button_RS = 0xC9,
    Dpad_Up = 0xCA,
    Dpad_Down = 0xCB,
    Dpad_Left = 0xCC,
    Dpad_Right = 0xCD
};

#define DEALERSHIP_X -47.0f
#define DEALERSHIP_Y -1116.0f
#define DEALERSHIP_Z 26.4f

// Defina conjuntos de carros (apenas veículos do jogo base/Xbox 360)
#define CAR_SETS 3
Hash carSets[CAR_SETS][4] = {
    { joaat("adder"), joaat("zentorno"), joaat("comet2"), joaat("buffalo2") },
    { joaat("entityxf"), joaat("cheetah"), joaat("carbonizzare"), joaat("banshee") },
    { joaat("infernus"), joaat("vacca"), joaat("bullet"), joaat("turismor") }
};
char* carNameSets[CAR_SETS][4] = {
    { "Adder", "Zentorno", "Comet", "Buffalo S" },
    { "Entity XF", "Cheetah", "Carbonizzare", "Banshee" },
    { "Infernus", "Vacca", "Bullet", "Turismo R" }
};
char* carPriceTextSets[CAR_SETS][4] = {
    { "R$ 1.000.000", "R$ 725.000", "R$ 100.000", "R$ 96.000" },
    { "R$ 795.000", "R$ 650.000", "R$ 195.000", "R$ 105.000" },
    { "R$ 440.000", "R$ 240.000", "R$ 155.000", "R$ 500.000" }
};
int carPriceSets[CAR_SETS][4] = {
    { 1000000, 725000, 100000, 96000 },
    { 795000, 650000, 195000, 105000 },
    { 440000, 240000, 155000, 500000 }
};

// Arrays atuais (serão atualizados a cada dia)
Hash carModels[4];
char* carNames[4];
char* carPricesText[4];
int carPrices[4];

// Adicione novamente as posições dos carros
vector3 carPositions[4] = {
    {-44.6615f, -1110.1520f, 26.6023f},
    {-47.6110f, -1108.7520f, 26.0258f},
    {-50.7014f, -1107.0430f, 26.7448f},
    {-53.8528f, -1105.6813f, 26.0133f}
};


int dealershipActive = 0;
int carsBought[4] = {0,0,0,0};
int carBuyTime[4] = {0,0,0,0};

Vehicle cars[4];
Blip carBlips[4]; // Adicione array para os blips dos carros

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

// Função aprimorada para desenhar texto 3D bonito e centralizado, com fundo translúcido
void Draw3DTextStyled(vector3 pos, char* text, RGBA mainColor, RGBA outlineColor, float scale, float bgWidth, float bgHeight) {
    vector2 screen;
    if(GET_SCREEN_COORD_FROM_WORLD_COORD(pos, &screen.x, &screen.y)) {
        // Fundo translúcido
        vector2 rectPos = { screen.x, screen.y };
        Size rectSize = { bgWidth, bgHeight };
        RGBA bgColor = {0, 0, 0, 180};
        DRAW_RECT(rectPos, rectSize, bgColor);

        // Texto estilizado
        SET_TEXT_FONT(7);
        SET_TEXT_SCALE(scale, scale);
        SET_TEXT_COLOUR(mainColor);
        SET_TEXT_CENTRE(1);
        SET_TEXT_OUTLINE();
        SET_TEXT_DROPSHADOW(2, outlineColor);
        SET_TEXT_EDGE(2, outlineColor);

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

// Função utilitária para calcular escala fixa para texto 3D, independente da distância
float GetStatic3DTextScale(float baseScale, float minScale, float maxScale, float distance, float refDistance) {
    float scale = baseScale * (refDistance / distance);
    if (scale < minScale) scale = minScale;
    if (scale > maxScale) scale = maxScale;
    return scale;
}

void UpdateDealershipCarsForDay() {
    int day = GET_CLOCK_DAY_OF_MONTH();
    int set = day % CAR_SETS;
    for (int i = 0; i < 4; i++) {
        carModels[i] = carSets[set][i];
        carNames[i] = carNameSets[set][i];
        carPricesText[i] = carPriceTextSets[set][i];
        carPrices[i] = carPriceSets[set][i];
    }
}

void main() {
    int lastDay = -1;

    while(1) {
        WAIT(0);
        // Atualiza carros se mudou o dia
        int currentDay = GET_CLOCK_DAY_OF_MONTH();
        if (currentDay != lastDay) {
            UpdateDealershipCarsForDay();
            lastDay = currentDay;
        }

        Ped player = PLAYER_PED_ID();

        // Remova ou comente a linha abaixo para não forçar o player a spawnar na concessionária a cada frame
        // SET_ENTITY_COORDS(player, DEALERSHIP_X, DEALERSHIP_Y, DEALERSHIP_Z, 0, 0, 0, 0);

        vector3 playerPos = GET_ENTITY_COORDS(player, 0);
        float distToDealer = Distance(playerPos, (vector3){DEALERSHIP_X, DEALERSHIP_Y, DEALERSHIP_Z});

        if(distToDealer < spawnDistance && dealershipActive == 0) {
            for(int i=0;i<4;i++) {
                REQUEST_MODEL(carModels[i]);
                while(!HAS_MODEL_LOADED(carModels[i])) WAIT(0);
                cars[i] = CREATE_VEHICLE(carModels[i], carPositions[i], 180.0f, 1, 1);
                SET_ENTITY_INVINCIBLE(cars[i], 1);
                SET_VEHICLE_UNDRIVEABLE(cars[i], 1);
                SET_VEHICLE_DOORS_LOCKED(cars[i], 2); // 2 = locked

                // Customização dos veículos
                SET_VEHICLE_MOD(cars[i], 0, i % 3, 0); // Spoiler
                SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(cars[i], (RGB){(i * 50) % 255, (i * 80) % 255, (i * 120) % 255});
                SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(cars[i], (RGB){(i * 100) % 255, (i * 60) % 255, (i * 40) % 255});
                SET_VEHICLE_MOD(cars[i], 1, i % 2, 0); // Bumper dianteiro
                SET_VEHICLE_MOD(cars[i], 2, i % 2, 0); // Bumper traseiro
                SET_VEHICLE_MOD(cars[i], 3, i % 3, 0); // Saias laterais

                // Cria blip para cada carro, mas deixa invisível inicialmente
                carBlips[i] = ADD_BLIP_FOR_COORD(carPositions[i].x, carPositions[i].y, carPositions[i].z);
                SET_BLIP_SPRITE(carBlips[i], 225);
                SET_BLIP_DISPLAY(carBlips[i], 2); // Mostra só próximo
                SET_BLIP_SCALE(carBlips[i], 0.7f);
                SET_BLIP_COLOUR(carBlips[i], 0); // 0 = preto
                SET_BLIP_AS_SHORT_RANGE(carBlips[i], 1);
                SET_BLIP_ALPHA(carBlips[i], 0); // invisível por padrão
            }
            dealershipActive = 1;
        }

        if(distToDealer > despawnDistance && dealershipActive == 1) {
            for(int i=0;i<4;i++) {
                if(carsBought[i] == 0) {
                    if(DOES_ENTITY_EXIST(cars[i])) DELETE_ENTITY(&cars[i]);
                    if(DOES_BLIP_EXIST(carBlips[i])) REMOVE_BLIP(&carBlips[i]);
                } else {
                    // Se o carro foi comprado, mas não existe mais, libera para nova compra
                    if(!DOES_ENTITY_EXIST(cars[i])) {
                        carsBought[i] = 0;
                        if(DOES_BLIP_EXIST(carBlips[i])) REMOVE_BLIP(&carBlips[i]);
                    }
                }
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

        // Atualiza visibilidade dos blips: só o mais próximo aparece
        for(int i=0;i<4;i++) {
            if(DOES_BLIP_EXIST(carBlips[i])) {
                if(i == closestCarIndex && closestDist < 30.0f) {
                    SET_BLIP_ALPHA(carBlips[i], 255); // visível
                    SET_BLIP_COLOUR(carBlips[i], 0); // preto
                } else {
                    SET_BLIP_ALPHA(carBlips[i], 0); // invisível
                }
            }
        }

        if(closestCarIndex != -1) {
            vector3 textPos = carPositions[closestCarIndex];
            textPos.z += 1.2f;

            float distToCar = Distance(playerPos, carPositions[closestCarIndex]);
            float staticScale = GetStatic3DTextScale(0.55f, 0.38f, 0.55f, distToCar, 8.0f);

            // Exibe nome do carro acima
            Draw3DTextStyled(
                (vector3){textPos.x, textPos.y, textPos.z + 0.35f},
                carNames[closestCarIndex],
                (RGBA){255,255,255,255},
                (RGBA){0,60,180,255},
                staticScale,
                0.28f, 0.045f
            );

            // Exibe preço abaixo do nome
            Draw3DTextStyled(
                (vector3){textPos.x, textPos.y, textPos.z + 0.15f},
                carPricesText[closestCarIndex],
                (RGBA){76,175,80,255},
                (RGBA){0,100,0,255},
                staticScale * 0.85f,
                0.25f, 0.04f
            );

            // Exibe mini mensagem na tela se player estiver próximo do veículo (ajuste para até 4.5f de distância)
            if (closestDist < 4.5f) {
                SET_TEXT_FONT(7);
                SET_TEXT_SCALE(0.35f, 0.35f);
                SET_TEXT_COLOUR((RGBA){255,255,255,220});
                SET_TEXT_CENTRE(1);
                SET_TEXT_OUTLINE();
                BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
                ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Aperte Botao Direito para adquirir este veiculo");
                END_TEXT_COMMAND_DISPLAY_TEXT((vector2){0.5f, 0.82f});

                // Compra apenas quando pressionar botão direito e estiver próximo
                if(closestDist < 2.5f && IS_CONTROL_JUST_PRESSED(0, Dpad_Right)) {
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
                        DrawNotification("CHAR_LS_CUSTOMS", "Concessionaria", "Compra realizada!", "Parabens, voce comprou o veiculo!", "DEALER");
                    } else {
                        DrawNotification("CHAR_LS_CUSTOMS", "Concessionaria", "Saldo insuficiente!", "Desculpe, voce nao tem dinheiro suficiente para comprar este veiculo!", "DEALER");
                    }
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