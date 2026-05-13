#ifndef CPATHFINDING_H
#define CPATHFINDING_H

typedef struct CPath_Point {
    int x;
    int y;
} CPath_Point;

typedef int (*CPath_IsWalkable)(int x, int y, void *map_data);

int cpath_is_inside(int x, int y, int width, int height);

int cpath_find_path(
    int start_x,
    int start_y,
    int goal_x,
    int goal_y,
    int width,
    int height,
    CPath_IsWalkable is_walkable,
    void *map_data,
    CPath_Point *out_path,
    int out_capacity
);

int cpath_get_reachable_positions(
    int start_x,
    int start_y,
    int distance,
    int width,
    int height,
    CPath_IsWalkable is_walkable,
    void *map_data,
    CPath_Point *out_positions,
    int out_capacity
);

#endif

