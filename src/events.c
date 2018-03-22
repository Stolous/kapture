#include <SDL2/SDL.h>

#include "settings.h"
#include "world.h"
#include "game.h"
#include "events.h"

int handleEvent(SDL_Event event, GameInfo* gi, WorldResources* res)
{
	// For mouse coordinates
	int32_t x, y;
	switch(event.type)
	{
		//printf("%s", event.text);
		case SDL_QUIT:
			return 1;
			break;
		case SDL_MOUSEBUTTONDOWN:
			// Get cell under mouse pointer
			SDL_GetMouseState(&x, &y);
			Vector2 cell = {((x - MAP_TOP) / TILE_SIZE), (y - MAP_LEFT) / TILE_SIZE};
			printf("click: %d, %d\n", cell.x, cell.y);
			// Move selected pawn
			movePawn(gi, res, res->selectedPawn, cell);

			// Select pawn
			Pawn* selection = getPawnAt(res, cell);
			if(selection == NULL)
				res->selectedPawn = NULL;
			else if(!selection->hasPlayed && selection->team != gi->turn % 2)
				res->selectedPawn = selection;

			break;
		case SDL_KEYDOWN:
			// Ends the turn
			if(event.key.keysym.sym == SDLK_RETURN && !event.key.repeat)
			{
				for(int i = 0; i < res->pawnsCount; ++i)
					res->pawns[i].hasPlayed = 0;
				endOfTurn(gi);
			}
			break;
		default:
			//printf("uncaught event: %d\n", event.type);
			break;
	}
	return 0;
}
