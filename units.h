#ifndef UNITS_H
#define UNITS_H

#include "utils.h"

typedef struct UnitStats
{
    // TODO: Use enum (check lab5)
    int class;
    /*
    Classes:
        1 - Knight
        2 - Archer
        3 - Cavalry
    */
    int hp_current; // when this reaches 0, the unit 'dies'
    int hp_max;
    int def; // flat amount to reduce incoming damage by
    int atk;
    int pen; // % amount to ignore def during attacks
    int mvm; // how many tiles the unit can move in the movement turn
    int crit; // % chance for an attack to deal double damage
    int range; // attack range in tiles
    int deadzone; // specifically for the Archer class: area around the unit where attack is disabled, in tiles

    Classes classes;
    
    int x;
    int y;

}UnitStats;

void createUnit(AppState *app, int class, int x, int y);
void renderUnits();
//determines and renders which tiles around the unit selected they can move to
void movementTilesRange(int unitMovementTiles);
//determines and renders which tiles around the unit selected they can attack
void attackTilesRange(int unitAttackRange);
void turnMechanics();
void movementPhase();
void attackPhase();
void destroyUnit();
int damageCalc(int enemyHealth, int damage, int enemyDef, int pen);


#endif