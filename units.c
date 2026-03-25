#include "utils.h"
#include "units.h"
#include "map.h"

int damageCalc(int enemyHealth, int damage, int enemyDef, int pen)
{
    return (enemyHealth - (damage - (enemyDef - (enemyDef * (pen / 100)))));
}

void createUnit(AppState *app, int class, int x, int y)
{
    if(app->units == NULL)
    {
    app->units = malloc(1, sizeof(UnitStats));

    app->units[0].class = 1;
    
    app->units[0].atk = 50;
    app->units[0].hp_max = 150;
    app->units[0].mvm = 2;
    app->units[0].range = 1;
    app->units[0].crit = 0;
    app->units[0].def = 20;
    app->units[0].pen = 0;
    app->units[0].deadzone = 0;
    app->units[0].hp_current = 150;
    
    }
}
void renderUnits()
{

}