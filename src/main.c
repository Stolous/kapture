#include <stdio.h>

#include <SDL2/SDL.h>

#include "settings.h"
#include "world.h"

int main(int argc, char** argv)
{
	// Initializing world
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Failed to initialize SDL2\n%s\n", SDL_GetError());
		return -1;
	}

	// Creating the window
	SDL_Window* window = SDL_CreateWindow("Kapture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);
	if(!window)
	{
		fprintf(stderr, "Failed to create the window\n%s\n", SDL_GetError());
		return -1;
	}

	// Creating renderer
	// TODO use hardware acceleration	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_UpdateWindowSurface(window);
	SDL_Surface* surface = SDL_GetWindowSurface(window);

	// Initialize world
	WorldResources worldResources;
	setupWorld(renderer, &worldResources);

	// Main game loop
	int shouldClose = 0;
	SDL_Event event;
	while(!shouldClose)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				//printf("%s", event.text);
				case SDL_QUIT:
					shouldClose = 1;
					break;
				case SDL_MOUSEBUTTONDOWN:
					printf("click\n");
					break;
			}
		}

		SDL_UpdateWindowSurface(window);
		
		renderWorld(renderer, &worldResources);

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	SDL_DestroyWindow(window);
	
	return 0;
}
