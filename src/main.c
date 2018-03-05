#include <stdio.h>

#include <SDL2/SDL.h>

#include "settings.h"
#include "world.h"

int main(int argc, char** argv)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Failed to initialize SDL2\n%s\n", SDL_GetError());
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Kapture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);
	if(!window)
	{
		fprintf(stderr, "Failed to create the window\n%s\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_UpdateWindowSurface(window);
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	int shouldClose = 0;
	SDL_Event event;
	while(!shouldClose)
	{
		if(!SDL_PollEvent(&event))
			continue;
		switch(event.type)
		{
			case SDL_QUIT:
				shouldClose = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("click");
				break;
		}
		
		SDL_UpdateWindowSurface(window);
		
		renderWorld(renderer);

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyWindow(window);

	return 0;
}
