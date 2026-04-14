#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Utils/sdl_utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"
#include "map.h"
#include "units.h"

static void set_nearest(SDL_Texture *t)
{
    if (t)
    SDL_SetTextureScaleMode(t, SDL_SCALEMODE_NEAREST);
}

static void Input_BeginFrame(InputState *in)
{
    /* pressed/released are one-frame flags, so clear them every frame */
    in->mouseLeftPressed = false;
    in->mouseLeftReleased = false;

    for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
    {
        in->keyPressed[i] = false;
        in->keyReleased[i] = false;
    }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    AppState *app = (AppState *)SDL_calloc(1, sizeof(AppState));
    if (!app)
    {
        SDL_Log("Out of memory.");
        return SDL_APP_FAILURE;
    }
    *appstate = app;

    /* Initialize SDL video subsystem */
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Create window + renderer once at startup */
    if (!SDL_CreateWindowAndRenderer("Week 03 - Skeleton", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &app->window, &app->renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Sprite Declaration and Definition
    
    app->spriteArcherBlue = sdl_load_texture(app->renderer, "Assets/Art/SP_ArcherBlue.png");
    app->spriteArcherRed = sdl_load_texture(app->renderer, "Assets/Art/SP_ArcherRed.png");
    app->spriteCavBlue = sdl_load_texture(app->renderer, "Assets/Art/SP_CavBlue.png");
    app->spriteCavRed = sdl_load_texture(app->renderer, "Assets/Art/SP_CavRed.png");
    app->spriteKnightBlue = sdl_load_texture(app->renderer, "Assets/Art/SP_KnightBlue.png");
    app->spriteKnightRed = sdl_load_texture(app->renderer, "Assets/Art/SP_KnightRed.png");
    app->spriteSBKnightBlue = sdl_load_texture(app->renderer, "Assets/Art/SP_SBKnightBlue.png");
    app->spriteSBKnightRed = sdl_load_texture(app->renderer, "Assets/Art/SP_SBKnightRed.png");
    app->uiSettings = sdl_load_texture(app->renderer, "Assets/Art/UI_Settings.png");
    
    app->tileGround = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeGround.png");
    app->tileWall = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeWall.png");
    app->tileBush = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeBush.png");
    app->tileRiver = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeRiver.png");
    app->tileCapture = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeCapture.png");
    app->tileHigh = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeHigh.png");
    app->tileTower = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeTower.png");
    app->tileGate = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeGate.png");

    set_nearest(app->spriteArcherBlue);
    set_nearest(app->spriteArcherRed);
    set_nearest(app->spriteCavBlue);
    set_nearest(app->spriteCavRed);
    set_nearest(app->spriteKnightBlue);
    set_nearest(app->spriteKnightRed);
    set_nearest(app->spriteSBKnightBlue);
    set_nearest(app->spriteSBKnightRed);
    set_nearest(app->uiSettings);
    set_nearest(app->tileGround);
    set_nearest(app->tileWall);
    set_nearest(app->tileBush);
    set_nearest(app->tileRiver);
    set_nearest(app->tileCapture);
    set_nearest(app->tileHigh);
    set_nearest(app->tileTower);
    set_nearest(app->tileGate);


    /* PROTOTYPE ONLY */
    loadMap("map.txt");

    app->lastTicksMS = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *app = (AppState *)appstate;
    InputState *in = &app->input;

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            {
                SDL_Scancode sc = event->key.scancode;
                bool repeat = event->key.repeat != 0;
                if (sc >= 0 && sc < SDL_SCANCODE_COUNT)
                {
                    if (!repeat && !in->keyDown[sc]) in->keyPressed[sc] = true;
                    in->keyDown[sc] = true;
                }
            }
            break;
    case SDL_EVENT_KEY_UP:
        {
            SDL_Scancode sc = event->key.scancode;
            if (sc >= 0 && sc < SDL_SCANCODE_COUNT)
            {
                in->keyDown[sc] = false;
                in->keyReleased[sc] = true;
            }
        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
        in->mouseX = event->motion.x;
        in->mouseY = event->motion.y;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (!in->mouseLeftDown) in->mouseLeftPressed = true;
            in->mouseLeftDown = true;
        }
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            in->mouseLeftDown    = false;
            in->mouseLeftReleased = true;
        }
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *app = (AppState *)appstate;

    // InputState *in = &app->input;

    // compute dt
    // Uint64 nowMS = SDL_GetTicks();
    // Uint64 elapsedMS = nowMS - app->lastTicksMS;
    // app->lastTicksMS = nowMS;
    // SDL_FRect unitRect = {100, 100, 32, 32};

    




    SDL_RenderClear(app->renderer);
    renderMap(app);

    // if (app->input.keyDown[SDL_SCANCODE_SPACE]) createUnit(1,1,1);


    SDL_RenderPresent(app->renderer);

    // reset edge flags
    Input_BeginFrame(&app->input);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)result;
    AppState *app = (AppState *)appstate;
    if (!app) return;
    
    if (app->spriteArcherBlue) SDL_DestroyTexture(app->spriteArcherBlue);
    if (app->spriteArcherRed) SDL_DestroyTexture(app->spriteArcherRed);
    if (app->spriteCavBlue) SDL_DestroyTexture(app->spriteCavBlue);
    if (app->spriteCavRed) SDL_DestroyTexture(app->spriteCavRed);
    if (app->spriteKnightBlue) SDL_DestroyTexture(app->spriteKnightBlue);
    if (app->spriteKnightRed) SDL_DestroyTexture(app->spriteKnightRed);
    if (app->spriteSBKnightBlue) SDL_DestroyTexture(app->spriteSBKnightBlue);
    if (app->spriteSBKnightRed) SDL_DestroyTexture(app->spriteSBKnightRed);
    if (app->tileGround) SDL_DestroyTexture(app->tileGround);
    if (app->tileWall) SDL_DestroyTexture(app->tileWall);
    if (app->tileRiver) SDL_DestroyTexture(app->tileRiver);
    if (app->tileRiver) SDL_DestroyTexture(app->tileRiver);
    if (app->tileBush) SDL_DestroyTexture(app->tileBush);   
    if (app->tileTower) SDL_DestroyTexture(app->tileTower);
    if (app->tileHigh) SDL_DestroyTexture(app->tileHigh);
    if (app->spriteSBKnightRed) SDL_DestroyTexture(app->spriteSBKnightRed);

    if (app->uiSettings) SDL_DestroyTexture(app->uiSettings);

    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    TTF_Quit();
    SDL_Quit();
    SDL_free(app);
}
