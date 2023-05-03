#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include "sprite.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "resource_manager.h"

static int WIDTH = 1280;
static int HEIGHT = 720;

int mouse_pos_x, mouse_pos_y;
// bool left_click;

// Viewport callback function
void resize_viewport(GLFWwindow* window, int width, int height) {
  // Actually change the OpenGL viewport settings
  glViewport(0, 0, width, height);

  // Reset the global variables referring to the width and height of the current viewport
  WIDTH = width;
  HEIGHT = height;
}

// // Mouse callback function
// void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
//   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//     left_click = true;
//   } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
//     left_click = false;
//   }
// }

// Cursorpos callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  mouse_pos_x = xpos;
  mouse_pos_y = ypos;
}

// Set up pointers to the rendering engine of the game
SpriteRenderer *Renderer;

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
  glfwSetWindowSizeCallback(window, resize_viewport);
  glfwSetCursorPosCallback(window, mouse_callback);

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=width y=height (left to right, bottom to top)
  glViewport(0, 0, WIDTH, HEIGHT);

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set the clear colour of the scene background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

  // Create a shader program, providing the vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "sprite");

  // Create the camera
  Camera::OrthoCamera camera(WIDTH, HEIGHT, -1.0f, 1.0f);

  // Actually create a sprite renderer instance
  Renderer = new SpriteRenderer(sprite_shader, camera);

  // Load textures into the game
  ResourceManager::Texture::load("textures/windows-11.png", true, "windows-icon");

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Clear the screen (paints it to the predefined clear colour)
    glClear(GL_COLOR_BUFFER_BIT);

    // Render a sprite at the location specified by scaling the mouse position with the camera's scale factor
    Renderer->render(ResourceManager::Texture::get("windows-icon"), glm::vec2(mouse_pos_x / camera.scale_factor.x, mouse_pos_y / camera.scale_factor.y), glm::vec2(100.0f, 100.0f));

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();
  delete Renderer;

  // Clean up and exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}