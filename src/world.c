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

void createTextures(SDL_Renderer* renderer, SDL_Texture** terrainTexture, SDL_Texture** entitiesTexture)
{
	// Load tileset
	SDL_Surface *tTileset, *eTileset;
	tTileset = SDL_LoadBMP("resources/map_tiles.bmp");
	eTileset = SDL_LoadBMP("resources/entities_tiles.bmp");
	
	if (!tTileset || !eTileset)
	{
		fprintf(stderr, "Failed to load map tilesets\n");
		SDL_Quit();
		exit(-1);
	}

	// Create texures from tileset
	*terrainTexture = SDL_CreateTextureFromSurface(renderer, tTileset);
	*entitiesTexture = SDL_CreateTextureFromSurface(renderer, eTileset);
}

void setupWorld(SDL_Renderer* renderer, WorldResources* res)
{
	// Getting textures
	SDL_Texture *terrainTexture, *entitiesTexture;
	createTextures(renderer, &terrainTexture, &entitiesTexture);
	
	// Packing up world resources
	//WorldResources res;
	readMapFile(res);
	res->terrainTexture = terrainTexture;
	res->entitiesTexture = entitiesTexture;
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

Pawn* getPawnAt(WorldResources* res, Vector2 position)
{
	for(int i = 0; i < res->pawnsCount; ++i)
	{
		if(res->pawns[i].position.x == position.x && res->pawns[i].position.y == position.y)
		{
			return &res->pawns[i];
		}
	}
	return NULL;
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
			SDL_RenderCopy(renderer, res->terrainTexture, &srcRect, &destRect);
		}
	}	
	
	for(int i = 0; i < res->pawnsCount; ++i)
	{
		/*printf("%d", res->entitiesTexture);
		SDL_Texture* texture = res->entitiesTexture;
		if(res->pawns[i].team)
			SDL_SetSurfaceColorMod(texture, TEAM_1_COLOR);
		else
			SDL_SetSurfaceColorMod(texture, 0,0,255);
		*/
		// Souce according to pawn array
		SDL_Rect srcRect = {(int)res->pawns[i].type * TILE_SIZE, (int)res->pawns[i].team * TILE_SIZE, TILE_SIZE, TILE_SIZE};
		// Pawn location on screen
		SDL_Rect destRect = {MAP_LEFT + TILE_SIZE * res->pawns[i].position.x, MAP_TOP + TILE_SIZE * res->pawns[i].position.y, TILE_SIZE, TILE_SIZE};
		SDL_RenderCopy(renderer, res->entitiesTexture, &srcRect, &destRect);
		
	}

	SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
	// Display the grid
	for(int i = 0;i < WIDTH; i += TILE_SIZE)
		SDL_RenderDrawLine(renderer, i, 0, i, HEIGTH);
	for(int i = 0;i < HEIGTH; i += TILE_SIZE)
		SDL_RenderDrawLine(renderer, 0, i, WIDTH, i);
	
	//SDL_Surface* line = SDL_CreateRGBSurface(0, WIDTH, 0, 50, 0, 0, 0, 0);
	//SDL_FillRect(line, NULL, SDL_MapRGB(line->format, 255, 0, 0));
	
	// Draw the movement square
	if(res->selectedPawn != NULL)
	{
		char type = res->selectedPawn->type;
		char points = type == 0 ? 0 : type == 1 ? 5 : type == 2 ? 3 : 2;
		// changing points according to terrain limitations
		switch(res->map[res->selectedPawn->position.y][res->selectedPawn->position.x])
		{
			case '1':
				points /= 2;
				break;
			case '2':
				points = 1;
				break;
		}
		// Getting positions on screen
		int posX = res->selectedPawn->position.x * TILE_SIZE + MAP_LEFT;
		int posY = res->selectedPawn->position.y * TILE_SIZE + MAP_TOP;
		// Points in the square
		SDL_Point square[5] = {
			{posX - (TILE_SIZE * points), posY - (TILE_SIZE * points)},
			{posX - (TILE_SIZE * points), posY + (TILE_SIZE * (points + 1))},
			{posX + (TILE_SIZE * (points + 1)), posY + (TILE_SIZE * (points + 1))},
			{posX + (TILE_SIZE * (points + 1)), posY - (TILE_SIZE * points)},
			{posX - (TILE_SIZE * points), posY - (TILE_SIZE * points)}
		};
		// Skipping if this is a flag
		if(type)
		{
			SDL_SetRenderDrawColor(renderer, 255, 30, 30, 255);
			SDL_RenderDrawLines(renderer, square, 5);
		}
	}

}
