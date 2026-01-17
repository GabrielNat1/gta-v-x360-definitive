#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// ============================================================================
// CONTROLES
// ============================================================================
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


// ============================================================
// INSTRUCTIONAL BUTTON SYSTEM (100% COMO VOCÊ ENVIOU)
// ============================================================
// (... NÃO ALTERADO ...)

#define MAX_INSTRUCTIONAL_BUTTONS 20
typedef struct {
    bool useIconId;
    int iconId;
    char* iconString;
    char* text;
} InstructionalButton;

InstructionalButton currentInstructions[MAX_INSTRUCTIONAL_BUTTONS];
int currentNumInstructions = 0;
bool shouldDisplayInstructionalButtons = FALSE;
int instructionalButtonsScaleformHandle = 0;
bool instructionalButtonsLoaded = FALSE;

void SetInstructionalButtons(InstructionalButton* arr, int count)
{
    if (count > MAX_INSTRUCTIONAL_BUTTONS)
        count = MAX_INSTRUCTIONAL_BUTTONS;

    for (int i = 0; i < count; i++)
        currentInstructions[i] = arr[i];

    currentNumInstructions = count;
}

void ShowInstructionalButtons()
{
    shouldDisplayInstructionalButtons = TRUE;
}

void HideInstructionalButtonsAndRelease()
{
    shouldDisplayInstructionalButtons = FALSE;

    if (instructionalButtonsLoaded && instructionalButtonsScaleformHandle != 0)
    {
        SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&instructionalButtonsScaleformHandle);
        instructionalButtonsScaleformHandle = 0;
        instructionalButtonsLoaded = FALSE;
    }
}

