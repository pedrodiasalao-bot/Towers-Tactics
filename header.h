#ifndef HEADER_H
#define HEADER_H

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Utils/sdl_utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define MAP_ROWS 18
#define MAP_COLS 32

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

/////////
/* MAP */
/////////

extern char map[MAP_ROWS][MAP_COLS];
void loadMap();
void renderMap();

////////////////
/* GAME LOGIC */
////////////////

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

typedef struct Camera
{
    float x;
    float y;

}Camera;

typedef struct UnitStats
{
    char class[20]; // which class it is (for sprites)
    int hp_current; // when this reaches 0, the unit 'dies'
    int hp_max;
    int def; // flat amount to reduce incoming damage by
    int atk;
    int pen; // % amount to ignore def during attacks
    int mvm; // how many tiles the unit can move in the movement turn
    int crit; // % chance for an attack to deal double damage
    int range; // attack range in tiles
    int deadzone; // specifically for the Archer class: area around the unit where attack is disabled, in tiles

    int x;
    int y;

}UnitStats;

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

    Camera *camera;
    UnitStats *unit;
    
    /* Timing */
    Uint64 lastTicksMS;

}AppState;

#endif