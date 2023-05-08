#ifndef __GAME_H__
#define __GAME_H__

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "glm/glm.hpp"

#include "sprite.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "resource_manager.h"
#include "object.h"

class Game {
  public:
    // This struct defines how information about mouse buttons is stored within the program
    typedef struct MouseButtonState {
      bool left_button;
      bool left_button_down, left_button_up;
    };

    // This struct defines how information about the current mouse state is stored within the program
    typedef struct MouseState {
      int x, y;
      MouseButtonState buttons;
      GameObject::Object *active_object;
    };

    // Set up variables
    MouseState mouse = { 0, 0, { false, false, false }, nullptr };
    unsigned int width, height;

    // The constructor function that takes the default width and height as the starting arguments
    Game(unsigned int width, unsigned int height);
    ~Game();

    // This function initialises all the variables and other resources that the game will need access to
    void init();

    // This function contains code to render stuff on the screen
    // Note: this function should be run as fast as possible
    void render();

    // This function contains code to update variables like input events, etc.
    // Note: this function should only run once every frame (use delta time)
    void update();

    // Callback functions for GLFW
    void resize_viewport_callback(GLFWwindow* window, int width, int height);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif