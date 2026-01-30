#include "types.h"
#include "constants.h"
#include "intrinsics.h"
#include "natives.h"
#include "common.h"

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

// --- Deluxe Cars Data ---
typedef struct {
    char* name;
    char* category;
    char* exclusivity;
    char* description;
    int price;
    int hash;
    int speed; // 1-5
    int accel; // 1-5
    int brake; // 1-5
    int traction; // 1-5
    int colorPrimary;
    int colorSecondary;
    int modKit;
    int mods[10]; // Exemplo: [spoiler, bumper, ...] -1 = não aplica
} DeluxeVehicle;

// Lista do dia (exemplo com 8 carros, todos tunados e coloridos)
DeluxeVehicle deluxeVehicles[8] = {
    // Pegassi Tezeract - Roxo cromado, full mod
    {"Pegassi Tezeract", "Super", "Deluxe", "Um hipercarro elétrico feito para quem não aceita limites. Exclusivo, silencioso e brutal.", 2800000, 0, 5, 4, 3, 4, 142, 145, 0, {3,2,1,1,1,1,1,1,1,1}},
    // Grotti Itali RSX - Vermelho fosco, full mod
    {"Grotti Itali RSX", "Super", "Deluxe", "Estilo italiano, desempenho feroz. Só hoje.", 2600000, 0, 5, 5, 3, 4, 27, 0, 0, {2,2,2,2,2,2,2,2,2,2}},
    // Overflod Zeno - Azul ultra, rodas esportivas
    {"Overflod Zeno", "Super", "Deluxe", "Tecnologia sueca, potência máxima. Raro e desejado.", 2450000, 0, 5, 4, 4, 5, 70, 111, 0, {1,1,1,1,1,1,1,1,1,1}},
    // Pfister Astron Custom - Branco gelo, detalhes dourados
    {"Pfister Astron Custom", "Esportivo", "Deluxe", "SUV esportivo de luxo, para quem exige tudo.", 1980000, 0, 4, 4, 4, 4, 111, 106, 0, {2,3,1,1,1,1,1,1,1,1}},
    // Annis ZR350 Custom - Verde corrida, bodykit
    {"Annis ZR350 Custom", "Esportivo", "Deluxe", "Clássico japonês, preparado para as ruas.", 1750000, 0, 4, 4, 4, 4, 53, 0, 0, {3,2,2,2,2,2,2,2,2,2}},
    // Progen Emerus - Laranja, rodas cromadas
    {"Progen Emerus", "Super", "Deluxe", "Performance britânica, luxo absoluto.", 2850000, 0, 5, 5, 4, 5, 38, 120, 0, {2,2,2,2,2,2,2,2,2,2}},
    // Obey 10F Widebody - Amarelo corrida, widebody
    {"Obey 10F Widebody", "Esportivo", "Deluxe", "Estilo e potência alemã, edição limitada.", 2100000, 0, 4, 5, 4, 4, 88, 0, 0, {3,3,3,3,3,3,3,3,3,3}},
    // Enus Jubilee - Preto metálico, detalhes cromados
    {"Enus Jubilee", "SUV", "Deluxe", "Luxo executivo, conforto e presença.", 1900000, 0, 4, 4, 4, 4, 0, 120, 0, {2,2,2,2,2,2,2,2,2,2}}
};

// --- Deluxe Catalog (expandido) ---
char* deluxeCatalog[] = {
    "Pegassi Tezeract", "Overflod Zeno", "Grotti Itali RSX", "Progen Emerus", "Dewbauchee Vagner",
    "Pfister Astron Custom", "Obey 10F Widebody", "Annis ZR350 Custom",
    "Enus Jubilee", "Lampadati Novak", "Benefactor SM722", "Bravado Buffalo STX",
    "Karin S95", "Grotti Brioso R/A", "Vapid Dominator ASP", "Dinka RT3000",
    "Pfister Comet S2", "Emperor Vectre", "Annis Euros", "Ubermacht Cypher"
};

// --- State ---
bool deluxeMenuOpen = false;
int deluxeMenuState = 0;
int deluxeMenuIndex = 0;
int deluxeMenuPage = 0;
bool boughtToday = false;
int playerMoney = 9999999;
int errorType = 0;
int errorTimer = 0;

