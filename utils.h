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
    UnitStats *units;
    
    /* Timing */
    Uint64 lastTicksMS;

}AppState;

#endif