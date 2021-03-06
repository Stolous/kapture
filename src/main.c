#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "settings.h"
#include "world.h"
#include "events.h"
#include "game.h"

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

	if(TTF_Init() == -1)
	{
		fprintf(stderr, "Failed to initialize SDL2_TTF\n%s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}	
	// Creating the GUI
	TTF_Font* font = TTF_OpenFont("resources/font.ttf", 24);

	// Initializing game
	GameInfo gameInfo;
	gameInfo.turn = 1;
	gameInfo.winner = -1;

	// Initializing world
	WorldResources worldResources;
	worldResources.selectedPawn = NULL;
	setupWorld(renderer, &worldResources);
	int flagNum = 0;
	for(int i = 0; i < worldResources.pawnsCount; ++i)
	{
		if(worldResources.pawns[i].type == 0)
		{
			worldResources.flags[flagNum] = &worldResources.pawns[i];
			++flagNum;
		}
	}


	// Main game loop
	int shouldClose = 0;
	SDL_Event event;
	while(!shouldClose)
	{
		// Sending all events to event handler
		while(SDL_PollEvent(&event))
		{
			shouldClose = handleEvent(event, &gameInfo, &worldResources);
		}

		SDL_UpdateWindowSurface(window);
		
		SDL_RenderClear(renderer);
		
		// Render world and HUD
		renderWorld(renderer, &gameInfo, &worldResources);
		drawHUD(renderer, font, &gameInfo);
		
		SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);

		SDL_RenderPresent(renderer);

		if(gameInfo.winner != -1)
			shouldClose = 1;


		SDL_Delay(16);
	}

	// Showing a messagebox to tell which player won the game
	if(gameInfo.winner != -1)
	{
		char text[] = "Winner : ";
		sprintf(text, "%s %s", text, gameInfo.winner ? "red team" : "blue team");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game finished", text, window);
	}

	// Closing window and quitting SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	TTF_CloseFont(font);
	TTF_Quit();
	
	return 0;
}