// --- Sons ---
void PlayNavSound() { PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1); }
void PlaySelectSound() { PLAY_SOUND_FRONTEND(-1, "SELECT", "HUD_LIQUOR_STORE_SOUNDSET", 1); }
void PlayErrorSound() { PLAY_SOUND_FRONTEND(-1, "ERROR", "HUD_FRONTEND_DEFAULT_SOUNDSET", 1); }

// --- Utilidades ---
float GetDistance(float x1, float y1, float z1, float x2, float y2, float z2) {
    float dx = x1 - x2, dy = y1 - y2, dz = z1 - z2;
    return SQRT(dx*dx + dy*dy + dz*dz);
}

// --- Função utilitária para help text ---
void FloatingHelpText(char* text) {
    BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
    END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
}

// --- Função utilitária para data (nativa, sem includes externos) ---
void GET_CURRENT_DATE_STRING(char* buffer, int size) {
    int day = GET_CLOCK_DAY_OF_MONTH();
    int month = GET_CLOCK_MONTH() + 1; // GTA V months: 0-11
    int year = GET_CLOCK_YEAR();
    // Formato: dd/mm/yyyy
    // Não use snprintf, apenas concatene manualmente (compatível com SC-CL)
    buffer[0] = (day/10) + '0';
    buffer[1] = (day%10) + '0';
    buffer[2] = '/';
    buffer[3] = (month/10) + '0';
    buffer[4] = (month%10) + '0';
    buffer[5] = '/';
    // Ano (ex: 2026)
    buffer[6] = ((year/1000)%10) + '0';
    buffer[7] = ((year/100)%10) + '0';
    buffer[8] = ((year/10)%10) + '0';
    buffer[9] = (year%10) + '0';
    buffer[10] = 0;
}

// --- Utilidades para UI ---
// Adiciona declaração de DrawRect para evitar warning/erro de função implícita
void DrawRect(float x, float y, float w, float h, int r, int g, int b, int a);

// Adiciona declarações para funções de texto customizadas usadas no menu
void SetTextScale(float p0, float size);
void SetTextColour(int r, int g, int b, int a);
void SetTextDropshadow(int distance, int r, int g, int b, int a);
void EndTextCommandDisplayText(float x, float y);

// Adiciona declarações para funções de UI customizadas usadas no menu
void DrawShadowRect(float x, float y, float w, float h, int r, int g, int b, int a, float offset);
void DrawRoundedBorder(float x, float y, float w, float h, int thickness, int r, int g, int b, int a);
void DrawVerticalGradient(float x, float y, float w, float h, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2);

// --- Menu Principal ---
void DrawMainMenu() {
    float menuX = 0.01f, menuY = 0.05f, menuW = 0.16f, menuH = 0.19f;
    
    // Sombra exterior
    DRAW_RECT((vector2){menuX + menuW/2 + 0.003f, menuY + menuH/2 + 0.003f}, (Size){menuW + 0.002f, menuH + 0.002f}, (RGBA){0, 0, 0, 180});
    
    // Fundo principal
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH/2}, (Size){menuW, menuH}, (RGBA){15, 15, 15, 255});
    
    // Borda externa dourada
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH - 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW - 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    
    // Cabeçalho com gradiente
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.018f}, (Size){menuW, 0.032f}, (RGBA){40, 40, 40, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.012f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.025f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 200});
    
    SET_TEXT_FONT(0);
    SetTextScale(0.0f, 0.36f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(1);
    SetTextDropshadow(2, 0, 0, 0, 200);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("DELUXE");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + menuW/2, menuY + 0.008f});

    char* opts[] = {"Comprar", "Catalogo", "Creditos"};
    float y = menuY + 0.043f;
    for (int i = 0; i < 3; i++) {
        int sel = (deluxeMenuIndex == i);
        
        if (sel) {
            // Fundo seleção
            DRAW_RECT((vector2){menuX + menuW/2, y + 0.008f}, (Size){menuW - 0.004f, 0.024f}, (RGBA){255, 215, 0, 80});
            // Borda seleção
            DRAW_RECT((vector2){menuX + 0.002f, y + 0.008f}, (Size){0.002f, 0.024f}, (RGBA){255, 215, 0, 255});
        }
        
        SetTextScale(0.0f, 0.28f);
        SetTextColour(sel ? 255 : 200, sel ? 215 : 200, sel ? 0 : 150, 255);
        SET_TEXT_CENTRE(0);
        SetTextDropshadow(sel ? 2 : 1, 0, 0, 0, 180);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(opts[i]);
        END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + 0.008f, y - 0.001f});
        y += 0.03f;
    }
}

