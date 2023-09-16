#include "game.h"

Game *game;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    game->Mouse.left_button = true;
    game->Mouse.left_button_down = true;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    game->Mouse.left_button = false;
    game->Mouse.left_button_up = true;
  }

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    game->Mouse.right_button = true;
    game->Mouse.right_button_down = true;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    game->Mouse.right_button = false;
    game->Mouse.right_button_up = true;
  }
}

void mouse_callback(GLFWwindow* window, double x, double y) {
  game->Mouse.position = glm::vec2(x, y);
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key >= 0) {
    if (action == GLFW_PRESS) {
      game->Keyboard[key] = { true, true, false };
    } else if (action == GLFW_RELEASE) {
      game->Keyboard[key] = { false, false, true };
    }
  }
}

int main() {
  game = new Game(1280, 720, "Rosewaltz Journey", false);
  game->set_callbacks(mouse_callback, mouse_button_callback, keyboard_callback);
  game->run();
  return 0;
}
