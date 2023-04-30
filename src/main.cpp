#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cmath>

#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "sprite.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
// #include "mouse.h"
#include "resource_manager.h"

// static int WIDTH = 1280;
// static int HEIGHT = 720;
static int WIDTH = 800;
static int HEIGHT = 800;
// static float ASPECT_RATIO = (float)WIDTH / (float)HEIGHT;

//! TEMP
void window_change_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  WIDTH = width;
  HEIGHT = height;
}

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

  // Change some GLFW settings post-initialisation (mostly cursor rn)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  // glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetWindowSizeCallback(window, window_change_callback);

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=width y=height (left to right, bottom to top)
  glViewport(0, 0, WIDTH, HEIGHT);

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Create a shader program, providing the vertex and fragment shaders
  Shader shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "default");

  // Create the camera
  Camera::OrthoCamera camera(WIDTH, HEIGHT, 0.0f, 100.0f);
  camera.scale(glm::vec2(5.0f, 5.0f));

  Texture image = ResourceManager::Texture::load("textures/windows-11.png", true, "sprite");

  Sprite sprite(shader);

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the sprite
    sprite.render(image);

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();  

  // Clean up and exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}