// --- Menu Comprar (Estilo Rockstar) ---
void DrawBuyMenu() {
    float menuX = 0.01f, menuY = 0.05f, menuW = 0.29f, menuH = 0.9f;
    
    // Sombra exterior
    DRAW_RECT((vector2){menuX + menuW/2 + 0.003f, menuY + menuH/2 + 0.003f}, (Size){menuW + 0.002f, menuH + 0.002f}, (RGBA){0, 0, 0, 200});
    
    // Fundo principal
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH/2}, (Size){menuW, menuH}, (RGBA){15, 15, 15, 255});
    
    // Borda externa dourada
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH - 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW - 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    
    // Cabeçalho
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.02f}, (Size){menuW, 0.036f}, (RGBA){40, 40, 40, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.012f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.03f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 200});
    
    SET_TEXT_FONT(0);
    SetTextScale(0.0f, 0.32f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(1);
    SetTextDropshadow(2, 0, 0, 0, 200);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("CARROS");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + menuW/2, menuY + 0.009f});

    // Lista de carros
    float y = menuY + 0.058f;
    for (int i = 0; i < 8; i++) {
        int sel = (deluxeMenuIndex == i);
        
        if (sel) {
            DRAW_RECT((vector2){menuX + menuW/2, y + 0.008f}, (Size){menuW - 0.004f, 0.022f}, (RGBA){255, 215, 0, 80});
            DRAW_RECT((vector2){menuX + 0.002f, y + 0.008f}, (Size){0.002f, 0.022f}, (RGBA){255, 215, 0, 255});
        }
        
        SetTextScale(0.0f, 0.23f);
        SetTextColour(sel ? 255 : 190, sel ? 215 : 190, sel ? 0 : 150, 255);
        SET_TEXT_CENTRE(0);
        SetTextDropshadow(sel ? 2 : 1, 0, 0, 0, 180);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(deluxeVehicles[i].name);
        END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + 0.008f, y - 0.001f});
        y += 0.024f;
    }

    // Painel direito com informações
    float panelX = 0.31f, panelY = 0.05f, panelW = 0.36f, panelH = 0.9f;
    
    // Sombra exterior painel
    DRAW_RECT((vector2){panelX + panelW/2 + 0.003f, panelY + panelH/2 + 0.003f}, (Size){panelW + 0.002f, panelH + 0.002f}, (RGBA){0, 0, 0, 200});
    
    // Fundo painel
    DRAW_RECT((vector2){panelX + panelW/2, panelY + panelH/2}, (Size){panelW, panelH}, (RGBA){15, 15, 15, 255});
    
    // Borda externa painel
    DRAW_RECT((vector2){panelX + panelW/2, panelY + 0.001f}, (Size){panelW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){panelX + panelW/2, panelY + panelH - 0.001f}, (Size){panelW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){panelX + 0.001f, panelY + panelH/2}, (Size){0.001f, panelH}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){panelX + panelW - 0.001f, panelY + panelH/2}, (Size){0.001f, panelH}, (RGBA){255, 215, 0, 255});

    DeluxeVehicle* v = &deluxeVehicles[deluxeMenuIndex];

    // Cabeçalho do painel
    DRAW_RECT((vector2){panelX + panelW/2, panelY + 0.02f}, (Size){panelW, 0.036f}, (RGBA){40, 40, 40, 255});
    DRAW_RECT((vector2){panelX + panelW/2, panelY + 0.012f}, (Size){panelW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){panelX + panelW/2, panelY + 0.03f}, (Size){panelW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 200});
    
    SetTextScale(0.0f, 0.28f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(1);
    SetTextDropshadow(2, 0, 0, 0, 200);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("DETALHES");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + panelW/2, panelY + 0.009f});

    float infoY = panelY + 0.065f;

    // Nome do carro
    SetTextScale(0.0f, 0.26f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(0);
    SetTextDropshadow(2, 0, 0, 0, 180);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(v->name);
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});
    
    infoY += 0.032f;
    
    // Linha divisória
    DRAW_RECT((vector2){panelX + panelW/2, infoY}, (Size){panelW - 0.004f, 0.001f}, (RGBA){255, 215, 0, 150});
    infoY += 0.012f;

    // Informações
    SetTextScale(0.0f, 0.19f);
    SetTextColour(180, 180, 180, 255);
    
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Categoria: ");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(v->category);
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});
    infoY += 0.024f;

    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Exclusivo: ");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(v->exclusivity);
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});
    infoY += 0.042f;

    // Barras de desempenho
    char* perf[] = {"Velocidade", "Aceleracao", "Frenagem", "Tracao"};
    int bars[] = {v->speed, v->accel, v->brake, v->traction};
    for (int i = 0; i < 4; i++) {
        SetTextScale(0.0f, 0.17f);
        SetTextColour(200, 200, 200, 255);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(perf[i]);
        END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});
        
        for (int j = 0; j < 5; j++) {
            int fill = (j < bars[i]);
            DRAW_RECT((vector2){panelX + 0.18f + 0.018f * j, infoY + 0.008f}, (Size){0.012f, 0.012f}, 
                     (RGBA){fill ? 255 : 50, fill ? 215 : 50, fill ? 0 : 50, 220});
        }
        infoY += 0.027f;
    }

    infoY += 0.015f;

    // Divisor
    DRAW_RECT((vector2){panelX + panelW/2, infoY}, (Size){panelW - 0.004f, 0.001f}, (RGBA){255, 215, 0, 150});
    infoY += 0.012f;

    // Preço
    SetTextScale(0.0f, 0.24f);
    SetTextColour(255, 215, 0, 255);
    SetTextDropshadow(2, 0, 0, 0, 180);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("$ ");
    ADD_TEXT_COMPONENT_INTEGER(v->price);
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});

    infoY += 0.045f;

    // Descrição
    SetTextScale(0.0f, 0.17f);
    SetTextColour(160, 160, 160, 255);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(v->description);
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, infoY});

    // Footer com instruções
    float footerY = panelY + panelH - 0.038f;
    DRAW_RECT((vector2){panelX + panelW/2, footerY}, (Size){panelW - 0.004f, 0.001f}, (RGBA){255, 215, 0, 150});
    footerY += 0.008f;
    
    SetTextScale(0.0f, 0.16f);
    SetTextColour(255, 215, 0, 200);
    SET_TEXT_CENTRE(0);
    SetTextDropshadow(1, 0, 0, 0, 160);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("~y~A~s~ Comprar | ~y~B~s~ Voltar");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){panelX + 0.008f, footerY});
}

