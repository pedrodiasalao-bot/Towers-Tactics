#ifndef UTILS_H
#define UTILS_H

#include "units.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Utils/sdl_utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

typedef struct Camera
{
    float x;
    float y;

}Camera;

typedef struct InputState
{
    /* Mouse state */
    float mouseX;
    float mouseY;
    bool mouseLeftDown;
    bool mouseLeftPressed;
    bool mouseLeftReleased;

    /* Keyboard state indexed by SDL_Scancode */
    bool keyDown[SDL_SCANCODE_COUNT];
    bool keyPressed[SDL_SCANCODE_COUNT];
    bool keyReleased[SDL_SCANCODE_COUNT];

}InputState;

typedef enum {

    STATE_MENU,
    STATE_PAUSE,
    STATE_PLAYING,
    STATE_SPAWN_UNIT,
    STATE_GAME_OVER,

}GameState;

typedef struct AppState
{
    /* SDL objects */
    SDL_Window *window;
    SDL_Renderer *renderer;

    /* Per-frame input data */
    InputState input;

    /* Unit Textures */
    SDL_Texture *spriteArcherBlue;
    SDL_Texture *spriteArcherRed;
    SDL_Texture *spriteCavBlue;
    SDL_Texture *spriteCavRed;
    SDL_Texture *spriteKnightBlue;
    SDL_Texture *spriteKnightRed;
    SDL_Texture *spriteSBKnightBlue;
    SDL_Texture *spriteSBKnightRed;
    SDL_Texture *uiSettings;
    SDL_Texture *tileGround;
    SDL_Texture *tileWall;
    SDL_Texture *tileRiver;
    SDL_Texture *tileBush;
    SDL_Texture *tileCapture;
    SDL_Texture *tileTower;
    SDL_Texture *tileHigh;
    SDL_Texture *tileGate;
    SDL_Texture *blueSelect;
    SDL_Texture *redSelect;
    SDL_Texture *greenSelect;
    SDL_Texture *tileCaptureRed;
    SDL_Texture *tileCaptureBlue;

    /* Camera */
    Camera *camera;

    /* Units */
    UnitStats units[50];
    int unitCount; // Useful for distinguishing units
    int selectedIndex; // So it's known what unit is currently selected (Useful for the moving)

    /* Game Logic */
    GameState currentState; // Game States
    int currentTurn; // 0 - Blue Player /// 1 -  Red Player
    int turnCounter;


    bool isASingleUnitStandingInPoint;
    int capPoint1Progress;
    int capPoint2Progress;
    int capPoint3Progress;
    int capPoint4Progress;
    int capPoint5Progress;
    int capPoint6Progress;
    int capPoint7Progress;
    int capPoint8Progress;
    
    /* Timing */
    Uint64 lastTicksMS;

    /* Font */

    TTF_Font *font;
    SDL_Texture *turnTextTexture;

}AppState;

// Turn Based System

void endTurn(AppState *app);
bool allUnitsMoved (AppState *app);
void updateTextTexture (AppState *app);
void renderUI(AppState *app);
void attackUnit(AppState *app, int attacker, int defender);

#endif