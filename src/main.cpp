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
  static const int WIDTH = 1280;
  static const int HEIGHT = 720;
  static const float ASPECT_RATIO = (float)WIDTH / (float)HEIGHT;

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
  /* VERTEX COORDINATES \\\ TEXTURE COORDINATES  */
    -0.5f, -0.5f, 0.0f,         0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,         0.0f, 1.0f,
     0.5f,  0.5f, 0.0f,         1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,         1.0f, 0.0f
  };

  // Set the index buffer to create the square
  GLuint square_indices[] = {
    0, 2, 1,
    0, 3, 2
  };

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

  // Create a shader program, providing the vertex and fragment shaders
  Shader shader_program("src/shaders/default.vert", "src/shaders/default.frag");

  // Create and bind the VAO
  VAO vao1;
  vao1.bind();

  // Create and bind the VBO and the EBO
  VBO vbo1(square_vertices, sizeof(square_vertices));
  EBO ebo1(square_indices, sizeof(square_indices));

  // Tell the VAO the structure of the vertices and texture coordinates that the vertices store
  //TODO: Automate the numbers (which are very easy to mess up)
  vao1.link_attrib(vbo1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*) 0);
  vao1.link_attrib(vbo1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*) (3 * sizeof(float)));

  // Unbind VAO, VBO, and EBO
  vao1.unbind();
  vbo1.unbind();
  ebo1.unbind();

  // Model matrix
  glm::mat4 model_matrix = glm::mat4(1.0f);
  // model_matrix = glm::translate(model_matrix, glm::vec3(0.5f * (float)((float)WIDTH / (float)HEIGHT), 0.5f, 0.0f));

  // View matrix
  glm::mat4 view_matrix = glm::mat4(1.0f);
  // view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
  view_matrix = glm::scale(view_matrix, glm::vec3((float)HEIGHT / 1.0f, (float)HEIGHT / 1.0f, 0.0f));

  // Projection matrix
  glm::mat4 projection_matrix = glm::mat4(1.0f);
  projection_matrix = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, 0.0f, 100.0f);

  // Create the camera
  Camera camera(view_matrix, projection_matrix);

  // Create the image texture
  Texture windows("textures/windows-11.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  windows.texture_unit(shader_program, "frag_texture", 0);

  //! TEMP DONT MIND THIS BUT REMOVE IT LATER
  glm::mat4 new_matrix = model_matrix;

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Finally use the shader program
    shader_program.activate();

    //! TEMP TESTING DRAG MECHANIC
    int xpos, ypos, deltax, deltay;
    get_mouse_coords(xpos, ypos, deltax, deltay);
    if (get_left_click()) {
      float xval = ASPECT_RATIO * ((float)(xpos) / (float)(WIDTH)), yval = ASPECT_RATIO * ((float)(HEIGHT) / (float)(WIDTH)) * ((float)(ypos) / (float)(HEIGHT));
      glm::vec3 newpos = glm::vec3(xval, yval, 0.0f);
      new_matrix = glm::translate(model_matrix, newpos);
    }
    // printf("\nrelative mouse position: [%.5f,%.5f]\n", (float)((float)(xpos) / (float)(WIDTH)), (float)((float)(ypos) / (float)(HEIGHT)));
    // printf("mouse coords: [%i,%i]\n", xpos, ypos);
    // printf("object location: [%.5f,%.5f]\n", xval, yval);

    // Resolve the camera matrices (the camera information needs to update every frame for the shaders to work properly)
    camera.resolve_matrices(shader_program, "view_matrix", "projection_matrix");

    //! TEMP
    // Use the model matrix to transform one object instead of them all
    int model_shader = glGetUniformLocation(shader_program.id, "model_matrix");
    glUniformMatrix4fv(model_shader, 1, GL_FALSE, glm::value_ptr(new_matrix));
    // glUniformMatrix4fv(model_shader, 1, GL_FALSE, glm::value_ptr(model_matrix));

    // Bind the texture
    //! the hell?
    windows.bind();

    // Tell OpenGL which VAO's to use
    vao1.bind();

    // Finally, draw the mesh using the vertexes specified in the Vertex Array Object using the Element Array Object
    //TODO: Automate this too, as whenever the structure of the vertices array changes, this also needs to change
    glDrawElements(GL_TRIANGLES, sizeof(square_vertices) / 5, GL_UNSIGNED_INT, 0);

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  //TODO: Find a way to automatically store all the created objects in an array and remove them at the end
  // Delete all the objects we created so far
  vao1.remove();
  vbo1.remove();
  ebo1.remove();
  windows.remove();
  shader_program.remove();

  // Clean up and exit the program
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void render() {

}