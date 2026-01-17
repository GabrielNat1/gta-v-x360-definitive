#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

// =============================================================
// CONFIG
// =============================================================
#define INTERACT_DIST 2.0f

// =============================================================
// BLIP — FLEECA HEIST (BANHAM)
// =============================================================
#define FLEECA_BLIP_X -2964.6704f
#define FLEECA_BLIP_Y  482.9354f
#define FLEECA_BLIP_Z   15.7068f

// =============================================================
// POSIÇÃO REAL (DA SUA FOTO) + CORREÇÃO DE PIVOT
// menu Z = 15.8365
// ajuste real do modelo = -0.06
// =============================================================
// Ponto de interação atualizado conforme solicitado
static const vector3 VAULT_POS =
{
    -2958.536f,
     482.2697f,
     (15.8365f - 0.06f)   // 15.7765f
};

// Posição de interação do player (onde deve estar para abrir)
static const vector3 INTERACT_POS =
{
    -2956.6f,
     481.7f,
     15.6f
};

// rotações exatas
#define VAULT_YAW_CLOSED  -2.4f
#define VAULT_YAW_OPEN   -108.1f

// =============================================================
// MODELO CORRETO
// hei_prop_heist_sec_door
// =============================================================
#define VAULT_MODEL -63539571

// =============================================================
// SOM DO COFRE (FLEECA HEIST)
// =============================================================
#define VAULT_OPEN_SOUND "VAULT_DOOR_OPEN"
#define VAULT_SOUNDSET   "FLEECA_SOUNDSET"

// =============================================================
// GLOBALS
// =============================================================
Object gVaultDoor = 0;
bool gVaultOpened = false;
int gFleecaBlip = 0;

