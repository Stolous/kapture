#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>
#include "game.h"

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
	char hasPlayed;
}
Pawn;


typedef struct WorldResources
{
	SDL_Texture* terrainTexture;
	SDL_Texture* entitiesTexture;
	// Map, containing only tiles
	char** map;
	char** fog;
	// Pawns array
	int pawnsCount;
	Pawn* pawns;
	Pawn* selectedPawn;
	Pawn* flagWearers[2];
}
WorldResources;

void readMapFile(WorldResources* res);
void createTextures(SDL_Renderer* renderer, SDL_Texture** terrainTexture, SDL_Texture** entitiesTexture);
void initFog(WorldResources* res);
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
void killPawn(WorldResources* resources, Pawn* pawn);
void handleFight(WorldResources* resources, Pawn* pawn);
int getMovPoints(Pawn* pawn, char tileType);
int movePawn(GameInfo* gi, WorldResources* resources, Pawn* pawn, Vector2 destination);
void renderWorld(SDL_Renderer* renderer, GameInfo* gi, WorldResources* res);

#endif
