#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Utils/sdl_utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "utils.h"
#include "map.h"
#include "units.h"

//DEBUG
static void drawText(
    SDL_Renderer* renderer,
    float x,
    float y,
    float scale,
    Uint8 r,
    Uint8 g,
    Uint8 b,
    Uint8 a,
    const char* text
)
{
    float oldX, oldY;
    Uint8 oldR, oldG, oldB, oldA;

    SDL_GetRenderScale(renderer, &oldX, &oldY);
    SDL_GetRenderDrawColor(renderer, &oldR, &oldG, &oldB, &oldA);

    SDL_SetRenderScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_RenderDebugTextFormat(
        renderer,
        x / scale,
        y / scale,
        "%s",
        text
    );

    // Restore previous state
    SDL_SetRenderScale(renderer, oldX, oldY);
    SDL_SetRenderDrawColor(renderer, oldR, oldG, oldB, oldA);
}
//DEBUG

// Pedro
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

void endTurn(AppState *app) {

    // Resetting all the unit movement so they can be moved again
    for (int i = 0; i < app->unitCount; i++) { // Loops through all the units currently in the map
        app->units[i].hasMoved = false;
    
    }

    // app->turnCounter++; // Increases the turn counter.
 
    // Making the sides switch (0 = Blue; 1 = Red)
    if (app->currentTurn == 0) {
        app->currentTurn = 1;
    } else {
        app->currentTurn = 0;
    }

    if (app->currentTurn == 0)
    {
        app->turnCounter++;

        if(app->turnCounter % 3 == 0)
    {
        createUnit(app, (rand() % 3) + 1, 1, 16, 0); // Prototype Test (1 - Type; 5,5 - Position; 0 - Blue Team)
        createUnit(app, (rand() % 3) + 1, 30, 1, 1);
    }
        capturePointMechanics(app);
    }

    updateTextTexture(app);
}

void renderUI(AppState *app){

    // Turn Text
    if (app->turnTextTexture){
        float w, h;
        SDL_GetTextureSize(app->turnTextTexture, &w, &h);
        SDL_FRect dstRect = {(WINDOW_WIDTH / 2.0f) - (w / 2.0f), 20.0f, w, h};
        SDL_RenderTexture(app->renderer, app->turnTextTexture, NULL, &dstRect);

    // Settings Button

    SDL_FRect buttonRect = {20.0f, 20.0f, 64.0f, 64.0f };

    bool hovered = (app->input.mouseX >= buttonRect.x && 
                    app->input.mouseX <= buttonRect.x + buttonRect.w &&
                    app->input.mouseY >= buttonRect.y && 
                    app->input.mouseY <= buttonRect.y + buttonRect.h);

    if(hovered && app->currentState != STATE_SKILL_MENU){
    SDL_RenderTexture(app->renderer, app->uiSettingsHover, NULL, &buttonRect);
    }else{
    SDL_RenderTexture(app->renderer, app->uiSettings, NULL, &buttonRect);
    }

    

    // Skill Tree Button

    float treeW = TEXTURE_WIDTH * 3.0f;
    float treeH = TEXTURE_HEIGHT * 3.0f;

    SDL_FRect treeRect = { WINDOW_WIDTH - treeW, WINDOW_HEIGHT - treeH, treeW, treeH};

    // When you select a unit, the tree becomes full opacity
    Uint8 alphaValue = 128;
    if (app->selectedIndex != -1) {
        alphaValue = 255;
    }
    
    bool treeHovered = (app->input.mouseX >= treeRect.x && 
                    app->input.mouseX <= treeRect.x + treeRect.w &&
                    app->input.mouseY >= treeRect.y && 
                    app->input.mouseY <= treeRect.y + treeRect.h);

  
    if (treeHovered && (app->selectedIndex != -1 || app->currentState == STATE_SKILL_MENU)) {
        SDL_SetTextureAlphaMod(app->uiSkillTreeHover, alphaValue);
        SDL_RenderTexture(app->renderer, app->uiSkillTreeHover, NULL, &treeRect);
          }
    else{
        SDL_SetTextureAlphaMod(app->uiSkillTree, alphaValue);
        SDL_RenderTexture(app->renderer, app->uiSkillTree, NULL, &treeRect);
    }

    // EXP Ball 

    float expW = TEXTURE_WIDTH * 0.6f;
    float expH = TEXTURE_HEIGHT * 0.6f;

    SDL_FRect expRect = {WINDOW_WIDTH - treeW - expW, WINDOW_HEIGHT - (expH + 10.0f), expW, expH};

    SDL_RenderTexture(app->renderer, app->uiExperiencePoint, NULL, &expRect);
    
    // drawText(app->renderer, WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 120, 3.0f, 255, 255, 255, 255, "QUIT");
}
 
}



