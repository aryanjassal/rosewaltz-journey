#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cmath>

#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "texture.h"
#include "shaders.h"
#include "camera.h"
#include "mouse.h"

// The main function
int main() {
  // Constants
  static const int WIDTH = 1280;
  static const int HEIGHT = 720;

  // // To store the mouse position for drag-and-drop
  // struct MOUSE_POS mouse_pos;

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
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rosewaltz Journey", NULL, NULL);
  glfwMakeContextCurrent(window);

  if (window == NULL)  {
    // Print an error message if the window fails to create
    printf("GLFW window failed to initialise!");

    // Terminate GLFW
    glfwTerminate();

    // Exit the program with an error code
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=width y=height
  glViewport(0, 0, WIDTH, HEIGHT);

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

  // Create the model, view, and projection matrices
  glm::mat4 model_matrix = glm::mat4(1.0f);

  glm::mat4 view_matrix = glm::mat4(1.0f);
  view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
  view_matrix = glm::scale(view_matrix, glm::vec3((float)HEIGHT / 2.0f, (float)HEIGHT / 2.0f, 0.0f));

  glm::mat4 projection_matrix = glm::mat4(1.0f);
  projection_matrix = glm::ortho(-((float)WIDTH / 2.0f), (float)WIDTH / 2.0f, (-(float)HEIGHT / 2.0f) , (float)HEIGHT / 2.0f, 0.0f, 100.0f);

  // Create the camera
  Camera camera(model_matrix, view_matrix, projection_matrix);

  // Create the image texture
  Texture windows("textures/windows-11.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  windows.texture_unit(shader_program, "frag_texture", 0);

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Finally use the shader program
    shader_program.activate();

    // Perform transformations on the objects
    glm::mat4 identity_matrix = glm::mat4(1.0f);
    identity_matrix = glm::translate(identity_matrix, glm::vec3(0.5f, 0.5f, 0.0f));                           // Translate the object
    identity_matrix = glm::rotate(identity_matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));        // Rotate the object
    identity_matrix = glm::scale(identity_matrix, glm::vec3(0.5f, 0.5f, 0.5f));                               // Scale the object

    // Resolve the camera matrices
    camera.resolve_matrices(shader_program, "model_matrix", "view_matrix", "projection_matrix");

    // Get the transform uniform
    GLuint transform_matrix = glGetUniformLocation(shader_program.id, "transform_matrix");
    glUniformMatrix4fv(transform_matrix, 1, GL_FALSE, glm::value_ptr(identity_matrix));

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