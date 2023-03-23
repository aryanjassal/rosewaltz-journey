#ifndef __MOUSE_HANDLER_H__
#define __MOUSE_HANDLER_H__

#include <GLFW/glfw3.h>

// Mouse button callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 

// Struct to store mouse position at every given frame
struct MOUSE_POS {
  bool left_button_down;
  double xpos;
  double ypos;
};

static struct MOUSE_POS mouse_pos;

#endif