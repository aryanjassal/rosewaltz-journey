#ifndef __INPUT_H__
#define __INPUT_H__

#include "glm/glm.hpp"
#include "object.h"

typedef struct MouseState {
  // General mouse information
  bool left_button, left_button_down, left_button_up;
  bool right_button, right_button_down, right_button_up;
  glm::vec2 position;

  // Information relating to selected objects
  Entity *clicked_object = nullptr;

  // Reset the state
  void reset() {
    left_button_down = false;
    left_button_up = false;
    right_button_down = false;
    right_button_up = false;
  }
};

typedef struct KeyState {
  bool pressed, down, released;
  char modifiers;
};

#endif
