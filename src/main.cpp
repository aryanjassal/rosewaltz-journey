#include "game.h"

Game *RosewaltzJourney;

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void mouse_callback(GLFWwindow *window, double x, double y);
void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int main() {
  RosewaltzJourney = new Game(1280, 720, "Rosewaltz Journey");
  RosewaltzJourney->init();
  RosewaltzJourney->set_callbacks(mouse_callback, mouse_button_callback, keyboard_callback);
  RosewaltzJourney->run();

  return 0;
}

// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    RosewaltzJourney->Mouse.left_button = true;
    RosewaltzJourney->Mouse.left_button_down = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    RosewaltzJourney->Mouse.left_button = false;
    RosewaltzJourney->Mouse.left_button_up = true;
  }
}

// Callback function to update the position of the mouse
void mouse_callback(GLFWwindow* window, double x, double y) {
  RosewaltzJourney->Mouse.position = glm::vec2(x, y);
}

// Callback function to interact with the keyboard
void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key >= 0) {
    if (action == GLFW_PRESS)
      RosewaltzJourney->Keyboard[key] = { true, true, false };
    else if (action == GLFW_RELEASE)
      RosewaltzJourney->Keyboard[key] = { false, false, true };
  }
}
