#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>


typedef struct WorldResources
{
	SDL_Texture* mapTexture;
	// The map. Must be last in struct because it has flexible length.
	char** map;
}
WorldResources;

char** readMapFile();
void createTextures(SDL_Renderer* renderer, SDL_Texture** tex_Plains);
void setupWorld(SDL_Renderer* renderer, WorldResources* res);


void renderWorld(SDL_Renderer* renderer, WorldResources* res);

typedef struct Tile
{
	char type;
	char visibility;
}
Tile;

#endif