// --- Menu Catálogo ---
void DrawCatalogMenu() {
    float menuX = 0.01f, menuY = 0.05f, menuW = 0.26f, menuH = 0.9f;
    
    // Sombra exterior
    DRAW_RECT((vector2){menuX + menuW/2 + 0.003f, menuY + menuH/2 + 0.003f}, (Size){menuW + 0.002f, menuH + 0.002f}, (RGBA){0, 0, 0, 200});
    
    // Fundo principal
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH/2}, (Size){menuW, menuH}, (RGBA){15, 15, 15, 255});
    
    // Borda externa dourada
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + menuH - 0.001f}, (Size){menuW, 0.001f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW - 0.001f, menuY + menuH/2}, (Size){0.001f, menuH}, (RGBA){255, 215, 0, 255});
    
    // Cabeçalho
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.02f}, (Size){menuW, 0.036f}, (RGBA){40, 40, 40, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.012f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 255});
    DRAW_RECT((vector2){menuX + menuW/2, menuY + 0.03f}, (Size){menuW - 0.002f, 0.002f}, (RGBA){255, 215, 0, 200});

    SET_TEXT_FONT(0);
    SetTextScale(0.0f, 0.32f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(1);
    SetTextDropshadow(2, 0, 0, 0, 200);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("CATALOGO");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + menuW/2, menuY + 0.009f});

    float y = menuY + 0.065f;
    int catalogSize = sizeof(deluxeCatalog) / sizeof(char*);
    for (int i = 0; i < catalogSize; i++) {
        SetTextScale(0.0f, 0.20f);
        SetTextColour(190, 190, 190, 255);
        SET_TEXT_CENTRE(0);
        SetTextDropshadow(1, 0, 0, 0, 160);
        BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("• ");
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(deluxeCatalog[i]);
        END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + 0.008f, y});
        y += 0.024f;
        
        if (y > menuY + menuH - 0.05f) break;
    }

    // Footer
    float footerY = menuY + menuH - 0.038f;
    DRAW_RECT((vector2){menuX + menuW/2, footerY}, (Size){menuW - 0.004f, 0.001f}, (RGBA){255, 215, 0, 150});
    footerY += 0.008f;
    
    SetTextScale(0.0f, 0.16f);
    SetTextColour(255, 215, 0, 200);
    SET_TEXT_CENTRE(0);
    SetTextDropshadow(1, 0, 0, 0, 160);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("~y~B~s~ Voltar");
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){menuX + 0.008f, footerY});
}

