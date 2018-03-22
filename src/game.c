#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "world.h"
#include "game.h"

void drawHUD(SDL_Renderer* renderer, TTF_Font* font, GameInfo* gi)
{
	// Dynamic turn display
	char text[] = "Turn";
	sprintf(text, "%s %d", text, (gi->turn+1)/2);
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, (SDL_Color){255, 255, 255});
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	const SDL_Rect textRect = {100, 720, 200, 50};
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_FreeSurface(textSurface);
	// Static help text
	char staticText[] = "Press enter to finish your turn";
	SDL_Surface* staticTextSurface = TTF_RenderText_Solid(font, staticText, (SDL_Color){255, 255, 255});
	SDL_Texture* staticTextTexture = SDL_CreateTextureFromSurface(renderer, staticTextSurface);
	const SDL_Rect staticTextRect = {800, 740, 200, 20};
	SDL_RenderCopy(renderer, staticTextTexture, NULL, &staticTextRect);
	SDL_FreeSurface(staticTextSurface);
}

void endOfTurn(GameInfo* gi)
{
	++gi->turn;
}

void endOfGame(char winner)
{
}