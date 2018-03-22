#ifndef EVENTS_H
#define EVENTS_H

#include <SDL2/SDL.h>

int handleEvent(SDL_Event event, GameInfo* gameInfo, WorldResources* resources);

#endif
