#ifndef __MOUSE_HANDLER_H__
#define __MOUSE_HANDLER_H__

#include <GLFW/glfw3.h>

// Mouse button callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//! TEST
void get_mouse_coords(int &x, int &y, int &deltax, int &deltay);
bool get_left_click();

// Struct to store mouse position at every given frame
struct MOUSE_POS {
  bool left_button_down;
  int xpos;
  int ypos;
  int deltax;
  int deltay;
};

#endif