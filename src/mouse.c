#include "mouse.h"

#include <stdio.h>

// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    mouse_pos.left_button_down = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    mouse_pos.left_button_down = false;
  }
}

// Cursorpos callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  mouse_pos.xpos = xpos;
  mouse_pos.ypos = ypos;

  if (mouse_pos.left_button_down)
    printf("cursor pos: [%.2f,%.2f]\n", xpos, ypos);
}