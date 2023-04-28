#include "mouse.h"

#include <stdio.h>

struct MOUSE_POS mouse_pos = { false, 0, 0, 0, 0};

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
  int deltax = mouse_pos.xpos - xpos, deltay = mouse_pos.ypos - ypos;
  mouse_pos.xpos = xpos;
  mouse_pos.ypos = ypos;
  mouse_pos.deltax = deltax;
  mouse_pos.deltay = deltay;
}

void get_mouse_coords(int &x, int &y, int &deltax, int &deltay) {
  deltax = mouse_pos.deltax;
  deltay = mouse_pos.deltay;
  x = mouse_pos.xpos;
  y = mouse_pos.ypos;
  return;
}

bool get_left_click() {
  return mouse_pos.left_button_down;
}
