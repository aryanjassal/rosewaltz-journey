#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "game.h"

static int WIDTH = 1280;
static int HEIGHT = 720;

Game *RosewaltzJourney;

void resize_viewport_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
  // Initialise GLFW
  glfwInit();

  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Use anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 16);

  // Create a GLFW window
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rosewaltz Journey", NULL, NULL);
  glfwMakeContextCurrent(window);

  // If the window does not exist, then something went wrong!
  if (window == NULL)  {
    printf("GLFW window failed to initialise!");
    glfwTerminate();
    return -1;
  }

  // Change some GLFW settings post-initialisation
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetWindowSizeCallback(window, resize_viewport_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=width y=height (left to right, bottom to top)
  glViewport(0, 0, WIDTH, HEIGHT);

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  RosewaltzJourney = new Game(WIDTH, HEIGHT);
  RosewaltzJourney->init();

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    RosewaltzJourney->render();
    RosewaltzJourney->update();

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);
  }

  // Clean up and exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// Viewport callback function
void resize_viewport_callback(GLFWwindow* window, int width, int height) {
  // Actually change the OpenGL viewport settings
  glViewport(0, 0, width, height);

  // Reset the global variables referring to the width and height of the current viewport
  RosewaltzJourney->width = width;
  RosewaltzJourney->height = height;
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  RosewaltzJourney->mouse.x = xpos;
  RosewaltzJourney->mouse.y = ypos;
}