#include "utils.h"
#include "map.h"

//DEBUG
static void drawText(
    SDL_Renderer* renderer,
    float x,
    float y,
    float scale,
    Uint8 r,
    Uint8 g,
    Uint8 b,
    Uint8 a,
    const char* text
)
{
    float oldX, oldY;
    Uint8 oldR, oldG, oldB, oldA;

    SDL_GetRenderScale(renderer, &oldX, &oldY);
    SDL_GetRenderDrawColor(renderer, &oldR, &oldG, &oldB, &oldA);

    SDL_SetRenderScale(renderer, scale, scale);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_RenderDebugTextFormat(
        renderer,
        x / scale,
        y / scale,
        "%s",
        text
    );

    // Restore previous state
    SDL_SetRenderScale(renderer, oldX, oldY);
    SDL_SetRenderDrawColor(renderer, oldR, oldG, oldB, oldA);
}
//DEBUG


char map[MAP_ROWS][MAP_COLS];
/*
MAP TILES:
    Z = Normal Ground Tile
    W = Wall
    R = River
    1 = Capture Point Neutral
    2 = Capture Point Left Team
    3 = Capture Point Right team
    G = Gate Intact
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

void renderMap(AppState *app)
{
    
    float centerX = WINDOW_WIDTH / 2.0f;
    float centerY = WINDOW_HEIGHT / 2.0f;

    float scaledW = TEXTURE_WIDTH * app->zoom;
    float scaledH = TEXTURE_HEIGHT * app->zoom;


    //LAYER 1
    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            float screenX = centerX + ((y * TEXTURE_WIDTH) - app->cameraX) * app->zoom;
            float screenY = centerY + ((x * TEXTURE_HEIGHT) - app->cameraY) * app->zoom;

            SDL_FRect dst_rect = { screenX, screenY, scaledW, scaledH };

            if (map[x][y] == 'Z')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);

            if (map[x][y] == 'W')
            SDL_RenderTexture(app->renderer, app->tileWall, NULL, &dst_rect);

            if (map[x][y] == 'R')
            SDL_RenderTexture(app->renderer, app->tileRiver, NULL, &dst_rect);

            if (map[x][y] == '1' && map[x][y] == '2' && map[x][y] == '3')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);

            if (map[x][y] == 'G')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);

            if (map[x][y] == 'T')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);

            if (map[x][y] == 'H')
            SDL_RenderTexture(app->renderer, app->tileHigh, NULL, &dst_rect);

            if (map[x][y] == 'B')
            SDL_RenderTexture(app->renderer, app->tileGround, NULL, &dst_rect);
        }
    }

    //LAYER 2
    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            float screenX = centerX + ((y * TEXTURE_WIDTH) - app->cameraX) * app->zoom;
            float screenY = centerY + ((x * TEXTURE_HEIGHT) - app->cameraY) * app->zoom;

            SDL_FRect dst_rect = { screenX, screenY, scaledW, scaledH };

            if (map[x][y] == '1')
            SDL_RenderTexture(app->renderer, app->tileCapture, NULL, &dst_rect);
            if (map[x][y] == '2')
            SDL_RenderTexture(app->renderer, app->tileCaptureBlue, NULL, &dst_rect);
            if (map[x][y] == '3')
            SDL_RenderTexture(app->renderer, app->tileCaptureRed, NULL, &dst_rect);

            if (map[x][y] == 'G')
            SDL_RenderTexture(app->renderer, app->tileGate, NULL, &dst_rect);

            if (map[x][y] == 'T')
            SDL_RenderTexture(app->renderer, app->tileTower, NULL, &dst_rect);

            if (map[x][y] == 'B')
            SDL_RenderTexture(app->renderer, app->tileBush, NULL, &dst_rect);
        }
    }
}

// Defines where capture points are in the map and activates the capturing mechanics for them.
void capturePointMechanics(AppState *app)
{
    // Capture points are counted from left to right.
    // For example, the first capture point is the left player's base capture point, while the eight (and last) point is the right player's base capture point.

    int pointX = 0;
    int pointY = 0;

    //CAPTURE POINT 1
    pointX = 1;
    pointY = 16;
    captureProgress(pointY, pointX, app, 1);

    //CAPTURE POINT 2
    pointX = 5;
    pointY = 1;
    captureProgress(pointY, pointX, app, 2);

    //CAPTURE POINT 3
    pointX = 10;
    pointY = 10;
    captureProgress(pointY, pointX, app, 3);

    //CAPTURE POINT 4
    pointX = 13;
    pointY = 3;
    captureProgress(pointY, pointX, app, 4);

    //CAPTURE POINT 5
    pointX = 18;
    pointY = 14;
    captureProgress(pointY, pointX, app, 5);

    //CAPTURE POINT 6
    pointX = 21;
    pointY = 7;
    captureProgress(pointY, pointX, app, 6);

    //CAPTURE POINT 7
    pointX = 26;
    pointY = 16;
    captureProgress(pointY, pointX, app, 7);

    //CAPTURE POINT 8
    pointX = 30;
    pointY = 1;
    captureProgress(pointY, pointX, app, 8);

}

void captureProgress (int pointY, int pointX, AppState *app, int capPointID)
{
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPointProgress[capPointID] == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPointProgress[capPointID] = 0;
                        app->team0SkillPoints++;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPointProgress[capPointID] = 0;
                        app->team1SkillPoints++;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPointProgress[capPointID]++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPointProgress[capPointID] > 0) app->capPointProgress[capPointID] = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPointProgress[capPointID] == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPointProgress[capPointID] = 0;
                        app->team1SkillPoints++;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPointProgress[capPointID]++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPointProgress[capPointID] > 0) app->capPointProgress[capPointID] = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPointProgress[capPointID] == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPointProgress[capPointID] = 0;
                        app->team0SkillPoints++;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPointProgress[capPointID]++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPointProgress[capPointID] > 0) app->capPointProgress[capPointID] = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }
}