// --- Mensagens de erro/sucesso ---
void DrawDeluxeError() {
    DrawRect(0.5f, 0.5f, 0.32f, 0.12f, 200, 0, 0, 220);
    SetTextScale(0.0f, 0.36f);
    SetTextColour(255, 255, 255, 255);
    SET_TEXT_CENTRE(1);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    if (errorType == 1)
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("~r~Limite Diário Atingido~s~");
    else
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("~r~Saldo Insuficiente~s~");
    EndTextCommandDisplayText(0.5f, 0.48f);

    SetTextScale(0.0f, 0.28f);
    SET_TEXT_CENTRE(1);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    if (errorType == 1)
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Você já adquiriu um veículo Deluxe hoje.\nVolte amanhã para novas ofertas.");
    else
        ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Este veículo exige mais recursos financeiros.");
    EndTextCommandDisplayText(0.5f, 0.52f);
}

void DrawDeluxeBought() {
    DrawRect(0.5f, 0.5f, 0.32f, 0.12f, 0, 0, 0, 220);
    SetTextScale(0.0f, 0.36f);
    SetTextColour(255, 215, 0, 255);
    SET_TEXT_CENTRE(1);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("~y~Compra realizada com sucesso!~s~");
    EndTextCommandDisplayText(0.5f, 0.48f);
    SetTextScale(0.0f, 0.28f);
    SET_TEXT_CENTRE(1);
    BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME("Veículo enviado para sua garagem.");
    EndTextCommandDisplayText(0.5f, 0.52f);
}

// --- Substitua SpawnDeluxeVehicle para usar a assinatura correta de CREATE_VEHICLE ---
int SpawnDeluxeVehicle(int hash) {
    vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), 1);
    float heading = GET_ENTITY_HEADING(PLAYER_PED_ID());
    // CREATE_VEHICLE(Hash modelHash, vector3 vec, float heading, bool isNetwork, bool p6)
    int veh = CREATE_VEHICLE(hash, playerPos, heading, 1, 1);
    SET_PED_INTO_VEHICLE(PLAYER_PED_ID(), veh, -1);
    return veh;
}

// --- Notificação de proximidade ---
void ShowDealershipPrompt() {
    FloatingHelpText("Pressione ~INPUT_FRONTEND_DOWN~ para ver os ~y~Deluxe Cars~s~ do dia.");
}

void DrawNotification(char* Foto, char* Remetente, char* TextoPrincipal, char* TextoFinal, char* ClanTag) {
    _SET_NOTIFICATION_TEXT_ENTRY("STRING");
    ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(TextoFinal);
    _SET_NOTIFICATION_MESSAGE_CLAN_TAG_2(Foto, Foto, 1, 7, Remetente, TextoPrincipal, 5.0f, ClanTag, 2);
    _DRAW_NOTIFICATION(1, 1);
}

