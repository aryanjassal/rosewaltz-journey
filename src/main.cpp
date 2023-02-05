#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// Temporary vertex shader
const char* vertex_shader_source = "#version 330 core\n"
"layout (location = 0) in vec3 pos_a;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(pos_a.x, pos_a.y, pos_a.z, 1.0);\n"
"}\0";

// Temporary fragment shader
const char* fragment_shader_source = "#version 330 core\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
" frag_color = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\0";

int main() {
  // Initialise GLFW
  glfwInit();

  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Set vertices of the triangle
  GLfloat triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };

  // Create a GLFW window
  GLFWwindow* window = glfwCreateWindow(640, 480, "Rosewaltz Journey", NULL, NULL);
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
  // In this case, the viewport goes from x=0 y=0 to x=640 y=480
  glViewport(0, 0, 640, 480);

  // Create the vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  // Create the fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  // Attach the shaders to the OpenGL program
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  // Delete the shaders as they have already been loaded into memory
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // NOTE THE ORDER OF VAO'S AND VBO'S BECAUSE IT MATTERS A LOT
  // Create a Verted Array Object to store multiple VBO's (more information on VBO's later)
  GLuint VAO;

  // Create a Vertex Buffer Object to store the vertex buffer data
  // This usually is an array of objects, but we only need one for now
  GLuint VBO;

  // Create the Vertex Attribute Object to store the vertex attributes
  // The size is 1 as there is only one VAO object
  glGenVertexArrays(1, &VAO);

  // Actually create the VBO buffer
  // The size is 1 as there is only one VBO object
  glGenBuffers(1, &VBO);

  // Bind the VAO buffer (not too sure what this does)
  glBindVertexArray(VAO);
  
  // Bind the buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Now let's actually store the vertex data in the buffers
  // Look up the last parameter in more detail as it can be used to save resources if correctly configured
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

  // Now set some attributes so OpenGL actually knows how to read the vertex data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

  // Confirm that the OpenGL attributes are actually enabled and applied
  // Not required, but good to do
  glEnableVertexAttribArray(0);

  // Bind the buffers to ensure nothing is changed later during runtime
  // Note the ordering because that matters
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Paint the window surface to the given color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);

  // Main events loop
  while(!glfwWindowShouldClose(window)) {
    // Paint the window surface to the given color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Finally use the shader program
    glUseProgram(shader_program);

    // Tell OpenGL which VAO's to use
    glBindVertexArray(VAO);

    // Finally, draw the triangle using the vertexes specified in the Vertex Array Object
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(window);

    // Poll GLFW for new events
    glfwPollEvents();
  }

  // Delete the Vertex Array Object
  glDeleteVertexArrays(1, &VAO);

  // Delete the Vertex Attribute Object
  glDeleteVertexArrays(1, &VAO);

  // Delete the Vertex Buffer Object
  glDeleteBuffers(1, &VBO);

  // Delete the shader program
  glDeleteProgram(shader_program);

  // Destroy the window that we created
  glfwDestroyWindow(window);

  // Terminate GLFW
  glfwTerminate();

  // Exit the program
  return 0;
}