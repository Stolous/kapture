#include <SDL2/SDL.h>

#include "settings.h"
#include "game.h"
#include "world.h"

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

		pawns[i].hasPlayed = 0;
		
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

void initFog(WorldResources* res)
{
	char** fogArray;
	if((fogArray = malloc(MAP_HEIGTH * sizeof(char*))) == NULL)
	{
		fprintf(stderr, "Failed to allocate fog in memory\n");
		SDL_Quit();
		exit(-1);
	}
	for(int i = 0; i<MAP_HEIGTH; ++i)
	{
		if((fogArray[i] = calloc((MAP_WIDTH+1), sizeof(char))) == NULL)
		{
			fprintf(stderr, "Failed to allocate fog in memory\n");
			SDL_Quit();
			exit(-1);
		}
	}

	for(int i = 0; i < MAP_WIDTH; ++i)
	{
		for(int j = 0; j < MAP_HEIGTH; ++j)
		{
			fogArray[j][i] = (i < 7 && j > 12) ? 1 : (i > 24 && j < 7) ? 2 : 0;
		}
	}
	res->fog = fogArray;
}

void setupWorld(SDL_Renderer* renderer, WorldResources* res)
{
	// Getting textures
	SDL_Texture *terrainTexture, *entitiesTexture;
	createTextures(renderer, &terrainTexture, &entitiesTexture);
	
	// Packing up world resources
	//WorldResources res;
	readMapFile(res);
	initFog(res);
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
		if(res->pawns[i].type != 0 &&
			res->pawns[i].position.x == position.x &&
			res->pawns[i].position.y == position.y)
		{
			return &res->pawns[i];
		}
	}
	return NULL;
}

void killPawn(WorldResources* res, Pawn* pawn)
{
	if(res->flagWearers[abs(pawn->team-1)] != NULL &&
		res->flagWearers[abs(pawn->team-1)] == pawn)
	{
		res->flagWearers[abs(pawn->team-1)] = NULL;
	}
	if(pawn->team)
		pawn->position = (Vector2){29, 2};
	else
		pawn->position = (Vector2){2, 17};
}

void handleFight(GameInfo* gi, WorldResources* res, Pawn* pawn)
{
	Pawn* enemy = NULL;
	// Find nearby enemy
	for(int i = 0; i < res->pawnsCount; ++i)
	{
		char posX = res->pawns[i].position.x, posY = res->pawns[i].position.y;
		if(((posY == pawn->position.y && (posX - 1 == pawn->position.x || posX + 1 == pawn->position.x)) ||
			(posX == pawn->position.x && (posY - 1 == pawn->position.y || posY + 1 == pawn->position.y))) &&
			(res->pawns[i].team != pawn->team || res->pawns[i].type == 0))
			enemy = &res->pawns[i];
	}

	srand(time(0));
	char randBool = rand() % 2;

	// Choosing which enemy to kill
	if(enemy == NULL)
		return; // No one to fight
	if(enemy->type == 0)
	{
		if(enemy->team == pawn->team &&
			res->flagWearers[abs(pawn->team-1)] != NULL &&
			res->flagWearers[abs(enemy->team-1)]->position.x == pawn->position.x && 
			res->flagWearers[abs(enemy->team-1)]->position.y == pawn->position.y)
			gi->winner = pawn->team;
		else if(enemy->team != pawn->team)
			res->flagWearers[abs(pawn->team-1)] = pawn;
	}
	else if(enemy->type == 1)
	{
		if(pawn->type == 1)
			return;
		if(pawn->type > 1)
			killPawn(res, enemy);
	}
	else if(enemy->type == 2)
	{
		if(pawn->type == 1)
			killPawn(res, pawn);
		if(pawn->type == 2)
			killPawn(res, randBool ? enemy : pawn);
		if(pawn->type > 2)
			killPawn(res, enemy);
	}
	else if(enemy->type == 3)
	{
		if(pawn->type < 3)
			killPawn(res, pawn);
		if(pawn->type == 3)
			killPawn(res, randBool ? enemy : pawn);
	}
}

int getMovPoints(Pawn* pawn, char tileType)
{
	char type = pawn->type;
	char points = type == 0 ? 0 : type == 1 ? 5 : type == 2 ? 3 : 2;
	// changing points according to terrain limitations
	switch(tileType/*res->map[res->selectedPawn->position.y][res->selectedPawn->position.x]*/)
	{
		case '1':
			points /= 2;
			break;
		case '2':
			points = 1;
			break;
	}
	
	return points;
}
int movePawn(GameInfo* gi, WorldResources* res, Pawn* pawn, Vector2 dest)
{
	if(getPawnAt(res, dest) != NULL)
		return 1; // There is already a pawn at this place
	if(pawn == NULL)
		return 2; // No one to move
	
	int points = getMovPoints(pawn, res->map[pawn->position.y][pawn->position.x]);
	if(	dest.x < pawn->position.x - points || 
		dest.x > pawn->position.x + points ||
		dest.y < pawn->position.y - points ||
		dest.y > pawn->position.y + points)
		return 3; // Too far
	
	if(dest.x < 0 || dest.x > (MAP_WIDTH - 1) || dest.y < 0 || dest.y > (MAP_HEIGTH - 1))
		return 4; // Out of map boundaries
	
	pawn->position = dest;
	char bitSet = (gi->turn%2 ? 0x1 : 0x2);
	res->fog[dest.y][dest.x] |= bitSet;
	if(dest.y > 0)
		res->fog[dest.y - 1][dest.x] |= bitSet;
	if(dest.y < (MAP_HEIGTH - 1))
		res->fog[dest.y + 1][dest.x] |= bitSet;
	if(dest.x > 0)
		res->fog[dest.y][dest.x - 1] |= bitSet;
	if(dest.x < (MAP_WIDTH))
		res->fog[dest.y][dest.x + 1] |= bitSet;

	pawn->hasPlayed = 1;
	res->selectedPawn = NULL;
	handleFight(gi, res, pawn);

	// Move the flag to the flag wearer
	if(res->flagWearers[abs(pawn->team-1)] != NULL &&
		pawn->position.x == res->flagWearers[abs(pawn->team-1)]->position.x &&
		pawn->position.y == res->flagWearers[abs(pawn->team-1)]->position.y)
		res->flags[abs(pawn->team-1)]->position = pawn->position;
}

void renderWorld(SDL_Renderer* renderer, GameInfo* gi, WorldResources* res)
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

			// rendering if we can see it
			if(((res->fog[j][i] >> 0x1) && !(gi->turn % 2)) ||
				(res->fog[j][i] & 0x1) && (gi->turn % 2))
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
		// rendering if we can see it
		if(((res->fog[res->pawns[i].position.y][res->pawns[i].position.x] >> 0x1) && !(gi->turn % 2)) ||
			(res->fog[res->pawns[i].position.y][res->pawns[i].position.x] & 0x1) && (gi->turn % 2))
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
		int points = getMovPoints(res->selectedPawn, res->map[res->selectedPawn->position.y][res->selectedPawn->position.x]);
		/*char type = res->selectedPawn->type;
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
		}*/
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
		if(res->selectedPawn->type)
		{
			// Drawing movement box
			SDL_SetRenderDrawColor(renderer, 255, 30, 30, 255);
			SDL_RenderDrawLines(renderer, square, 5);
		}
	}

}