// --- Créditos via notificação estilo celular ---
void ShowCreditsNotification() {
    DrawNotification("CHAR_SOCIAL_CLUB", "HybridSlayer", "Deluxe Cars", "Mod feito por HybridSlayer com carinho.", "DE2026");
    WAIT(3000);
    DrawNotification("CHAR_SOCIAL_CLUB", "Definitive Edition", "Creditos", "Creditos ao Definitive Edition 2026", "DE2026");
}

// --- Lógica do Menu ---
void HandleDeluxeMenuInput() {
    DISABLE_CONTROL_ACTION(0, Button_B, 1);

    if (deluxeMenuState == 0) { // Menu Principal
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Up) && deluxeMenuIndex > 0) {
            deluxeMenuIndex--;
            PlayNavSound();
        }
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Down) && deluxeMenuIndex < 2) {
            deluxeMenuIndex++;
            PlayNavSound();
        }
        if (IS_CONTROL_JUST_PRESSED(0, Button_A)) {
            PlaySelectSound();
            if (deluxeMenuIndex == 0) {
                deluxeMenuState = 1;
                deluxeMenuIndex = 0;
            }
            else if (deluxeMenuIndex == 1) {
                deluxeMenuState = 2;
                deluxeMenuIndex = 0;
            }
            else if (deluxeMenuIndex == 2) {
                ShowCreditsNotification();
                deluxeMenuIndex = 0;
            }
        }
    }
    else if (deluxeMenuState == 1) { // Comprar Carro
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Up) && deluxeMenuIndex > 0) {
            deluxeMenuIndex--;
            PlayNavSound();
        }
        if (IS_CONTROL_JUST_PRESSED(0, Dpad_Down) && deluxeMenuIndex < 7) {
            deluxeMenuIndex++;
            PlayNavSound();
        }
        if (IS_CONTROL_JUST_PRESSED(0, Button_A)) {
            PlaySelectSound();
            if (boughtToday) {
                errorType = 1;
                errorTimer = 0;
            } else if (playerMoney < deluxeVehicles[deluxeMenuIndex].price) {
                errorType = 2;
                errorTimer = 0;
            } else {
                playerMoney -= deluxeVehicles[deluxeMenuIndex].price;
                boughtToday = true;
                int veh = SpawnDeluxeVehicle(deluxeVehicles[deluxeMenuIndex].hash);
                SET_VEHICLE_COLOURS(veh, deluxeVehicles[deluxeMenuIndex].colorPrimary, deluxeVehicles[deluxeMenuIndex].colorSecondary);
                SET_VEHICLE_MOD_KIT(veh, deluxeVehicles[deluxeMenuIndex].modKit);
                for (int m = 0; m < 10; m++) {
                    if (deluxeVehicles[deluxeMenuIndex].mods[m] >= 0)
                        SET_VEHICLE_MOD(veh, m, deluxeVehicles[deluxeMenuIndex].mods[m], 0);
                }
                deluxeMenuOpen = false;
                PlaySelectSound();
            }
        }
        if (IS_CONTROL_JUST_PRESSED(0, Button_B)) {
            PlayNavSound();
            deluxeMenuState = 0;
            deluxeMenuIndex = 0;
        }
    }
    else if (deluxeMenuState == 2) { // Catálogo
        if (IS_CONTROL_JUST_PRESSED(0, Button_B)) {
            PlayNavSound();
            deluxeMenuState = 0;
            deluxeMenuIndex = 0;
        }
    }
}

// --- Inicialização ---
void InitDeluxeVehicleHashes() {
    deluxeVehicles[0].hash = GET_HASH_KEY("tezeract");
    deluxeVehicles[1].hash = GET_HASH_KEY("italirsx");
    deluxeVehicles[2].hash = GET_HASH_KEY("zeno");
    deluxeVehicles[3].hash = GET_HASH_KEY("astron2");
    deluxeVehicles[4].hash = GET_HASH_KEY("zr350");
    deluxeVehicles[5].hash = GET_HASH_KEY("emerus");
    deluxeVehicles[6].hash = GET_HASH_KEY("tenf2");
    deluxeVehicles[7].hash = GET_HASH_KEY("jubilee");
}

