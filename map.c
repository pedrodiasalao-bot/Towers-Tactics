#include "header.h"

char map[MAP_ROWS][MAP_COLS];
/*
MAP TILES:
    Z = Normal Ground Tile
    W = Wall
    R = River
    C = Capture Point
    G = Gate
    T = Tower
    H = High Ground
    B = Bush
*/

void loadMap(const char *filename)
{
    // char extension[] = ".txt";
    // char levelTens[3];
    // sprintf(levelTens, "%d", levelSelect);
    // char filename[32];
    // sprintf(filename, "Maps/map%s%s", levelTens, extension);

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Failed to open map file");
        return;
    }

    printf("loading map %s", filename);
    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            int ch = fgetc(file);
            if (ch == EOF)
            {
                fclose(file);
                return;
            }
            if (ch == '\n')
            {
                col--;
                continue;
            }
            map[row][col] = (char)ch;
        }
    }
    fclose(file);
}

void renderMap()
{
    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            SDL_FRect dst_rect = {TEXTURE_WIDTH * y, TEXTURE_HEIGHT + (TEXTURE_HEIGHT * x), TEXTURE_WIDTH, TEXTURE_HEIGHT};

            if (map[x][y] == 'Z')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);

            if (map[x][y] == 'W')
            SDL_RenderTexture(app->renderer, app->tileWall, NULL, &dst_rect);

            if (map[x][y] == 'R')
            SDL_RenderTexture(app->renderer, app->tileRiver, NULL, &dst_rect);

            if (map[x][y] == 'C')
            SDL_RenderTexture(app->renderer, app->tileCapture, NULL, &dst_rect);

            if (map[x][y] == 'G')
            SDL_RenderTexture(app->renderer, app->tileGate, NULL, &dst_rect);

            if (map[x][y] == 'T')
            SDL_RenderTexture(app->renderer, app->tileTower, NULL, &dst_rect);

            if (map[x][y] == 'H')
            SDL_RenderTexture(app->renderer, app->tileHigh, NULL, &dst_rect);

            if (map[x][y] == 'B')
            SDL_RenderTexture(app->renderer, app->tileBush, NULL, &dst_rect);
        }
    }
}
