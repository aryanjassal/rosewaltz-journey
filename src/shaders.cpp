#include "shaders.h"
#include <fstream>

std::string get_file_contents(const char* filename) {
  std::ifstream file(filename, std::ios::binary);
  if (file) {
    std::string contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&contents[0], contents.size());
    file.close();
    return contents;
  }
  throw(errno);
}

Shader::Shader(const char* vertex_file, const char* fragment_file) {
  std::string vertex_code = get_file_contents(vertex_file);
  std::string fragment_code = get_file_contents(fragment_file);

  const char* vertex_source = vertex_code.c_str();
  const char* fragment_source = fragment_code.c_str();

  // Create the vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);

  // Create the fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);

  // Attach the shaders to the OpenGL program
  id = glCreateProgram();
  glAttachShader(id, vertex_shader);
  glAttachShader(id, fragment_shader);
  glLinkProgram(id);

  // Delete the shaders as they have already been loaded into memory
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Shader::activate() {
  glUseProgram(id);
}

void Shader::remove() {
  glDeleteProgram(id);
}