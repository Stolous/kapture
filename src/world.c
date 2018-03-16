#include <SDL2/SDL.h>

#include "world.h"
#include "settings.h"

char** readMapFile()
{
	FILE *mapFile = fopen("resources/map", "rt");
	if(!mapFile)
	{
		fprintf(stderr, "Failed to load map\n");
		SDL_Quit();
		exit(-1);
	}

	char** mapArray;
	if((mapArray = malloc(MAP_HEIGTH*sizeof(char*))) == NULL)
	{
		fprintf(stderr, "Failed to allocate map in memory\n");
		SDL_Quit();
		exit(-1);
	}
	for(int i = 0; i<MAP_HEIGTH; i++)
	{
		char line[100];
		fgets(line, sizeof(line), mapFile);
		line[MAP_WIDTH] = '\0';

		if((mapArray[i] = malloc(MAP_WIDTH + 1)) == NULL)
		{
			fprintf(stderr, "Failed to allocate map in memory\n");
			SDL_Quit();
			exit(-1);
		}
		strcpy(mapArray[i], line);
	}

	printf("map:\n");	
	for(int i = 0; i < MAP_HEIGTH; i++)
		printf("%s\n", mapArray[i]);

	return mapArray;
}

void createTextures(SDL_Renderer* renderer, SDL_Texture** mapTexture)
{
	// Load tileset
	SDL_Surface* tileset;
	tileset = SDL_LoadBMP("resources/map_tiles.bmp");
	if (!tileset)
	{
		fprintf(stderr, "Failed to load map tileset\n");
		SDL_Quit();
		exit(-1);
	}

	// Create texures from tileset
	*mapTexture = SDL_CreateTextureFromSurface(renderer, tileset);
}

void setupWorld(SDL_Renderer* renderer, WorldResources* res)
{
	// Getting textures
	SDL_Texture* mapTexture;
	createTextures(renderer, &mapTexture);
	
	// Packing up world resources
	//WorldResources res;
	res->map = readMapFile();
	res->mapTexture = mapTexture;
}

void renderWorld(SDL_Renderer* renderer, WorldResources* res)
{
	//printf("%d\n", (int)(res->map[1][1]-'0'));
	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	
	for(int i = 0; i<MAP_WIDTH; i++)
	{
		for(int j = 0; j<MAP_HEIGTH; j++)
		{ 
			// Source according to map
			SDL_Rect srcRect = {(int)(res->map[j][i]-'0') * 32, 0, 32, 32};
			// Tile location on screen
			SDL_Rect destRect = {MAP_LEFT + 32*i, MAP_TOP + 32*j, 32, 32};
			SDL_RenderCopy(renderer, res->mapTexture, &srcRect, &destRect);
		}
	}	
	
	
	for(int i = 0;i < WIDTH; i += 32)
		SDL_RenderDrawLine(renderer, i, 0, i, HEIGTH);
	for(int i = 0;i < HEIGTH; i += 32)
		SDL_RenderDrawLine(renderer, 0, i, WIDTH, i);
	
	//SDL_Surface* line = SDL_CreateRGBSurface(0, WIDTH, 0, 50, 0, 0, 0, 0);
	//SDL_FillRect(line, NULL, SDL_MapRGB(line->format, 255, 0, 0));
	

}
