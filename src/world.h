#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>

typedef struct Vector2
{
	char x;
	char y;
}
Vector2;

typedef struct Pawn
{
	Vector2 position;

	char type;
	char team;
}
Pawn;


typedef struct WorldResources
{
	SDL_Texture* terrainTexture;
	SDL_Texture* entitiesTexture;
	// Map, containing only tiles
	char** map;
	// Pawns array
	int pawnsCount;
	Pawn* pawns;
	Pawn* selectedPawn;
}
WorldResources;

void readMapFile(WorldResources* res);
void createTextures(SDL_Renderer* renderer, SDL_Texture** terrainTexture, SDL_Texture** entitiesTexture);
void setupWorld(SDL_Renderer* renderer, WorldResources* res);

typedef struct Tile
{
	char biome;
	char entity; 
	char visibility;
}
Tile;

void getTileInfo(char** map, int x, int y, Tile* tile);
Pawn* getPawnAt(WorldResources* resources, Vector2 position);

void renderWorld(SDL_Renderer* renderer, WorldResources* res);

#endif
