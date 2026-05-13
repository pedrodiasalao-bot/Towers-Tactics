#include "cpathfinding.h"

#include <limits.h>
#include <stdlib.h>

static int cpath_index(int x, int y, int width)
{
    return y * width + x;
}

static void cpath_index_to_point(int index, int width, CPath_Point *point)
{
    point->x = index % width;
    point->y = index / width;
}

static int cpath_grid_is_valid(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return 0;
    }

    if (width > INT_MAX / height) {
        return 0;
    }

    return 1;
}

static int cpath_can_walk(
    int x,
    int y,
    int width,
    int height,
    CPath_IsWalkable is_walkable,
    void *map_data
)
{
    if (!cpath_is_inside(x, y, width, height)) {
        return 0;
    }

    return is_walkable(x, y, map_data);
}

int cpath_is_inside(int x, int y, int width, int height)
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

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
)
{
    int tile_count;
    int start_index;
    int goal_index;
    int queue_start = 0;
    int queue_end = 0;
    int found = 0;
    int path_length = 0;
    int current;
    int i;
    int *queue;
    int *parent;
    unsigned char *visited;

    static const int direction_x[4] = { 1, -1, 0, 0 };
    static const int direction_y[4] = { 0, 0, 1, -1 };

    if (
        is_walkable == NULL ||
        out_path == NULL ||
        out_capacity <= 0 ||
        !cpath_grid_is_valid(width, height)
    ) {
        return 0;
    }

    if (
        !cpath_can_walk(start_x, start_y, width, height, is_walkable, map_data) ||
        !cpath_can_walk(goal_x, goal_y, width, height, is_walkable, map_data)
    ) {
        return 0;
    }

    tile_count = width * height;
    start_index = cpath_index(start_x, start_y, width);
    goal_index = cpath_index(goal_x, goal_y, width);

    if (start_index == goal_index) {
        out_path[0].x = start_x;
        out_path[0].y = start_y;
        return 1;
    }

    queue = (int *)malloc((size_t)tile_count * sizeof(int));
    parent = (int *)malloc((size_t)tile_count * sizeof(int));
    visited = (unsigned char *)calloc((size_t)tile_count, sizeof(unsigned char));

    if (queue == NULL || parent == NULL || visited == NULL) {
        free(queue);
        free(parent);
        free(visited);
        return 0;
    }

    for (i = 0; i < tile_count; i++) {
        parent[i] = -1;
    }

    queue[queue_end++] = start_index;
    visited[start_index] = 1;

    while (queue_start < queue_end && !found) {
        current = queue[queue_start++];

        for (i = 0; i < 4; i++) {
            CPath_Point current_point;
            int next_x;
            int next_y;
            int next_index;

            cpath_index_to_point(current, width, &current_point);
            next_x = current_point.x + direction_x[i];
            next_y = current_point.y + direction_y[i];

            if (!cpath_can_walk(next_x, next_y, width, height, is_walkable, map_data)) {
                continue;
            }

            next_index = cpath_index(next_x, next_y, width);

            if (visited[next_index]) {
                continue;
            }

            visited[next_index] = 1;
            parent[next_index] = current;
            queue[queue_end++] = next_index;

            if (next_index == goal_index) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        free(queue);
        free(parent);
        free(visited);
        return 0;
    }

    current = goal_index;
    while (current != -1) {
        path_length++;
        current = parent[current];
    }

    if (path_length > out_capacity) {
        free(queue);
        free(parent);
        free(visited);
        return 0;
    }

    current = goal_index;
    for (i = path_length - 1; i >= 0; i--) {
        cpath_index_to_point(current, width, &out_path[i]);
        current = parent[current];
    }

    free(queue);
    free(parent);
    free(visited);

    return path_length;
}

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
)
{
    int tile_count;
    int start_index;
    int queue_start = 0;
    int queue_end = 0;
    int output_count = 0;
    int i;
    int *queue;
    int *steps;
    unsigned char *visited;

    static const int direction_x[4] = { 1, -1, 0, 0 };
    static const int direction_y[4] = { 0, 0, 1, -1 };

    if (
        is_walkable == NULL ||
        out_positions == NULL ||
        out_capacity < 0 ||
        distance < 0 ||
        !cpath_grid_is_valid(width, height)
    ) {
        return 0;
    }

    if (!cpath_can_walk(start_x, start_y, width, height, is_walkable, map_data)) {
        return 0;
    }

    if (distance == 0) {
        return 0;
    }

    tile_count = width * height;
    start_index = cpath_index(start_x, start_y, width);

    queue = (int *)malloc((size_t)tile_count * sizeof(int));
    steps = (int *)malloc((size_t)tile_count * sizeof(int));
    visited = (unsigned char *)calloc((size_t)tile_count, sizeof(unsigned char));

    if (queue == NULL || steps == NULL || visited == NULL) {
        free(queue);
        free(steps);
        free(visited);
        return 0;
    }

    for (i = 0; i < tile_count; i++) {
        steps[i] = 0;
    }

    queue[queue_end++] = start_index;
    visited[start_index] = 1;

    while (queue_start < queue_end) {
        int current = queue[queue_start++];
        CPath_Point current_point;

        if (steps[current] >= distance) {
            continue;
        }

        cpath_index_to_point(current, width, &current_point);

        for (i = 0; i < 4; i++) {
            int next_x = current_point.x + direction_x[i];
            int next_y = current_point.y + direction_y[i];
            int next_index;

            if (!cpath_can_walk(next_x, next_y, width, height, is_walkable, map_data)) {
                continue;
            }

            next_index = cpath_index(next_x, next_y, width);

            if (visited[next_index]) {
                continue;
            }

            visited[next_index] = 1;
            steps[next_index] = steps[current] + 1;
            queue[queue_end++] = next_index;

            if (output_count >= out_capacity) {
                free(queue);
                free(steps);
                free(visited);
                return output_count;
            }

            out_positions[output_count].x = next_x;
            out_positions[output_count].y = next_y;
            output_count++;
        }
    }

    free(queue);
    free(steps);
    free(visited);

    return output_count;
}

