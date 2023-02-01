#include <SDL2/SDL.h>
#include <stdio.h>
#include "utils.hpp"

// Initialise the SDL graphics API
void sdl_init(SDL_Window** window, SDL_Surface** surface, char* window_title, int screen_width, int screen_height) {
  // Tell SDL to let the window manager render the window first on X11 before trying to update it
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

  // Initialise SDL video
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    // If it couldn't initialise, then print out an error message and quit the application
    printf("SDL couldn't initialise! SDL_ERROR: %s\n", SDL_GetError());
    exit(0);
  } else {
    // Create a physical window with the options provided in the [window_options] parameter
    *window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);

    // If the window is still null, then something went wrong in the window creation process. Print out an error message then quit the application.
    if (*window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      exit(0);
    } else {
      // Get the surface that we will render on from the window
      *surface = SDL_GetWindowSurface(*window);
    }
  }

  // This function will only return if there were no errors
}

// Clean up and quit the application
void sdl_quit(SDL_Window** window) {
  // Destroy the window and nullify the memory location
  SDL_DestroyWindow(*window);
  window = NULL;
  
  // Quit SDL subsystems
  SDL_Quit();

  // Exit the application
  exit(0);
}