// =============================================================
// UI
// =============================================================
void DrawHelp(const char* txt)
{
    BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(txt);
    END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

// =============================================================
// BLIP DO BANCO
// =============================================================
void CreateFleecaBlip()
{
    gFleecaBlip = ADD_BLIP_FOR_COORD(
        FLEECA_BLIP_X,
        FLEECA_BLIP_Y,
        FLEECA_BLIP_Z
    );

    SET_BLIP_SPRITE(gFleecaBlip, 272);
    SET_BLIP_COLOUR(gFleecaBlip, 2);
    SET_BLIP_SCALE(gFleecaBlip, 1.0f);
    SET_BLIP_AS_SHORT_RANGE(gFleecaBlip, false);

    BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Fleeca Bank (Heist)");
    END_TEXT_COMMAND_SET_BLIP_NAME(gFleecaBlip);
}

// =============================================================
// DELETAR A PORTA NATIVA (ANTES DE TUDO)
// =============================================================
void DeleteNativeVaultDoor()
{
    Object obj;
    Object obj2;
    int tries = 0;

    // Tente deletar por até 10 tentativas para garantir remoção
    while (tries < 10)
    {
        obj = GET_CLOSEST_OBJECT_OF_TYPE(
            VAULT_POS,
            5.0f, // raio maior para garantir busca
            VAULT_MODEL,
            false,
            false,
            false
        );

        if (DOES_ENTITY_EXIST(obj))
        {
            SET_ENTITY_AS_MISSION_ENTITY(obj, true, true);
            DELETE_OBJECT(&obj);
            WAIT(0);
        }

        obj2 = GET_CLOSEST_OBJECT_OF_TYPE(
            VAULT_POS,
            5.0f,
            2121050683,
            false,
            false,
            false
        );

        if (DOES_ENTITY_EXIST(obj2))
        {
            SET_ENTITY_AS_MISSION_ENTITY(obj2, true, true);
            DELETE_OBJECT(&obj2);
            WAIT(0);
        }

        // Se nenhum dos dois existe, pode sair
        if (!DOES_ENTITY_EXIST(obj) && !DOES_ENTITY_EXIST(obj2))
            break;

        tries++;
    }
}

// =============================================================
// SPAWN DA PORTA (FECHADA)
// =============================================================
void SpawnManagedVaultDoor()
{
    REQUEST_MODEL(VAULT_MODEL);
    while (!HAS_MODEL_LOADED(VAULT_MODEL))
        WAIT(0);

    gVaultDoor = CREATE_OBJECT(
        VAULT_MODEL,
        VAULT_POS.x,
        VAULT_POS.y,
        VAULT_POS.z,
        true,
        true,
        false
    );

    // >>> ESSENCIAL (pivot bug fix)
    SET_ENTITY_COORDS_NO_OFFSET(
        gVaultDoor,
        VAULT_POS,
        true,
        true,
        true
    );

    SET_ENTITY_ROTATION(
        gVaultDoor,
        0.0f,
        0.0f,
        VAULT_YAW_CLOSED,
        2,
        true
    );

    FREEZE_ENTITY_POSITION(gVaultDoor, true);
    SET_ENTITY_COLLISION(gVaultDoor, true, true);
}

// =============================================================
// ABRIR COFRE + SOM + ANIMAÇÃO
// =============================================================
void OpenVaultDoor()
{
    if (gVaultOpened || !DOES_ENTITY_EXIST(gVaultDoor))
        return;

    // Move o player para a posição de interação
    SET_ENTITY_COORDS(PLAYER_PED_ID(), INTERACT_POS.x, INTERACT_POS.y, INTERACT_POS.z, true, false, false, true);

    // Heading fixo para olhar para a porta (ajuste manual, exemplo: 210.0f)
    SET_ENTITY_HEADING(PLAYER_PED_ID(), 210.0f);

    // Congela o player durante a animação e abertura da porta
    FREEZE_ENTITY_POSITION(PLAYER_PED_ID(), true);

    // Animação do jogador: hackear/mexer no painel (sequência única)
    REQUEST_ANIM_DICT("anim@heists@keypad@");
    while (!HAS_ANIM_DICT_LOADED("anim@heists@keypad@"))
        WAIT(0);

    TASK_PLAY_ANIM(
        PLAYER_PED_ID(),
        "anim@heists@keypad@",
        "enter",
        8.0f,
        -8.0f,
        2000,
        1,
        0,
        false,
        false,
        false
    );
    WAIT(2000);

    TASK_PLAY_ANIM(
        PLAYER_PED_ID(),
        "anim@heists@keypad@",
        "idle_a",
        8.0f,
        -8.0f,
        2000,
        1,
        0,
        false,
        false,
        false
    );
    WAIT(2000);

    TASK_PLAY_ANIM(
        PLAYER_PED_ID(),
        "anim@heists@keypad@",
        "exit",
        8.0f,
        -8.0f,
        1500,
        1,
        0,
        false,
        false,
        false
    );
    WAIT(1500);

    FREEZE_ENTITY_POSITION(gVaultDoor, false);

    // som real do Fleeca
    PLAY_SOUND_FROM_ENTITY(
        -1,
        VAULT_OPEN_SOUND,
        gVaultDoor,
        VAULT_SOUNDSET,
        false,
        (any)0
    );

    // Animação suave da porta (rotação)
    float currentYaw = VAULT_YAW_CLOSED;
    float targetYaw = VAULT_YAW_OPEN;
    float step = (targetYaw - currentYaw) / 90.0f; // 90 frames para abrir

    for (int i = 0; i <= 90; ++i)
    {
        float yaw = currentYaw + (step * i);
        SET_ENTITY_ROTATION(
            gVaultDoor,
            0.0f,
            0.0f,
            yaw,
            2,
            true
        );
        WAIT(10);
    }

    SET_ENTITY_COLLISION(gVaultDoor, false, false);

    // Libera o player após a porta abrir
    FREEZE_ENTITY_POSITION(PLAYER_PED_ID(), false);

    gVaultOpened = true;
}

// =============================================================
// INTERAÇÃO
// =============================================================
void UpdateInteraction()
{
    if (gVaultOpened)
        return;

    vector3 p = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);

    // O player só pode interagir se estiver exatamente na posição de interação
    if (VDIST(p, INTERACT_POS) < 0.5f)
    {
        DrawHelp("~INPUT_CONTEXT~ Abrir cofre");

        if (IS_CONTROL_JUST_PRESSED(0, 51))
        {
            OpenVaultDoor();
        }
    }
}

// =============================================================
// MAIN
// =============================================================
void main()
{
    WAIT(1000);

    CreateFleecaBlip();

    // passo crítico
    DeleteNativeVaultDoor();
    WAIT(500);

    // Move o jogador para mais longe do cofre para evitar crash
    vector3 spawnPos = INTERACT_POS;
    spawnPos.x += 5.0f; // mais distante lateralmente
    spawnPos.z += 2.0f; // mais alto para garantir não ficar preso
    SET_ENTITY_COORDS(PLAYER_PED_ID(), spawnPos.x, spawnPos.y, spawnPos.z, true, false, false, true);

    SpawnManagedVaultDoor();

    while (true)
    {
        WAIT(0);
        UpdateInteraction();
    }
}
