#include <SDL2/SDL.h>

#include "settings.h"

void renderWorld(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	
	for(int i = 0;i < WIDTH; i += 30)
		SDL_RenderDrawLine(renderer, i, 0, i, HEIGTH);
	for(int i = 0;i < HEIGTH; i += 30)
		SDL_RenderDrawLine(renderer, 0, i, WIDTH, i);
	
	//SDL_Surface* line = SDL_CreateRGBSurface(0, WIDTH, 0, 50, 0, 0, 0, 0);
	//SDL_FillRect(line, NULL, SDL_MapRGB(line->format, 255, 0, 0));
	

}
