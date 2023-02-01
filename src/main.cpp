#include <SDL2/SDL.h>
#include <stdio.h>
#include "utils.hpp"

int main(int argc, char* args[]) {
  // Initialise the SDL window
  SDL_Window* window = NULL;
  SDL_Surface* surface = NULL;
  sdl_init(&window, &surface, "Rosewaltz Journey");

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
  SDL_UpdateWindowSurface(window);

  // Keep the window up and handle the 'X' button
  SDL_Event e;
  bool quit = false;
  while(quit == false) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  // Safely close SDL
  sdl_quit(&window);
}