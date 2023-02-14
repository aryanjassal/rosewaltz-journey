#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cmath>

#include "stb/stb_image.h"
#include "texture.h"
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

  // Set vertices, color, and texture coordinates of a square
  GLfloat square_vertices[] = {
  /* VERTEX COORDINATES   \\\   COLOR INFORMATION   \\\   TEXTURE COORDINATES  */
    -0.5f, -0.5f, 0.0f,         1.0f, 0.0f, 0.0f,             0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,             0.0f, 1.0f,
     0.5f,  0.5f, 0.0f,         0.0f, 0.0f, 1.0f,             1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,         1.0f, 1.0f, 1.0f,             1.0f, 0.0f
  };

  // Set the index buffer to create the square
  GLuint square_indices[] = {
    0, 2, 1,
    0, 3, 2
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

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Create a shader program, providing the vertex and fragment shaders
  Shader shader_program("src/shaders/default.vert", "src/shaders/default.frag");

  // Create and bind the VAO
  VAO vao1;
  vao1.bind();

  // Create and bind the VBO and the EBO
  VBO vbo1(square_vertices, sizeof(square_vertices));
  EBO ebo1(square_indices, sizeof(square_indices));

  // Tell the VAO the structure of the vertices and colors that the vertices store
  vao1.link_attrib(vbo1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*) 0);
  vao1.link_attrib(vbo1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*) (3 * sizeof(float)));
  vao1.link_attrib(vbo1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*) (6 * sizeof(float)));

  // Unbind VAO, VBO, and EBO
  vao1.unbind();
  vbo1.unbind();
  ebo1.unbind();

  // Create the image texture
  Texture windows("textures/windows-11.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  windows.texture_unit(shader_program, "frag_texture", 0);

  // Get the uniform scale variable from within the shader
  GLuint scale_id = glGetUniformLocation(shader_program.id, "scale");

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Finally use the shader program
    shader_program.activate();

    // Change the scale of the triangles
    // Note that this can only be done after activating the shader
    glUniform1f(scale_id, 1.0f);

    // Bind the texture
    windows.bind();

    // Tell OpenGL which VAO's to use
    vao1.bind();

    // Finally, draw the triangle using the vertexes specified in the Vertex Array Object using the Element Array Object
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  // Delete all the objects we created so far
  vao1.remove();
  vbo1.remove();
  ebo1.remove();
  windows.remove();
  shader_program.remove();

  // Destroy the window that we created
  glfwDestroyWindow(window);

  // Terminate GLFW
  glfwTerminate();

  // Exit the program
  return 0;
}