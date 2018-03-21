#include <SDL2/SDL.h>

#include "world.h"
#include "settings.h"
#include "events.h"

int handleEvent(SDL_Event event, WorldResources* res)
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
			SDL_GetMouseState(&x, &y);
			Vector2 cell = {((x - MAP_TOP) / TILE_SIZE), (y - MAP_LEFT) / TILE_SIZE};
			printf("click: %d, %d\n", cell.x, cell.y);
			res->selectedPawn = getPawnAt(res, cell);
			break;
		default:
			//printf("uncaught event: %d\n", event.type);
			break;
	}
	return 0;
}