void ProcessAndDrawInstructionalButtonsPerFrame()
{
    if (!shouldDisplayInstructionalButtons)
    {
        if (instructionalButtonsScaleformHandle)
            HideInstructionalButtonsAndRelease();
        return;
    }

    if (!instructionalButtonsLoaded)
        instructionalButtonsScaleformHandle = REQUEST_SCALEFORM_MOVIE("INSTRUCTIONAL_BUTTONS");

    if (!HAS_SCALEFORM_MOVIE_LOADED(instructionalButtonsScaleformHandle))
        return;

    instructionalButtonsLoaded = TRUE;

    BEGIN_SCALEFORM_MOVIE_METHOD(instructionalButtonsScaleformHandle, "CLEAR_ALL");
    END_SCALEFORM_MOVIE_METHOD();

    for (int i = 0; i < currentNumInstructions; i++)
    {
        BEGIN_SCALEFORM_MOVIE_METHOD(instructionalButtonsScaleformHandle, "SET_DATA_SLOT");
        SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(i);

        if (currentInstructions[i].useIconId)
            SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(currentInstructions[i].iconId);
        else {
            BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
            ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(currentInstructions[i].iconString);
            END_TEXT_COMMAND_SCALEFORM_STRING();
        }

        BEGIN_TEXT_COMMAND_SCALEFORM_STRING("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(currentInstructions[i].text);
        END_TEXT_COMMAND_SCALEFORM_STRING();
        END_SCALEFORM_MOVIE_METHOD();
    }

    BEGIN_SCALEFORM_MOVIE_METHOD(instructionalButtonsScaleformHandle, "DRAW_INSTRUCTIONAL_BUTTONS");
    SCALEFORM_MOVIE_METHOD_ADD_PARAM_INT(-1);
    END_SCALEFORM_MOVIE_METHOD();

    DRAW_SCALEFORM_MOVIE_FULLSCREEN(instructionalButtonsScaleformHandle, 255, 255, 255, 255);
}


// ============================================================
// ÍCONES E BOTÕES (NÃO ALTERADO)
// ============================================================
enum InstructionalIconID {
    ICON_ARROW_UP = 0, ICON_ARROW_DOWN = 1, ICON_ARROW_LEFT = 2, ICON_ARROW_RIGHT = 3,
    ICON_DPAD_UP = 4, ICON_DPAD_DOWN = 5, ICON_DPAD_LEFT = 6, ICON_DPAD_RIGHT = 7,
    ICON_DPAD = 8, ICON_DPAD_ALL = 9, ICON_DPAD_UP_DOWN = 10, ICON_DPAD_LEFT_RIGHT = 11,
    ICON_LSTICK_UP = 12, ICON_LSTICK_DOWN = 13, ICON_LSTICK_LEFT = 14, ICON_LSTICK_RIGHT = 15,
    ICON_LSTICK = 16, ICON_LSTICK_ALL = 17, ICON_LSTICK_UP_DOWN = 18, ICON_LSTICK_LEFT_RIGHT = 19,
    ICON_LSTICK_ROTATE = 20, ICON_RSTICK_UP = 21, ICON_RSTICK_DOWN = 22, ICON_RSTICK_LEFT = 23,
    ICON_RSTICK_RIGHT = 24, ICON_RSTICK = 25, ICON_RSTICK_ALL = 26, ICON_RSTICK_UP_DOWN = 27,
    ICON_RSTICK_LEFT_RIGHT = 28, ICON_RSTICK_ROTATE = 29,
    ICON_A = 30, ICON_B = 31, ICON_X = 32, ICON_Y = 33,
    ICON_LB = 34, ICON_LT = 35, ICON_RB = 36, ICON_RT = 37,
    ICON_START = 38, ICON_SELECT = 39,
    ICON_INVALID_40 = 40, ICON_INVALID_41 = 41, ICON_INVALID_42 = 42, ICON_INVALID_43 = 43,
    ICON_CLOCKWISE_SPINNER = 44, ICON_ARROW_UP_DOWN = 45, ICON_ARROW_LEFT_RIGHT = 46,
    ICON_ARROW_LEFT_RIGHT_UP_DOWN = 47, ICON_CLOCKWISE_SPINNER_ALT = 48,
    ICON_CLOUD_SAVE_SPINNER = 49, ICON_COUNTER_CLOCKWISE_SPINNER = 50
};

static InstructionalButton DriveSimulator[] = {
    {TRUE, ICON_DPAD_RIGHT, NULL, "Seta p/ Direita"},
    {TRUE, ICON_Y, NULL, "Alerta"},
    {TRUE, ICON_DPAD_LEFT, NULL, "Seta p/ Esquerda"}
};
static int numDriveSimulatorButtons = 3;


// ============================================================================
// FINANCEIRO
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
// NOTIFICAÇÕES
// ============================================================================
void Notify(const char* msg)
{
    BEGIN_TEXT_COMMAND_PRINT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg);
    END_TEXT_COMMAND_PRINT(2500, TRUE);
}