bool allUnitsMoved (AppState *app) {
    // Check to see if all units of the team in question has moved so the turn automatically ends
    for (int i = 0; i < app->unitCount; i++){
        if (app->units[i].team == app->currentTurn) {
            if (!app->units[i].hasMoved){
                return false;
            }
        }
    }
    return true;
}

void attackUnit (AppState *app, int attacker, int defender){

    UnitStats *atk = &app->units[attacker]; // Refers to the unit that's attacking
    UnitStats *currentHP = &app->units[defender];  // Refers to the unit that's defending

    currentHP->currentHP -= atk->atk; // Subtracts the defender's HP depending on the attacker's ATK stat

    if (currentHP->currentHP <= 0){ // If unit dies, change the order of the array
        for (int i = defender; i < app->unitCount - 1; i++){
            app->units[i] = app->units[i + 1];
        }
    app->unitCount--; // Removes them from the Unit Count
    }

atk->hasMoved = true; // The attacker's move turn gets consumed
app->selectedIndex = -1; // Unit is unselected

}

void update(AppState *app) {

    switch(app->currentState) {
        case STATE_MENU:
            if (app->input.mouseLeftDown) {
                app->currentState = STATE_PLAYING;

            }
         break;

      
        
        
    }
}



void updateTextTexture (AppState *app){
    // Changing the Text Color from Red to Blue or Blue to Red depending on whose turn it is

    char message[32];
    SDL_Color color;

    if(app->currentTurn == 0) {
        sprintf(message, "BLUE MOVEMENT TURN");
        color = (SDL_Color){0, 0, 255, 255};
    }
    if (app->currentTurn == 1){
        sprintf(message, "RED MOVEMENT TURN");
        color = (SDL_Color){255, 0, 0, 255};
    }
 
    if (app->turnTextTexture) SDL_DestroyTexture(app->turnTextTexture);

    SDL_Surface *surf = TTF_RenderText_Blended(app->font, message, 0, color);
    if (surf) {
        app->turnTextTexture = SDL_CreateTextureFromSurface(app->renderer, surf);
        SDL_DestroySurface(surf);
} }

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

    if (!TTF_Init()) {
    SDL_Log("TTF_Init failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
    }
    /* Create window + renderer once at startup */
    if (!SDL_CreateWindowAndRenderer("Towers and Tactics", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &app->window, &app->renderer)) {
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
    app->tileCaptureRed = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeCaptureRed.png");
    app->tileCaptureBlue = sdl_load_texture(app->renderer, "Assets/Art/Prototype/TL_PrototypeCaptureBlue.png");

    app->blueSelect = sdl_load_texture(app->renderer, "Assets/Art/SP_BlueSelect.png");
    app->redSelect = sdl_load_texture(app->renderer, "Assets/Art/SP_RedSelect.png");
    app->greenSelect = sdl_load_texture(app->renderer, "Assets/Art/SP_GreenSelect.png");
    app->uiSelect = sdl_load_texture(app->renderer, "Assets/Art/UI_Select.png");
    app->uiSettingsHover = sdl_load_texture(app->renderer, "Assets/Art/UI_SettingsHovered.png");
    app->uiSkillTree = sdl_load_texture(app->renderer, "Assets/Art/UI_SkillTree.png");
    app->uiSkillTreeHover = sdl_load_texture(app->renderer, "Assets/Art/UI_SkillTreeHover.png");
    app->uiExperiencePoint = sdl_load_texture(app->renderer, "Assets/Art/UI_ExperiencePoint.png");
    app->skillTreeMenuBase = sdl_load_texture(app->renderer, "Assets/Art/skillTree.png");
    app->skillTreeMenuSelection = sdl_load_texture(app->renderer, "Assets/Art/skillSelection.png");


    app->font = TTF_OpenFont("Assets/PressStart2P.ttf", 24);

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
    set_nearest(app->blueSelect);
    set_nearest(app->redSelect);
    set_nearest(app->greenSelect);
    set_nearest(app->tileCaptureRed);
    set_nearest(app->tileCaptureBlue);
    set_nearest(app->uiSelect);
    set_nearest(app->uiSettingsHover);
    set_nearest(app->uiSkillTree);
    set_nearest(app->uiSkillTreeHover);
    set_nearest(app->uiExperiencePoint);
    set_nearest(app->skillTreeMenuBase);
    set_nearest(app->skillTreeMenuSelection);

    /* PROTOTYPE ONLY */
    loadMap("map.txt");

    app->isASingleUnitStandingInPoint = false;
    // app->capPointProgress[9];

    // Camera Initialization
    app->cameraX = WINDOW_WIDTH / 2.0f;
    app->cameraY = WINDOW_HEIGHT / 2.0f;
    app->zoom = 1.0f;
    app->isDragging = false;

    app->unitCount = false;
    app->selectedIndex = -1;
    app->lastTicksMS = SDL_GetTicks();
    createUnit(app, 1, 2, 16, 0); // Prototype Test (1 - Type; 5,5 - Position; 0 - Blue Team)
    createUnit(app, 1, 30, 2, 1); 
    createUnit(app, 2, 1, 15, 0);
    createUnit(app, 2, 29, 1, 1);
    createUnit(app, 3, 2, 15, 0);
    createUnit(app, 3, 29, 2, 1);
    updateTextTexture(app);

   
   
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

        if (app->isDragging && app->currentState == STATE_PLAYING) {
            float deltaX = event->motion.x - app->lastMouseX;
            float deltaY = event->motion.y - app->lastMouseY;

            // Camera Drag
            app->cameraX -= deltaX / app->zoom;
            app->cameraY -= deltaY / app->zoom;


            // Making sure players can't zoom or drag out of borders
            float worldWidthPixels = MAP_COLS * TEXTURE_WIDTH;
            float worldHeightPixels = MAP_ROWS * TEXTURE_HEIGHT;

            float minCamX = (WINDOW_WIDTH / 2.0f) / app->zoom;
            float minCamY = (WINDOW_HEIGHT / 2.0f) / app->zoom;

            float maxCamX = worldWidthPixels - minCamX;
            float maxCamY = worldHeightPixels - minCamY;

            if (maxCamX < minCamX) {
                app->cameraX = worldWidthPixels / 2.0f;
            } else {
                if (app->cameraX < minCamX) app->cameraX = minCamX;
                if (app->cameraX > maxCamX) app->cameraX = maxCamX;
            }

            if (maxCamY < minCamY) {
                app->cameraY = worldHeightPixels / 2.0f;
            } else {
                if (app->cameraY < minCamY) app->cameraY = minCamY;
                if (app->cameraY > maxCamY) app->cameraY = maxCamY;
            }

            app->lastMouseX = event->motion.x;
            app->lastMouseY = event->motion.y;
        }

        break;

    // CAMERA MOVEMENT:
case SDL_EVENT_MOUSE_WHEEL:

    if (app->currentState == STATE_PLAYING) {

        if (event->wheel.y > 0) {
            app->zoom += 0.3f;
        } else if (event->wheel.y < 0) {
            app->zoom -= 0.3f;
    }
        if (app->zoom < 1.0f) app->zoom = 1.0f;
        if (app->zoom > 2.0f) app->zoom = 2.0f;
            
    }
    break;
    // UNIT MOVEMENT:

    case SDL_EVENT_MOUSE_BUTTON_DOWN:

     if(event->button.button == SDL_BUTTON_RIGHT && app->currentState == STATE_PLAYING) {
            app->isDragging = true;
            app->lastMouseX = event->button.x;
            app->lastMouseY = event->button.y;

            return SDL_APP_CONTINUE;
        }

        if (app->currentState == STATE_MENU || app->currentState == STATE_PAUSE)
        {
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                app->currentState = STATE_PLAYING;
            }
        }

        if (app->currentState == STATE_SKILL_MENU)
        {
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                app->currentState = STATE_PLAYING;
            }
        }

    else if (app->currentState == STATE_PLAYING){
            if (!in->mouseLeftDown) in->mouseLeftPressed = true;
            in->mouseLeftDown = true;

            SDL_FRect buttonRect = {20.0f, 20.0f, 64.0f, 64.0f };

            if (event->button.x >= buttonRect.x && 
                event->button.x <= buttonRect.x + buttonRect.w &&
                event->button.y >= buttonRect.y && 
                event->button.y <= buttonRect.y + buttonRect.h) {
                    app->currentState = STATE_PAUSE;
                    return SDL_APP_CONTINUE;
                }
            
            float treeW = TEXTURE_WIDTH * 3.0f;
            float treeH = TEXTURE_HEIGHT * 3.0f;
            SDL_FRect treeRect = {WINDOW_WIDTH - treeW, WINDOW_HEIGHT - treeH, treeW, treeH};

            if (event->button.x >= treeRect.x && 
                event->button.x <= treeRect.x + treeRect.w &&
                event->button.y >= treeRect.y && 
                event->button.y <= treeRect.y + treeRect.h &&
                app->selectedIndex != -1) {
                    app->currentState = STATE_SKILL_MENU;
                    return SDL_APP_CONTINUE;
                }


            float centerX = WINDOW_WIDTH / 2.0f;
            float centerY = WINDOW_HEIGHT / 2.0f;
            // mouse.x and mouse.y = Coordinates of the button press
            // NOTE: Adapted for camera movement
            
            int gridX = (int)(((event->button.x - centerX) / app->zoom + app->cameraX) / TEXTURE_WIDTH);
            int gridY = (int)(((event->button.y - centerY) / app->zoom + app->cameraY) / TEXTURE_HEIGHT);

            char moveTile = map[gridY][gridX]; // Tile the unit is going to be moved to
            bool isTileWalkable = (moveTile != 'W' && moveTile != 'R'); // Checks if the tile is a wall or river

            if (app->input.mouseLeftPressed)
        { 
            // If unit is already picked up (And if it hasn't moved yet):
            if (app->selectedIndex != -1) 
{ 
    UnitStats *u = &app->units[app->selectedIndex];
    int targetIdx = -1;

    // Check if there is a unit on the tile we clicked
    for (int i = 0; i < app->unitCount; i++) {
        if (app->units[i].x == gridX && app->units[i].y == gridY) {
            targetIdx = i;
            break;
        }
    }

    int distance = abs(u->x - gridX) + abs(u->y - gridY);

    // Triggers if clicked on an enemy unit
    if (targetIdx != -1 && app->units[targetIdx].team != u->team) 
    {
        char defenderTile = map[app->units[targetIdx].y][app->units[targetIdx].x];
        bool isHidingBush = (u->class == 2 && defenderTile == 'B');            // Checks if unit is sitting on a bush

        if (distance <= u->range) {
            if (isHidingBush){

            }else{
            attackUnit(app, app->selectedIndex, targetIdx); // The function we built earlier
        } 
    }
}
    // Walks to spot if clicked on a walkable tile
    else if (targetIdx == -1 && isTileWalkable) 
    {
        if (distance <= u->mvm) {
            u->x = gridX;
            u->y = gridY;
            u->hasMoved = true;
            app->selectedIndex = -1;
        }
    }
    // Cancels movement if out of range or clicked on same team unit.
    else 
    {
        app->selectedIndex = -1; 
    }
}else 
    {
            // If unit isn't already picked up, then pick it up.
            
        for (int i = 0; i < app->unitCount; i++) 
        {
            if (app->units[i].x == gridX && app->units[i].y == gridY) 
            {
                if(app->units[i].team == app->currentTurn && !app->units[i].hasMoved)
        {
                if(app->units[i].team == app->currentTurn)
                { 
                app->selectedIndex = i;
                break;
                } }
        }
    } 

}
    } 
} break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            in->mouseLeftDown    = false;
            in->mouseLeftReleased = true;
        }   

        if (event->button.button == SDL_BUTTON_RIGHT)
        {
            app->isDragging = false;
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

    if (app->currentState == STATE_MENU) {
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);
        drawText(app->renderer, WINDOW_WIDTH/2 - 350, WINDOW_HEIGHT/2, 5.0f, 255, 255, 255, 255, "TOWERS AND TACTICS");
        drawText(app->renderer, WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 120, 1.0f, 255, 255, 255, 255, "CLICK TO START");
        drawText(app->renderer, 40, WINDOW_HEIGHT/3, 1.5f, 255, 255, 255, 255, "Win Conditions:\n\n -> Capture Enemy Base Flag\nOR\nCapture All Map Flags\n\n\nControls:\n\nLEFT CLICK: Click on units to move them\nSPACE: Skip Turn\nSCROLL WHEEL: Zoom in and out");

    }else if (app->currentState == STATE_PLAYING)
    {

    if (app->input.keyPressed[SDL_SCANCODE_SPACE] || allUnitsMoved(app))
    {       
        endTurn(app);
    }

    SDL_RenderClear(app->renderer);
    renderMap(app);
    renderUnits(app);
    renderUI(app);
 
    unitTileInteraction(app);

}  else if (app->currentState == STATE_PAUSE){

        SDL_RenderClear(app->renderer);
        renderMap(app);
        renderUnits(app);
        renderUI(app);

        SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 125);

        SDL_FRect screenOverlay = {0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(app->renderer, &screenOverlay);

        drawText(app->renderer, WINDOW_WIDTH/2 - 110, WINDOW_HEIGHT/2, 3.0f, 255, 255, 255, 255, "CONTINUE");
        drawText(app->renderer, WINDOW_WIDTH/2 - 75, WINDOW_HEIGHT/2 + 120, 3.0f, 255, 255, 255, 255, "QUIT");


}

    else if (app->currentState == STATE_SKILL_MENU)
    {

        SDL_RenderClear(app->renderer);
        renderMap(app);
        renderUnits(app);
        renderUI(app);

        SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 125);

        SDL_FRect screenOverlay = {0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(app->renderer, &screenOverlay);

        SDL_FRect skillTreeRect = {300.0f, 50.0f, 800.0f, 800.0f};
        SDL_RenderTexture(app->renderer, app->skillTreeMenuBase, NULL, &skillTreeRect);

        SDL_FRect skillTreeCharacterRect = {325.0f, 410.0f, 80.0f, 80.0f};
        // Knight skill tree
        if (app->units[app->selectedIndex].class == 1)
        {
            drawText(app->renderer, 780, 165, 4, 255, 255, 255, 255, "+ Health");
            drawText(app->renderer, 780, 435, 4, 255, 255, 255, 255, "+ Attack");
            drawText(app->renderer, 780, 705, 3, 255, 255, 255, 255, "+ ATK Range");

            if (app->units[app->selectedIndex].team == 0) SDL_RenderTexture(app->renderer, app->spriteKnightBlue, NULL, &skillTreeCharacterRect);
            if (app->units[app->selectedIndex].team == 1) SDL_RenderTexture(app->renderer, app->spriteKnightRed, NULL, &skillTreeCharacterRect);
        }
        // Archer skill tree
        if (app->units[app->selectedIndex].class == 2)
        {
            drawText(app->renderer, 780, 165, 4, 255, 255, 255, 255, "+ Attack");
            drawText(app->renderer, 780, 437, 3, 255, 255, 255, 255, "+ ATK Range");
            drawText(app->renderer, 780, 705, 3, 255, 255, 255, 255, "+ Movement");

            if (app->units[app->selectedIndex].team == 0) SDL_RenderTexture(app->renderer, app->spriteArcherBlue, NULL, &skillTreeCharacterRect);
            if (app->units[app->selectedIndex].team == 1) SDL_RenderTexture(app->renderer, app->spriteArcherRed, NULL, &skillTreeCharacterRect);
        }
        // Cavalry skill tree
        if (app->units[app->selectedIndex].class == 3)
        {
            drawText(app->renderer, 780, 170, 3, 255, 255, 255, 255, "+ Movement");
            drawText(app->renderer, 780, 435, 4, 255, 255, 255, 255, "+ Attack");
            drawText(app->renderer, 780, 705, 4, 255, 255, 255, 255, "+ Health");

            if (app->units[app->selectedIndex].team == 0) SDL_RenderTexture(app->renderer, app->spriteCavBlue, NULL, &skillTreeCharacterRect);
            if (app->units[app->selectedIndex].team == 1) SDL_RenderTexture(app->renderer, app->spriteCavRed, NULL, &skillTreeCharacterRect);
        }

        // Selection png
        int selectionMult = 130;
        SDL_FRect selectionRect1 = {713, 117, selectionMult * 3, selectionMult};
        SDL_FRect selectionRect2 = {713, 385, selectionMult * 3, selectionMult};
        SDL_FRect selectionRect3 = {713, 653, selectionMult * 3, selectionMult};

        bool hovered1 = (app->input.mouseX >= selectionRect1.x && 
                         app->input.mouseX <= selectionRect1.x + selectionRect1.w &&
                         app->input.mouseY >= selectionRect1.y && 
                         app->input.mouseY <= selectionRect1.y + selectionRect1.h);

        bool hovered2 = (app->input.mouseX >= selectionRect2.x && 
                         app->input.mouseX <= selectionRect2.x + selectionRect2.w &&
                         app->input.mouseY >= selectionRect2.y && 
                         app->input.mouseY <= selectionRect2.y + selectionRect2.h);

        bool hovered3 = (app->input.mouseX >= selectionRect3.x && 
                         app->input.mouseX <= selectionRect3.x + selectionRect3.w &&
                         app->input.mouseY >= selectionRect3.y && 
                         app->input.mouseY <= selectionRect3.y + selectionRect3.h);

        SDL_FRect selectionRect = {732, 134, (selectionMult * 3) * 0.902, (selectionMult) * 0.74};
        SDL_RenderTexture(app->renderer, app->skillTreeMenuSelection, NULL, &selectionRect);

        SDL_SetTextureAlphaMod(app->skillTreeMenuSelection, 255);

        if (hovered1 && app->units[app->selectedIndex].skillBranch1 != 1);
    }

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
    if (app->skillTreeMenuBase) SDL_DestroyTexture(app->skillTreeMenuBase);
    if (app->skillTreeMenuSelection) SDL_DestroyTexture(app->skillTreeMenuSelection);

    if (app->uiSettings) SDL_DestroyTexture(app->uiSettings);

    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window) SDL_DestroyWindow(app->window);
    if (app->turnTextTexture) SDL_DestroyTexture(app->turnTextTexture);
    if (app->font) TTF_CloseFont(app->font);
    TTF_Quit();
    SDL_Quit();
    SDL_free(app);
} 
