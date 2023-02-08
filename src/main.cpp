#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cmath>

#include "shaders.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

int main() {
  // Initialise GLFW
  glfwInit();

  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Use anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 16);

  // Set vertices of the triangle
  GLfloat triangle_vertices[] = {
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,      // Bottom left corner
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,       // Bottom right corner
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,    // Top corner
    -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,   // Inner left center
    0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,    // Inner right center
    0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f        // Inner bottom center
  };

  // The index of how to render the vectors
  GLuint indices[] = {
    0, 3, 5,  // Bottom left triangle
    3, 2, 4,  // Top triangle
    5, 4, 1   // Bottom right triangle
  };

  // Create a GLFW window
  GLFWwindow* window = glfwCreateWindow(600, 600, "Rosewaltz Journey", NULL, NULL);
  glfwMakeContextCurrent(window);

  // If the window fails to initialise, then exit the program immediately
  if (window == NULL) {
    // Print an error message if the window fails to create
    printf("GLFW failed to create a window!\n");

    // Terminate GLFW
    glfwTerminate();

    // Exit the program with an error code
    return -1;
  }

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=600 y=600
  glViewport(0, 0, 600, 600);

  Shader shader_program("src/shaders/default.vert", "src/shaders/default.frag");

  VAO vao1;
  vao1.bind();

  VBO vbo1(triangle_vertices, sizeof(triangle_vertices));
  EBO ebo1(indices, sizeof(indices));

  vao1.link_vbo(vbo1, 0);
  vao1.unbind();
  vbo1.unbind();
  ebo1.unbind();

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // // Finally use the shader program
    shader_program.activate();

    // Tell OpenGL which VAO's to use
    vao1.bind();

    // Finally, draw the triangle using the vertexes specified in the Vertex Array Object using the Element Array Object
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  // Delete all the objects we created so far
  vao1.remove();
  vbo1.remove();
  ebo1.remove();
  shader_program.remove();

  // Destroy the window that we created
  glfwDestroyWindow(window);

  // Terminate GLFW
  glfwTerminate();

  // Exit the program
  return 0;
}