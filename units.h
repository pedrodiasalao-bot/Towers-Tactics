#ifndef UNITS_H
#define UNITS_H

#include "utils.h"

typedef struct UnitStats
{
  int id; // Differenciating Units
  int type; // What class the unit is
  int team; // 0 - Blue /// 1 - Red

  // Positioning
  int x;
  int y;

  // Stats (WILL ADD MORE, WIP)
  int hp;
  int mvm;
  int range;

  bool hasMoved; // So you can't move a unit more than once

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


  // TODO: Use enum (check lab5)
    
    /*
    Classes:
        1 - Knight
        2 - Archer
        3 - Cavalry
    */
    /* int hp_current; // when this reaches 0, the unit 'dies'
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
    int y; */