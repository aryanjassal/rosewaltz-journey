#include <SDL2/SDL.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[]) {
  // Tell SDL to let the window manager render the window first on X11 before trying to update it
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

  // The window we will be rendering to
  SDL_Window* window = NULL;

  // The surface contained within the screen
  SDL_Surface* surface = NULL;

  // The image to render to the screen
  SDL_Surface* arch_chan = NULL;

  // Initialise SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL couldn't initialise! SDL_Error: %s\n", SDL_GetError());
  } else {
    // Create a window
    window = SDL_CreateWindow("Rosewaltz Journey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
    // Get the surface from the window
      surface = SDL_GetWindowSurface(window);

      // Paint the surface white
      SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 255, 0));

      // Update the surface
      SDL_UpdateWindowSurface(window);

      // Render an image on the screen
      arch_chan = SDL_LoadBMP("arch-chan.bmp");
      SDL_BlitSurface(arch_chan, NULL, surface, NULL);
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
    }
  }

  // Destroy the window
  SDL_DestroyWindow(window);
  window = NULL;

  // Destroy the image
  SDL_FreeSurface(arch_chan);
  arch_chan = NULL;

  // Quit SDL subsystems
  SDL_Quit();

  // Exit the program without an error
  return 0;
}