// --- Implementações de Funções de UI ---
void DrawRect(float x, float y, float w, float h, int r, int g, int b, int a) {
    DRAW_RECT((vector2){x + w / 2, y + h / 2}, (Size){w, h}, (RGBA){r, g, b, a});
}

void SetTextScale(float p0, float size) {
    SET_TEXT_SCALE(p0, size);
}

void SetTextColour(int r, int g, int b, int a) {
    SET_TEXT_COLOUR((RGBA){r, g, b, a});
}

void SetTextDropshadow(int distance, int r, int g, int b, int a) {
    SET_TEXT_DROPSHADOW(distance, (RGBA){r, g, b, a});
}

void EndTextCommandDisplayText(float x, float y) {
    END_TEXT_COMMAND_DISPLAY_TEXT((vector2){x, y});
}

void DrawShadowRect(float x, float y, float w, float h, int r, int g, int b, int a, float offset) {
    DRAW_RECT((vector2){x + w / 2 + offset, y + h / 2 + offset}, (Size){w, h}, (RGBA){0, 0, 0, a / 2});
    DRAW_RECT((vector2){x + w / 2, y + h / 2}, (Size){w, h}, (RGBA){r, g, b, a});
}

void DrawRoundedBorder(float x, float y, float w, float h, int thickness, int r, int g, int b, int a) {
    float t = thickness * 0.001f;
    DRAW_RECT((vector2){x + w / 2, y}, (Size){w, t}, (RGBA){r, g, b, a});
    DRAW_RECT((vector2){x + w / 2, y + h}, (Size){w, t}, (RGBA){r, g, b, a});
    DRAW_RECT((vector2){x, y + h / 2}, (Size){t, h}, (RGBA){r, g, b, a});
    DRAW_RECT((vector2){x + w, y + h / 2}, (Size){t, h}, (RGBA){r, g, b, a});
}

void DrawVerticalGradient(float x, float y, float w, float h, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2) {
    int steps = 10;
    for (int i = 0; i < steps; i++) {
        float progress = (float)i / (float)steps;
        int r = r1 + (r2 - r1) * progress;
        int g = g1 + (g2 - g1) * progress;
        int b = b1 + (b2 - b1) * progress;
        int a = a1 + (a2 - a1) * progress;
        float stepY = y + (h / steps) * i;
        DRAW_RECT((vector2){x + w / 2, stepY + (h / steps) / 2}, (Size){w, h / steps}, (RGBA){r, g, b, a});
    }
}

// --- Main Loop ---
void main()
{
    NETWORK_SET_SCRIPT_IS_SAFE_FOR_NETWORK_GAME();
    InitDeluxeVehicleHashes();
    errorTimer = 0;
    
    while (true)
    {
        int player = PLAYER_PED_ID();
        vector3 p = GET_ENTITY_COORDS(player, 1);
        float dist = GetDistance(p.x, p.y, p.z, -56.9457f, -1096.4818f, 25.0f);

        if (!deluxeMenuOpen && dist < 2.0f) {
            ShowDealershipPrompt();
            if (IS_CONTROL_JUST_PRESSED(0, Dpad_Down)) {
                deluxeMenuOpen = true;
                deluxeMenuState = 0;
                deluxeMenuIndex = 0;
                PlaySelectSound();
            }
        }

        if (deluxeMenuOpen) {
            if (deluxeMenuState == 0) {
                DrawMainMenu();
            }
            else if (deluxeMenuState == 1) {
                DrawBuyMenu();
            }
            else if (deluxeMenuState == 2) {
                DrawCatalogMenu();
            }
            HandleDeluxeMenuInput();
        }

        if (errorType > 0) {
            DrawDeluxeError();
            errorTimer++;
            if (errorTimer > 150) {
                errorType = 0;
                errorTimer = 0;
            }
        }

        WAIT(0);
    }
}