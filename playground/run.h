#ifndef SOREN_PLAYGROUND_RUN_H
#define SOREN_PLAYGROUND_RUN_H

#include <soren_std.h>
#include <SDL3/SDL.h>
#include <graphics/text/soren_font.h>

extern FontInterface* font;
extern char* title;

void game_init(SDL_Window* window, SDL_Renderer* renderer);
void game_update(SDL_Window* window, SDL_Renderer* renderer);

#endif