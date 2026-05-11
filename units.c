#include "utils.h"
#include "units.h"
#include "map.h"
#include <stdlib.h>

int damageCalc(int enemyHealth, int damage, int enemyDef, int pen)
{
    return (enemyHealth - (damage - (enemyDef - (enemyDef * (pen / 100)))));
}

void createUnit(AppState *app, int class, int x, int y, int team)
{

    if (app->unitCount < 50) // Cap of 50 Units
    {
     
        UnitStats *u = &app->units[app->unitCount];

       // Temporary Stats
        u->id = app->unitCount;
        u->x = x;
        u->y = y;
        u->class = class; // 1 - knight; 2 - Archer; 3 - Cavalry

        if (class == 1){ 
            // Knight
            u->hp = 150;
            u->atk = 50;
            u->mvm = 4;
            u->baseRange = 1;
            u->range = u->baseRange;
        } else if (class == 2){
            u->hp = 50;
            u->atk = 75;
            u->mvm = 4;
            u->baseRange = 3;
            u->range = u->baseRange;
        } else if (class == 3){
            u->hp = 100;
            u->atk = 100;
            u->mvm = 6;
            u->baseRange = 1;
            u->range = u->baseRange;
        }

        u->team = team; // Differs them between the Blue (0) and Red (0)
        u->hasMoved = false;
  
        app->unitCount++; // Updates the Unit Count which in turn updates the unit ID
        
        printf("Unit spawned at %d, %d. Total units: %d\n", x, y, app->unitCount);
    }
    else
    {
        printf("Map full! Cannot spawn more units.\n");
    }
}


void renderUnits(AppState *app)
{
    for (int i = 0; i < app->unitCount; i++) // Cycles between all the current units
    {
        UnitStats *u = &app->units[i];
        SDL_FRect dst_rect = {u->x * TEXTURE_WIDTH, u->y * TEXTURE_HEIGHT, TEXTURE_WIDTH, TEXTURE_HEIGHT}; // So the units snap to the grid

        // For unit select UI, translates mouseX into grid base
        int mouseHoverX = (int)app->input.mouseX / TEXTURE_WIDTH;
        int mouseHoverY = (int)app->input.mouseY / TEXTURE_HEIGHT;
    
        // Visual Feedback for getting picked up
        /* TO DO: CHANGE YELLOW OUTLINE TO blueSelect AND redSelect TILES */
        if ((int)u->x == mouseHoverX && (int)u->y == mouseHoverY && u->hasMoved == false) {
            

            SDL_FRect hover_rect = {u->x * TEXTURE_WIDTH, u->y * TEXTURE_HEIGHT, TEXTURE_WIDTH, TEXTURE_HEIGHT};

            SDL_RenderTexture(app->renderer, app->uiSelect, NULL, &hover_rect);
        }
        if (i == app->selectedIndex && u->hasMoved == false)
        {
           
            // NOTE: For the sake of not frankensteining radius tiles again like last project, I used outside help for the blue & red render tiles math.
            for (int y = 0; y < MAP_ROWS; y++) 
            {
                for (int x = 0; x < MAP_COLS; x++) 
                {
                    int distance = abs(u->x - x) + abs(u->y - y);
                    SDL_FRect range_rect = {x * TEXTURE_WIDTH, y * TEXTURE_HEIGHT, TEXTURE_WIDTH, TEXTURE_HEIGHT};
                    //NOTE: Same logic used in unit movement

                    char tileRange = map[y][x];
                    bool isTileWalkable = (tileRange != 'W' && tileRange != 'R');

                    if (distance >= 0 && distance <= u->mvm)
                    {
                        if (isTileWalkable){
                        SDL_RenderTexture(app->renderer, app->blueSelect, NULL, &range_rect);

                        {

                        for (int j = 0; j < app->unitCount; j++) {
                        UnitStats *enemy = &app->units[j];
                        if (enemy->team != u->team && (int)enemy->x == x && (int)enemy->y == y){
                            SDL_RenderTexture(app->renderer, app->redSelect, NULL, &range_rect);
                        
                        }
                        }}}else{
                            
                        }
                    } else  if (distance > u->mvm && distance <= (u->mvm + u->range))
                    {
                        SDL_RenderTexture(app->renderer, app->redSelect, NULL, &range_rect);

                        
                    }
                    
                    

                }
          
        }
    }

        if (u->hasMoved){ 
            SDL_SetTextureAlphaMod(app->spriteKnightBlue, 128);
            SDL_SetTextureAlphaMod(app->spriteArcherBlue, 128);
            SDL_SetTextureAlphaMod(app->spriteCavBlue, 128);
            SDL_SetTextureAlphaMod(app->spriteKnightRed, 128);
            SDL_SetTextureAlphaMod(app->spriteArcherRed, 128);
            SDL_SetTextureAlphaMod(app->spriteCavRed, 128);
        } else{
            SDL_SetTextureAlphaMod(app->spriteKnightBlue, 255);
            SDL_SetTextureAlphaMod(app->spriteArcherBlue, 255);
            SDL_SetTextureAlphaMod(app->spriteCavBlue, 255);
            SDL_SetTextureAlphaMod(app->spriteKnightRed, 255);
            SDL_SetTextureAlphaMod(app->spriteArcherRed, 255);
            SDL_SetTextureAlphaMod(app->spriteCavRed, 255);
        }
        // Render the Unit Sprite after the background squares so it's on top.
       if (u->team == 0) {
        if (u->class == 1){
        SDL_RenderTexture(app->renderer, app->spriteKnightBlue, NULL, &dst_rect);
        }else if (u->class == 2){
        SDL_RenderTexture(app->renderer, app->spriteArcherBlue, NULL, &dst_rect);
        }else if (u->class == 3){
        SDL_RenderTexture(app->renderer, app->spriteCavBlue, NULL, &dst_rect);
        }
        } else {
        if (u->class == 1){
        SDL_RenderTexture(app->renderer, app->spriteKnightRed, NULL, &dst_rect);
        }else if (u->class == 2){
        SDL_RenderTexture(app->renderer, app->spriteArcherRed, NULL, &dst_rect);
        }else if (u->class == 3){
        SDL_RenderTexture(app->renderer, app->spriteCavRed, NULL, &dst_rect);
        }
}
}
}

/*
void movementTilesRange(int unitX, int unitY, int movementRange);
{
    if (checkSurroundingTiles("G", unitX, unitY) == true)
    {
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 255, 100);
        SDL_RenderRect(app->renderer, &dst_rect);
    }
}
    */


void unitTileInteraction(AppState *app) {
    
    for (int i = 0; i < app->unitCount; i++) // Cycles between all the current units
    {
        UnitStats *u = &app->units[i];

        // Converts map coordinates to unit coordinates, present in unit movement
        int mapX = (int)u->x; 
        int mapY = (int)u->y;

        if (mapX >= 0 && mapX < MAP_COLS && mapY >= 0 && mapY < MAP_ROWS) {
                    char tileRange = map[mapY][mapX];

                    bool isTileHigh = (tileRange == 'T' || tileRange == 'H');

                if (isTileHigh) {
                    u->range = u->baseRange * 2;
                } else {
                    u->range = u->baseRange;
                }
}
}
}
