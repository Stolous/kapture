#include <SDL2/SDL.h>

#include "world.h"
#include "settings.h"

void readMapFile(WorldResources* res)
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
	printf("map:\n");	
	for(int i = 0; i<MAP_HEIGTH; ++i)
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
		printf("%s\n", mapArray[i]);
	}
	
	// Getting the number of pawns
	//int pawnCount;
	//fscanf(mapFile, "%d", &pawnCount);
	
	printf("pawns:\n");	
	char type, team, posX, posY;
	Pawn* pawns = malloc(sizeof(Pawn));
	int i = 0;
	while(fscanf(mapFile, "%d %d %d %d", &type, &team, &posX, &posY) != EOF)
	{
		pawns[i].type = type;
		pawns[i].team = team;
		pawns[i].position.x = posX;
		pawns[i].position.y = posY;
		
		printf("%d from team %d at %d, %d\n", type, team, posX, posY);
		++i;
		pawns = realloc(pawns, (i+1)*sizeof(Pawn));
	}

	for(int j = 0; j < i; ++j)
	{
		printf("%d from team %d at %d, %d\n", pawns[j].type, pawns[j].team, pawns[j].position.x, pawns[j].position.y);
	}
	
	res->map = mapArray;
	res->pawnsCount = i;
	res->pawns = pawns;
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
	readMapFile(res);
	res->mapTexture = mapTexture;
}


void getTileInfo(char** map, int x, int y, Tile* tile)
{
	char tileByte = map[y][x] - '0';
	// Biome
	tile->biome = (tileByte & 0x7);
	// Entity
	tile->entity= (tileByte >> 0x3) & 0x7;
	// Visibility
	tile->visibility = (tileByte >> 0x6) & 0x7;
}

void renderWorld(SDL_Renderer* renderer, WorldResources* res)
{
	// Display map tiles 
	for(int i = 0; i < MAP_WIDTH; ++i)
	{
		for(int j = 0; j < MAP_HEIGTH; ++j)
		{
			// Get tile informations
			Tile tile;
			getTileInfo(res->map, i, j, &tile);
			// Source according to map
			SDL_Rect srcRect = {(int)(tile.biome) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
			// Tile location on screen
			SDL_Rect destRect = {MAP_LEFT + TILE_SIZE*i, MAP_TOP + TILE_SIZE*j, TILE_SIZE, TILE_SIZE};
			SDL_RenderCopy(renderer, res->mapTexture, &srcRect, &destRect);
		}
	}	
	
	for(int i = 0; i < res->pawnsCount; ++i)
	{
		// Souce according to pawn array
		SDL_Rect srcRect = {(int)(/*res->pawns[i].type*/3) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
		// Pawn location on screen
		SDL_Rect destRect = {MAP_LEFT + TILE_SIZE * res->pawns[i].position.x, MAP_TOP + TILE_SIZE * res->pawns[i].position.y, TILE_SIZE, TILE_SIZE};
		SDL_RenderCopy(renderer, res->mapTexture, &srcRect, &destRect);
		
	}

	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	// Display the grid
	for(int i = 0;i < WIDTH; i += TILE_SIZE)
		SDL_RenderDrawLine(renderer, i, 0, i, HEIGTH);
	for(int i = 0;i < HEIGTH; i += TILE_SIZE)
		SDL_RenderDrawLine(renderer, 0, i, WIDTH, i);
	
	//SDL_Surface* line = SDL_CreateRGBSurface(0, WIDTH, 0, 50, 0, 0, 0, 0);
	//SDL_FillRect(line, NULL, SDL_MapRGB(line->format, 255, 0, 0));
	

}
