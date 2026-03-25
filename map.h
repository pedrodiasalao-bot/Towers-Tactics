#ifndef MAP_H
#define MAP_H

#define MAP_ROWS 18
#define MAP_COLS 32

#define TEXTURE_WIDTH 32
#define TEXTURE_HEIGHT 32

extern char map[MAP_ROWS][MAP_COLS];
void loadMap();
void renderMap(AppState *app);
void capturePointMechanics();


#endif