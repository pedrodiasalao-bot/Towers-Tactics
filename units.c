#include "utils.h"
#include "units.h"
#include "map.h"

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
        u->hp = 150;
        u->mvm = 2;
        u->class = class; // 1 - knight; 2 - Archer; 3 - Cavalry
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

        if (u->team == 0) {

        SDL_RenderTexture(app->renderer, app->spriteKnightBlue, NULL, &dst_rect);
        } else {
        SDL_RenderTexture(app->renderer, app->spriteKnightRed, NULL, &dst_rect);
        }

            

        // Visual Feedback for getting picked up
        /* TO DO: CHANGE YELLOW OUTLINE TO blueSelect AND redSelect TILES */
        if (i == app->selectedIndex)
        {
            SDL_SetRenderDrawColor(app->renderer, 255, 255, 0, 255);
            SDL_RenderRect(app->renderer, &dst_rect);
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