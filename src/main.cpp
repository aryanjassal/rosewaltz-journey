#include "game.h"

Game *RosewaltzJourney;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double x, double y);

int main() {
  RosewaltzJourney = new Game(1366, 768, "Rosewaltz Journey");
  RosewaltzJourney->init();
  RosewaltzJourney->set_callbacks(mouse_callback, mouse_button_callback);
  RosewaltzJourney->run();

  return 0;
}

// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    RosewaltzJourney->mouse.buttons.left_button = true;
    RosewaltzJourney->mouse.buttons.left_button_down = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    RosewaltzJourney->mouse.buttons.left_button = false;
    RosewaltzJourney->mouse.buttons.left_button_up = true;
  }
}

// Callback function to update the position of the mouse
void mouse_callback(GLFWwindow* window, double x, double y) {
  RosewaltzJourney->mouse.x = x;
  RosewaltzJourney->mouse.y = y;
}