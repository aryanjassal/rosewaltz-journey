#include <SDL2/SDL.h>

// Initialise the SDL graphics API
void sdl_init(SDL_Window** window, SDL_Surface** surface, char* window_title, int screen_width=640, int screen_height=480);

// Clean up and quit the application
void sdl_quit(SDL_Window** window);