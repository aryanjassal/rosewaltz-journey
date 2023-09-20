#ifndef __GAME_H__
#define __GAME_H__

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include <cstdio>
#include <map>
#include <stdexcept>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include <thread>

#include "input.h"
#include "renderer.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "resource_manager.h"
#include "object.h"
#include "player.h"
#include "utils.h"
#include "font.h"
#include "physics.h"
#include "tutils.h"

class Game {
  public:
    // Set up state variables
    MouseState Mouse;
    KeyboardState Keyboard;

    // Set up other generic variables
    unsigned int width, height;
    bool fullscreen = false;

    // The constructor function that takes the default width and height as the starting arguments
    Game(unsigned int width, unsigned int height, std::string window_title, bool fullscreen = false);
    ~Game();

    // This function handles the main game logic loop
    void run();

    // Set the game as active, redirecting input events to this game
    void set_active();
    
  private:
    // Global variables used by the game
    GLFWwindow *window;
    Renderer *renderer;
    Camera *camera;

    // Store the name of the window
    std::string title;

    // Set all flags/hints for the window
    void set_window_hints();

    // Create a GLFW window
    void create_window();

    // Toggle the fullscreen status of the window
    void toggle_fullscreen();

    // Render stuff onto the screen
    void render();
    
    // Run the logic and other computations
    void update();
};

#endif
