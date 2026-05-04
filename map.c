#include "utils.h"
#include "map.h"

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
    
    //LAYER 1
    for (int x = 0; x < MAP_ROWS; x++)
    {
        for (int y = 0; y < MAP_COLS; y++)
        {
            SDL_FRect dst_rect = {TEXTURE_WIDTH * y, TEXTURE_HEIGHT * x, TEXTURE_WIDTH, TEXTURE_HEIGHT};

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
            SDL_FRect dst_rect = {TEXTURE_WIDTH * y, TEXTURE_HEIGHT * x, TEXTURE_WIDTH, TEXTURE_HEIGHT};

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
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint1Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint1Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint1Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint1Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint1Progress > 0) app->capPoint1Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint1Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint1Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint1Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint1Progress > 0) app->capPoint1Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint1Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint1Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint1Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint1Progress > 0) app->capPoint1Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 2
    pointX = 5;
    pointY = 1;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint2Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint2Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint2Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint2Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint2Progress > 0) app->capPoint2Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint2Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint2Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint2Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint2Progress > 0) app->capPoint2Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint2Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint2Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint2Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint2Progress > 0) app->capPoint2Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 3
    pointX = 10;
    pointY = 10;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint3Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint3Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint3Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint3Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint3Progress > 0) app->capPoint3Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint3Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint3Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint3Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint3Progress > 0) app->capPoint3Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint1Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint1Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint1Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint1Progress > 0) app->capPoint1Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 4
    pointX = 13;
    pointY = 3;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint4Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint4Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint4Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint4Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint4Progress > 0) app->capPoint4Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint4Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint4Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint4Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint4Progress > 0) app->capPoint4Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint4Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint4Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint4Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint4Progress > 0) app->capPoint4Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 5
    pointX = 18;
    pointY = 14;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint5Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint5Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint5Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint5Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint5Progress > 0) app->capPoint5Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint5Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint5Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint5Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint5Progress > 0) app->capPoint5Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint5Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint5Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint5Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint5Progress > 0) app->capPoint5Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 6
    pointX = 21;
    pointY = 7;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint6Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint6Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint6Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint6Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint6Progress > 0) app->capPoint6Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint6Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint6Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint6Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint6Progress > 0) app->capPoint6Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint6Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint6Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint6Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint6Progress > 0) app->capPoint6Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 7
    pointX = 26;
    pointY = 16;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint7Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint7Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint7Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint7Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint7Progress > 0) app->capPoint7Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint7Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint7Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint7Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint7Progress > 0) app->capPoint7Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint7Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint7Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint7Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint7Progress > 0) app->capPoint7Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

    //CAPTURE POINT 8
    pointX = 30;
    pointY = 1;
    switch (map[pointY][pointX])
    {
        case '1':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint8Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[pointY][pointX] = '2';
                        app->capPoint8Progress = 0;
                    }
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint8Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '1')
                {
                    app->capPoint8Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint8Progress > 0) app->capPoint8Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '2':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint8Progress == 2)
                {
                    if (app->units[i].team == 1)
                    {
                        map[pointY][pointX] = '3';
                        app->capPoint8Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '2' && app->units[i].team == 1)
                {
                    app->capPoint8Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint8Progress > 0) app->capPoint8Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
        
        case '3':
            for (int i = 0; i < app->unitCount; i++)
            {
                // point capture
                if ((app->units[i].x == pointX && app->units[i].y == pointY) && app->capPoint8Progress == 2)
                {
                    if (app->units[i].team == 0)
                    {
                        map[16][1] = '2';
                        app->capPoint8Progress = 0;
                    }
                }
                // capture progress
                if (app->units[i].x == pointX && app->units[i].y == pointY && map[pointY][pointX] == '3' && app->units[i].team == 0)
                {
                    app->capPoint8Progress++;
                    app->isASingleUnitStandingInPoint = true;
                }
            }
            if (app->isASingleUnitStandingInPoint == false && app->capPoint8Progress > 0) app->capPoint8Progress = 0;
            else app->isASingleUnitStandingInPoint = false;
            break;
    }

}