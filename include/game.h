#ifndef __GAME_H__
#define __GAME_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <map>
#include <stdexcept>

#include "glm/glm.hpp"

#include "sprite.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "resource_manager.h"
#include "object.h"
#include "player.h"
#include "utils.h"

class Game {
  public:
    // This struct defines how information about the current mouse state is stored within the program
    typedef struct MouseState {
      // General mouse information
      bool left_button, left_button_down, left_button_up;
      glm::vec2 position;
      
      // Information relating to selected GameObjects
      GameObject *clicked_object = nullptr;
      std::vector<GameObject *> focused_objects;
    };

    // This sturct defines how information about the current key and keyboard state is stored within the program
    typedef struct KeyState {
      bool pressed, down, released;
    };

    // Set up state variables
    MouseState Mouse;
    std::map<int, KeyState> Keyboard;

    // Set up other generic variables
    unsigned int width, height;
    bool fullscreen = false;

    // The constructor function that takes the default width and height as the starting arguments
    Game(unsigned int width, unsigned int height, std::string window_title, bool fullscreen = false);
    ~Game();

    // This function initialises all the variables and other resources that the game will need access to
    void init();

    // This function handles the main game logic loop
    void run();

    // This function contains code to render stuff on the screen
    // Tip: this function should be run as fast as possible
    void render();

    // This function contains code to update variables like input events, etc.
    // Tip: this function should only run once every frame (use delta time)
    void update();

    // Set GLFW callbacks
    void set_callbacks(GLFWcursorposfun cursorpos_callback, GLFWmousebuttonfun cursorbutton_callback, GLFWkeyfun keyboard_callback);
  
  private:
    // The GLFW window to which everything is output
    GLFWwindow *GameWindow;

    // Store the name of the window
    std::string GameTitle;

    // Set all flags/hints for the window
    void set_window_hints();

    // Create a GLFW window
    void create_window(GLFWwindow *&window);

    // Toggle the fullscreen status of the window
    void toggle_fullscreen();
};

#endif
