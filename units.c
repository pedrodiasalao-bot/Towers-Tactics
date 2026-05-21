#include "utils.h"
#include "units.h"
#include "map.h"
#include "cpathfinding.h"
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
            u->maxHP = 150;
            u->currentHP = 150;
            u->atk = 50;
            u->mvm = 4;
            u->baseRange = 1;
            u->range = u->baseRange;
        } else if (class == 2){
            // Archer
            u->maxHP = 50;
            u->currentHP = 50;
            u->atk = 75;
            u->mvm = 4;
            u->baseRange = 3;
            u->range = u->baseRange;
        } else if (class == 3){
            // Cavalry
            u->maxHP = 100;
            u->currentHP = 100;
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
    // Recycled logic from renderMap
    float centerX = WINDOW_WIDTH / 2.0f;
    float centerY = WINDOW_HEIGHT / 2.0f;
    float scaledW = TEXTURE_WIDTH * app->zoom;
    float scaledH = TEXTURE_HEIGHT * app->zoom;

    // For unit select UI, translates mouseX into grid base
    int mouseHoverX = (int)(((app->input.mouseX - centerX) / app->zoom + app->cameraX) / TEXTURE_WIDTH);
    int mouseHoverY = (int)(((app->input.mouseY - centerY) / app->zoom + app->cameraY) / TEXTURE_HEIGHT);

    for (int i = 0; i < app->unitCount; i++) // Cycles between all the current units
    {
        UnitStats *u = &app->units[i];

        float unitScreenX = centerX + ((u->x * TEXTURE_WIDTH) - app->cameraX) * app->zoom;
        float unitScreenY = centerY + ((u->y * TEXTURE_WIDTH) - app->cameraY) * app->zoom;
        SDL_FRect dst_rect = { unitScreenX, unitScreenY, scaledW, scaledH }; // So the units snap to the grid

    
        // Visual Feedback for getting picked up
        
        if ((int)u->x == mouseHoverX && (int)u->y == mouseHoverY && u->hasMoved == false) 
        {

            SDL_RenderTexture(app->renderer, app->uiSelect, NULL, &dst_rect);
        }
        if (i == app->selectedIndex && u->hasMoved == false)
        {

                CPath_Point reachable[MAP_ROWS * MAP_COLS];

                int count = cpath_get_reachable_positions(
                (int)u->x,         
                (int)u->y,        
                u->mvm,          
                MAP_COLS,         
                MAP_ROWS,          
                map_is_walkable,   
                map,               
                reachable,          
                MAP_ROWS * MAP_COLS 
    );
    
   for (int y = 0; y < MAP_ROWS; y++) {
                for (int x = 0; x < MAP_COLS; x++) {

                    if (map_is_walkable(x, y, map)) {
                    bool inAttackRange = false; // Checks if it's within the RANGE stat

                for (int p = 0; p < count; p++) {
                        int dist = abs(reachable[p].x - x) + abs(reachable[p].y - y);
                        if (dist <= u->range)    { 
                            inAttackRange = true;
                            break; 
                        }
                    }

                    if (inAttackRange) {

                        bool isWalkable = false;
                        for (int p = 0; p < count; p++) {
                          if (reachable[p].x == x && reachable[p].y == y) {

                        isWalkable = true;

                        break;
                    }
                }
                    if (!isWalkable) {
                        float redScreenX = centerX + ((x * TEXTURE_WIDTH) - app->cameraX) * app->zoom;
                        float redScreenY = centerY + ((y * TEXTURE_HEIGHT) - app->cameraY) * app->zoom;
                        SDL_FRect red_rect = { redScreenX, redScreenY, scaledW, scaledH };
                        SDL_RenderTexture(app->renderer, app->redSelect, NULL, &red_rect);
                        
                    }
                }
            }
        }
   }
        for (int p = 0; p < count; p++)
        {
                int x = reachable[p].x;
                int y = reachable[p].y;

                float blueScreenX = centerX + ((x * TEXTURE_WIDTH) - app->cameraX) * app->zoom;
                float blueScreenY = centerY + ((y * TEXTURE_HEIGHT) - app->cameraY) * app->zoom;
                SDL_FRect range_rect = { blueScreenX, blueScreenY, scaledW, scaledH };

                SDL_RenderTexture(app->renderer, app->blueSelect, NULL, &range_rect);
        
 

                        for (int j = 0; j < app->unitCount; j++) {
                        UnitStats *enemy = &app->units[j];
                        if (enemy->team != u->team && (int)enemy->x == x && (int)enemy->y == y){

                            char enemyTile = map[y][x];
                            if (u->class == 2 && enemyTile == 'B'){ // If unit is in archer's range AND in bush
                                continue;
                            }


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


int map_is_walkable(int x, int y, void *map_data) 
{
    char (*tiles)[MAP_COLS] = map_data;
    char tile = tiles[y][x];

    return tile != 'W' && tile != 'R' && tile != 'D';
}

// Checks if a single unit has an upgrade in the map, then returns a bool.
bool checkIfUnitHasBeenUpgraded(AppState *app, int class, int branch, int team)
{
    for (int i = 0; i < app->unitCount; i++)
    {
        if (app->units[i].class == class && app->units[i].team == team)
        {
            switch (branch)
            {
                case 1:
                    if (app->units[i].skillBranch1 == 1) return true;
                    break;
                case 2:
                    if (app->units[i].skillBranch2 == 1) return true;
                    break;
                case 3:
                    if (app->units[i].skillBranch3 == 1) return true;
                    break;
            }
        }
    }
    return false;
}

int obtainClickedUnitID(AppState *app, int gridX, int gridY)
{
    for (int i = 0; i < app->unitCount; i++)
    {
        if (app->units[i].x == gridX && app->units[i].y == gridY) return i;
    }
    return 0;
}

// This function makes it so all units of the same class and team have the same branch on their stats.
// Afterwards, the upgradeUnits function will apply stats to all units of the team who have that branch.
void applyBranchToAllUnits(AppState *app, int t, int class, int branch)
{
    for (int i = 0; i < app->unitCount; i++)
    {
        if (app->units[i].class == class && app->units[i].team == t)
        {
            switch (branch)
            {
                case 1:
                    app->units[i].skillBranch1 = 1;
                    break;
                case 2:
                    app->units[i].skillBranch2 = 1;
                    break;
                case 3:
                    app->units[i].skillBranch3 = 1;
                    break;
            }
        }
    }
}

// Applies upgraded stats to all classes of a team as long as they have the corresponding skill branch active.
void upgradeUnits(AppState *app, int t)
{
    for (int i = 0; i < app->unitCount; i++)
    {
        // KNIGHT UPGRADES
        if (app->units[i].class == 1 && app->units[i].team == t)
        {
            switch (app->units[i].skillBranch1)
            {
                case 0:
                    break;
                case 1: // 1st Upgrade
                    if (app->units[i].currentHP == app->units[i].maxHP) app->units[i].currentHP = 200;
                    app->units[i].maxHP = 200;
                    break;
            }
            switch (app->units[i].skillBranch2)
            {
                case 0:
                    break;
                case 1: // 2nd Upgrade
                    app->units[i].atk = 75;
                    break;
            }
            switch (app->units[i].skillBranch3)
            {
                case 0:
                    break;
                case 1: // 3rd Upgrade
                    app->units[i].baseRange = 2;
                    break;
            }
        }

        // ARCHER UPGRADES
        if (app->units[i].class == 2 && app->units[i].team == t)
        {
            switch (app->units[i].skillBranch1)
            {
                case 0:
                    break;
                case 1: // 1st Upgrade
                    app->units[i].atk = 100;
                    break;
            }
            switch (app->units[i].skillBranch2)
            {
                case 0:
                    break;
                case 1: // 2nd Upgrade
                    app->units[i].baseRange = 4;
                    break;
            }
            switch (app->units[i].skillBranch3)
            {
                case 0:
                    break;
                case 1: // 3rd Upgrade
                    app->units[i].mvm = 5;
                    break;
            }
        }

        // CAVALRY UPGRADES
        if (app->units[i].class == 3 && app->units[i].team == t)
        {
            switch (app->units[i].skillBranch1)
            {
                case 0:
                    break;
                case 1: // 1st Upgrade
                    app->units[i].mvm = 7;
                    break;
            }
            switch (app->units[i].skillBranch2)
            {
                case 0:
                    break;
                case 1: // 2nd Upgrade
                    app->units[i].atk = 125;
                    break;
            }
            switch (app->units[i].skillBranch3)
            {
                case 0:
                    break;
                case 1: // 3rd Upgrade
                    app->units[i].baseRange = 2;
                    break;
            }
        }
    }
}   