void FloatingHelpText(const char* text)
{
    BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

// ============================================================================
// CONSTANTES / VARIÁVEIS
// ============================================================================
#define MAX_VEHICLES 128
#define FUEL_PRICE_PER_PERCENT 3

Vehicle g_vehList[MAX_VEHICLES];
float g_fuelList[MAX_VEHICLES];
float g_tempList[MAX_VEHICLES];
vector3 g_lastPos[MAX_VEHICLES];
bool g_hasLastPos[MAX_VEHICLES];

bool g_refueling = false;
Vehicle g_refuelVeh = 0;
float g_refuelStart = 0;
int g_lastRefuelTick = 0;

// SETAS
bool g_leftIndicatorOn = false;
bool g_rightIndicatorOn = false;
bool g_hazardOn = false;

// Veículo pessoal
Vehicle g_personalVehicle = 0;
int g_personalVehicleBlip = 0;

// ============================================================================
// HUD POSITIONS
// ============================================================================
#define HUD_FUEL_BG_X 0.1200f
#define HUD_FUEL_BG_Y 0.9590f
#define HUD_FUEL_BG_W 0.1390f
#define HUD_FUEL_BG_H 0.0240f

#define HUD_FUEL_X 0.1200f
#define HUD_FUEL_Y 0.9580f
#define HUD_FUEL_W 0.1330f
#define HUD_FUEL_H 0.0120f

#define HUD_TEMP_BG_X 0.0860f
#define HUD_TEMP_BG_Y 0.9820f
#define HUD_TEMP_BG_W 0.0690f
#define HUD_TEMP_BG_H 0.0130f

#define HUD_TEMP_X 0.0860f
#define HUD_TEMP_Y 0.9820f
#define HUD_TEMP_W 0.0690f
#define HUD_TEMP_H 0.0130f

// SETAS (corrigidas)
#define IND_LEFT_X    0.1350f
#define IND_LEFT_Y    0.9850f

#define IND_ALERT_X   0.1570f   // NOVO ALERTA NO MEIO

#define IND_RIGHT_X   0.1780f   // AFASTADA para caber o alerta
#define IND_RIGHT_Y   0.9850f

#define IND_W         0.0210f
#define IND_H         0.0240f

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================
void DrawRectC(float x, float y, float w, float h, int r, int g, int b, int a)
{
    DRAW_RECT(x, y, w, h, r, g, b, a);
}

bool IsBikeModel(Hash m)
{
    return (
        m == GET_HASH_KEY("bmx") ||
        m == GET_HASH_KEY("cruiser") ||
        m == GET_HASH_KEY("scorcher") ||
        m == GET_HASH_KEY("tribike") ||
        m == GET_HASH_KEY("tribike2") ||
        m == GET_HASH_KEY("tribike3")
        );
}

// ============================================================================
// POSTOS
// ============================================================================
// (Lista mantida)
vector3 g_gasStations[] =
{
    { -543.40f, -1218.00f, 18.20f },
    { 265.00f, -1262.90f, 29.20f },
    { -724.50f, -935.70f, 19.00f },
    { -304.00f, -1471.00f, 30.00f },
    { 1211.60f, -1389.40f, 35.40f },
    { 1160.50f, -322.00f, 69.20f },
    { -70.90f, -1760.80f, 29.50f },
    { -255.90f, -1267.10f, 31.30f },
    { 2005.40f, 3773.10f, 32.40f },
    { 1785.30f, 3329.60f, 40.30f },
    { 1180.00f, 2650.00f, 38.10f },
    { 2673.20f, 3266.20f, 55.20f },
    { 1688.10f, 4929.40f, 42.00f },
    { 1703.90f, 6415.40f, 32.80f },
    { 154.80f, 6627.00f, 31.80f },
    { -90.50f, 6336.00f, 31.40f },
    { -2095.00f, -320.00f, 13.00f }
};

int GAS_COUNT = 17;
Blip g_gasBlips[32];

void CreateGasBlips()
{
    for (int i = 0; i < GAS_COUNT; i++)
    {
        g_gasBlips[i] = ADD_BLIP_FOR_COORD(g_gasStations[i].x, g_gasStations[i].y, g_gasStations[i].z);
        SET_BLIP_SPRITE(g_gasBlips[i], 361);
        SET_BLIP_COLOUR(g_gasBlips[i], 28);
        SET_BLIP_AS_SHORT_RANGE(g_gasBlips[i], TRUE);
    }
}

// ============================================================================
// REGISTRAR VEÍCULO
// ============================================================================
int RegisterVehicle(Vehicle veh)
{
    for (int i = 0; i < MAX_VEHICLES; i++)
        if (g_vehList[i] == veh)
            return i;

    for (int j = 0; j < MAX_VEHICLES; j++)
        if (g_vehList[j] == 0)
        {
            g_vehList[j] = veh;
            g_fuelList[j] = 100.0f;
            g_tempList[j] = 20.0f;
            g_hasLastPos[j] = false;
            return j;
        }

    return -1;
}

// ============================================================================
// BLIP VEÍCULO PESSOAL
// ============================================================================
void UpdatePersonalVehicleBlip(Vehicle veh)
{
    if (veh == 0) return;

    if (veh != g_personalVehicle)
    {
        g_personalVehicle = veh;

        if (g_personalVehicleBlip != 0)
            REMOVE_BLIP(&g_personalVehicleBlip);

        g_personalVehicleBlip = ADD_BLIP_FOR_ENTITY(veh);

        Hash m = GET_ENTITY_MODEL(veh);

        if (IS_THIS_MODEL_A_CAR(m))   SET_BLIP_SPRITE(g_personalVehicleBlip, 225);
        else if (IS_THIS_MODEL_A_BIKE(m)) SET_BLIP_SPRITE(g_personalVehicleBlip, 348);
        else if (IS_THIS_MODEL_A_HELI(m)) SET_BLIP_SPRITE(g_personalVehicleBlip, 64);
        else if (IS_THIS_MODEL_A_PLANE(m)) SET_BLIP_SPRITE(g_personalVehicleBlip, 307);
        else SET_BLIP_SPRITE(g_personalVehicleBlip, 225);

        BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Veículo Pessoal");
        END_TEXT_COMMAND_SET_BLIP_NAME(g_personalVehicleBlip);
    }
}

// ============================================================================
// DESENHO DAS SETAS + ALERTA
// ============================================================================
void DrawLeftIndicatorSprite(bool active)
{
    if (!active) return;
    int t = GET_GAME_TIMER();
    if ((t / 350) % 2 == 0)
        DRAW_SPRITE("DriverSimulatorMod", "button_9", IND_RIGHT_X, IND_RIGHT_Y, IND_W, IND_H, 0.0f, 255, 255, 0, 180);
}

void DrawRightIndicatorSprite(bool active)
{
    if (!active) return;
    int t = GET_GAME_TIMER();
    if ((t / 350) % 2 == 0)
        DRAW_SPRITE("DriverSimulatorMod", "button_8", IND_LEFT_X, IND_LEFT_Y, IND_W, IND_H, 0.0f, 255, 255, 0, 180);
}

void DrawHazardSprite(bool active)
{
    if (!active) return;
    int t = GET_GAME_TIMER();
    if ((t / 350) % 2 == 0)
        DRAW_SPRITE("DriverSimulatorMod", "button_10", IND_ALERT_X, 0.9860f, 0.0210f, 0.0340f, 0.0f, 255, 0, 0, 255);
}

// ============================================================================
// FUEL + TEMPERATURA
// ============================================================================
void UpdateFuelSystem()
{
    Ped player = PLAYER_PED_ID();
    bool inside = IS_PED_IN_ANY_VEHICLE(player, FALSE);

    Vehicle veh = inside ? GET_VEHICLE_PED_IS_IN(player, FALSE) : 0;

    if (!inside && g_refuelVeh != 0)
        veh = g_refuelVeh;

    if (!DOES_ENTITY_EXIST(veh)) return;
    if (IsBikeModel(GET_ENTITY_MODEL(veh))) return;

    UpdatePersonalVehicleBlip(veh);

    int id = RegisterVehicle(veh);
    if (id < 0) return;

    float* fuel = &g_fuelList[id];
    float* temp = &g_tempList[id];
    vector3* lastPos = &g_lastPos[id];
    bool* hasPos = &g_hasLastPos[id];

    vector3 pos = GET_ENTITY_COORDS(veh, TRUE);
    int now = GET_GAME_TIMER();

    // Combustível
    if (inside)
    {
        if (!(*hasPos))
        {
            *lastPos = pos;
            *hasPos = true;
        }
        else
        {
            float dist = VDIST(pos.x, pos.y, pos.z, lastPos->x, lastPos->y, lastPos->z);
            *lastPos = pos;

            float consumption = dist * 0.009f;
            if (dist < 0.1f) consumption += 0.000015f;

            *fuel -= consumption;
            if (*fuel < 0) *fuel = 0;
        }
    }

    if (*fuel <= 30.0f)
    DRAW_SPRITE("DriverSimulatorMod", "button_5", 0.0650f, 0.9100, 0.0270f, 0.0390f, 0.0f, 255, 225, 225, 180); // Alerta de Combustivel

    if (*fuel <= 0.1f)
    SET_VEHICLE_ENGINE_ON(veh, FALSE, TRUE, TRUE);

    // ===============================
    // TEMPERATURA — SISTEMA CORRIGIDO
    // ===============================

    float health = GET_VEHICLE_ENGINE_HEALTH(veh);
    bool accelerating = IS_CONTROL_PRESSED(0, Button_RT);

    // Motor saudável
    if (health >= 600)
    {
        if (inside)
        {
            if (accelerating)
            {
                // aquecimento leve e realista
                *temp += 0.010f;
            }
            else
            {
                // motor ligado mas parado — esfria até 35%
                if (*temp > 35.0f)
                    *temp -= 0.015f;

                // nunca abaixo de 35 com motor ligado
                if (*temp < 35.0f)
                    *temp = 35.0f;
            }
        }
        else
        {
            // Motor desligado = esfria até 20 graus
            *temp -= 0.030f;
            if (*temp < 20.0f)
                *temp = 20.0f;
        }

        // Limite realista para motor saudável
        if (*temp > 50.0f)
            *temp = 50.0f;
    }
    else
    {

        DRAW_SPRITE("DriverSimulatorMod", "button_11", 0.0670f, 0.8260, 0.0300f, 0.0490f, 0.0f, 255, 225, 225, 180); // Alerta de Motor


        // Motor danificado (<600) superaquece
        if (inside)
        {
            if (accelerating)
                *temp += 0.050f;   // esquenta rápido
            else
                *temp += 0.020f;   // mesmo parado esquenta
        }
        else
        {
            // Motor danificado desligado esfria lentamente
            *temp -= 0.010f;
            if (*temp < 20.0f)
                *temp = 20.0f;
        }

        if (*temp > 120.0f)
            *temp = 120.0f;
    }

    if (*temp > 57)
    {
        DRAW_SPRITE("DriverSimulatorMod", "button_7", 0.0930, 0.9160, 0.0320f, 0.0580f, 0.0f, 255, 0, 0, 180); // Alerta de Óleo 
    }

    // ===============================
    // DANO COM SUPER AQUECIMENTO
    // ===============================
    if (*temp > 60)
    {
        float dmg = ((*temp - 60) / 60) * 0.40f;
        SET_VEHICLE_ENGINE_HEALTH(veh, health - dmg); 
        DRAW_SPRITE("DriverSimulatorMod", "button_6", 0.0640, 0.8690, 0.0230f, 0.0490f, 0.0f, 255, 0, 0, 180); // Alerta de Temperatura 
    }


    // Alerta temp
    static int lastBeep = 0;
    if (*temp >= 70)
    {
        if (now - lastBeep >= 900)
        {
            PLAY_SOUND_FRONTEND(-1, "Beep_Red", "DLC_HEIST_HACKING_SNAKE_SOUNDS", TRUE);
            lastBeep = now;
        }

        if ((now / 400) % 2 == 0)
        DRAW_RECT(HUD_TEMP_X, HUD_TEMP_Y, HUD_TEMP_W, HUD_TEMP_H, 255, 40, 40, 180);
    }

    // HUD combustivel + temp (igual ao seu)
    DrawRectC(HUD_FUEL_BG_X, HUD_FUEL_BG_Y, HUD_FUEL_BG_W, HUD_FUEL_BG_H, 40, 40, 40, 180);
    DrawRectC(HUD_FUEL_X, HUD_FUEL_Y, HUD_FUEL_W, HUD_FUEL_H, 40, 40, 40, 150);

    float fPct = *fuel / 100.0f;
    float fW = HUD_FUEL_W * fPct;
    int fr = 255, fg = 180, fb = 0;

    if (*fuel < 60) fg = 120;
    if (*fuel < 25) { fg = 0; fb = 0; }

    DrawRectC(HUD_FUEL_X - (HUD_FUEL_W - fW) * 0.5f, HUD_FUEL_Y, fW, HUD_FUEL_H, fr, fg, fb, 200);

    DrawRectC(HUD_TEMP_BG_X, HUD_TEMP_BG_Y, HUD_TEMP_BG_W, HUD_TEMP_BG_H, 40, 40, 40, 150);

    float tPct = *temp / 120.0f;
    float tW = HUD_TEMP_W * tPct;
    int tr = 0, tg = 255, tb = 0;

    if (*temp > 70) { tr = 255; tg = 255; tb = 0; }
    if (*temp > 95) { tr = 255; tg = 0; tb = 0; }

    DrawRectC(HUD_TEMP_X - (HUD_TEMP_W - tW) * 0.5f, HUD_TEMP_Y, tW, HUD_TEMP_H, tr, tg, tb, 200);

    // Abastecimento — mantido

    bool near = false;

    for (int i = 0; i < GAS_COUNT; i++)
    {
        float d = VDIST(pos.x, pos.y, pos.z, g_gasStations[i].x, g_gasStations[i].y, g_gasStations[i].z);

        if (d < 8.0f)
        {
            near = true;

            if (!g_refueling)
                FloatingHelpText("~INPUT_FRONTEND_RIGHT~ Abastecer");
            else
                FloatingHelpText("~INPUT_FRONTEND_RIGHT~ Finalizar");

            if (IS_CONTROL_JUST_PRESSED(0, Dpad_Right))
            {
                if (!g_refueling)
                {
                    g_refueling = true;
                    g_refuelVeh = veh;
                    g_refuelStart = *fuel;
                    g_lastRefuelTick = now;

                    PLAY_SOUND_FRONTEND(-1, "GARAGE_DOOR_OPEN", "DOOR_GARAGE_SOUNDS", TRUE);
                    Notify("~y~Abastecendo...");
                }
                else
                {
                    g_refueling = false;

                    float added = *fuel - g_refuelStart;
                    if (added < 0) added = 0;

                    int cost = (int)(added * FUEL_PRICE_PER_PERCENT);
                    AddMoneyToCurrentCharacter(-cost);

                    PLAY_SOUND_FRONTEND(-1, "GARAGE_DOOR_CLOSE", "DOOR_GARAGE_SOUNDS", TRUE);
                    Notify("~g~Abastecimento concluído.");
                }
            }
        }
    }

    if (!near && g_refueling)
    {
        g_refueling = false;

        float added = *fuel - g_refuelStart;
        if (added < 0) added = 0;

        int cost = (int)(added * FUEL_PRICE_PER_PERCENT);
        AddMoneyToCurrentCharacter(-cost);

        Notify("~g~Abastecimento Finalizado.");
    }

    if (g_refueling)
    {
        if (now - g_lastRefuelTick >= 150)
        {
            g_lastRefuelTick = now;

            if (*fuel >= 100)
            {
                g_refueling = false;

                float added = 100 - g_refuelStart;
                AddMoneyToCurrentCharacter(-(int)(added * FUEL_PRICE_PER_PERCENT));

                *fuel = 100;

                Notify("~g~Tanque cheio.");
            }
            else
            {
                *fuel += 1.0f;
                if (*fuel > 100) *fuel = 100;
            }
        }
    }

    // DESENHA SETAS + ALERTA
    DrawLeftIndicatorSprite(g_leftIndicatorOn);
    DrawRightIndicatorSprite(g_rightIndicatorOn);
    DrawHazardSprite(g_hazardOn);
}

// ============================================================================
// SETAS DO VEÍCULO
// ============================================================================
void UpdateVehicleIndicators()
{
    Ped p = PLAYER_PED_ID();
    if (!IS_PED_IN_ANY_VEHICLE(p, FALSE))
    {
        DrawLeftIndicatorSprite(false);
        DrawRightIndicatorSprite(false);
        DrawHazardSprite(false);
        return;
    }

    Vehicle veh = GET_VEHICLE_PED_IS_IN(p, FALSE);
    if (IsBikeModel(GET_ENTITY_MODEL(veh))) return;

    bool rb = IS_CONTROL_PRESSED(0, Button_RB);

    DISABLE_CONTROL_ACTION(0, INPUT_VEH_HANDBRAKE, TRUE);

    if (rb)
    {
        DISABLE_CONTROL_ACTION(0, INPUT_VEH_RADIO_WHEEL, TRUE);
        DISABLE_CONTROL_ACTION(0, INPUT_VEH_HEADLIGHT, TRUE);
        DISABLE_CONTROL_ACTION(0, INPUT_CELLPHONE_UP, TRUE);
        DISABLE_CONTROL_ACTION(0, INPUT_CHARACTER_WHEEL, TRUE);
        DISABLE_CONTROL_ACTION(0, INPUT_VEH_ATTACK, TRUE);
        DISABLE_CONTROL_ACTION(0, INPUT_VEH_EXIT, TRUE);  HideInstructionalButtonsAndRelease();
        SetInstructionalButtons(DriveSimulator, numDriveSimulatorButtons);
        ShowInstructionalButtons();
        ProcessAndDrawInstructionalButtonsPerFrame();
    }

    if (!rb)
    {
        DrawLeftIndicatorSprite(g_leftIndicatorOn);
        DrawRightIndicatorSprite(g_rightIndicatorOn);
        DrawHazardSprite(g_hazardOn);
        return;
    }

    // ALERTA
    if (IS_CONTROL_JUST_PRESSED(0, Button_Y))
    {
        g_hazardOn = !g_hazardOn;
        g_leftIndicatorOn = g_hazardOn;
        g_rightIndicatorOn = g_hazardOn;

        SET_VEHICLE_INDICATOR_LIGHTS(veh, 0, g_leftIndicatorOn);
        SET_VEHICLE_INDICATOR_LIGHTS(veh, 1, g_rightIndicatorOn);
    }

    if (!g_hazardOn)
    {
        // ESQUERDA
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Right))
        {
            g_leftIndicatorOn = !g_leftIndicatorOn;
            g_rightIndicatorOn = false;

            SET_VEHICLE_INDICATOR_LIGHTS(veh, 0, g_leftIndicatorOn);
            SET_VEHICLE_INDICATOR_LIGHTS(veh, 1, FALSE);
        }

        // DIREITA
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Left))
        {
            g_rightIndicatorOn = !g_rightIndicatorOn;
            g_leftIndicatorOn = false;

            SET_VEHICLE_INDICATOR_LIGHTS(veh, 1, g_rightIndicatorOn);
            SET_VEHICLE_INDICATOR_LIGHTS(veh, 0, FALSE);
        }
    }

    DrawLeftIndicatorSprite(g_leftIndicatorOn);
    DrawRightIndicatorSprite(g_rightIndicatorOn);
    DrawHazardSprite(g_hazardOn);
}

// ============================================================================
// MAIN
// ============================================================================
void main()
{
    REQUEST_SCRIPT_AUDIO_BANK("DLC_HEIST_HACKING_SNAKE_SOUNDS", FALSE);
    REQUEST_SCRIPT_AUDIO_BANK("DOOR_GARAGE_SOUNDS", FALSE);
    REQUEST_STREAMED_TEXTURE_DICT("DriverSimulatorMod", TRUE);

    CreateGasBlips();

    while (true)
    {
        UpdateFuelSystem();
        UpdateVehicleIndicators();
        WAIT(0);
    }
}
