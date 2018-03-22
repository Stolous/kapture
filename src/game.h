#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL_ttf.h>

typedef struct GameInfo
{
    int turn;
    char currentPlayer;
}
GameInfo;

void drawHUD(SDL_Renderer* renderer, TTF_Font* font, GameInfo* gi);

void endOfTurn(GameInfo* gi);

#endif
