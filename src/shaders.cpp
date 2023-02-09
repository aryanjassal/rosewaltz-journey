#include "shaders.h"

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
  compile_errors(vertex_shader, "VERTEX");

  // Create the fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);
  compile_errors(fragment_shader, "FRAGMENT");

  // Attach the shaders to the OpenGL program
  id = glCreateProgram();
  glAttachShader(id, vertex_shader);
  glAttachShader(id, fragment_shader);
  glLinkProgram(id);
  compile_errors(id, "PROGRAM");

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

// Check if the shaders compile
// I have no real idea of what is going on here
void Shader::compile_errors(unsigned int shader, const char* type) {
  GLint has_compiled;
  char info_log[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &has_compiled);
    if (has_compiled == GL_FALSE) {
      glGetShaderInfoLog(shader, 1024, NULL, info_log);
      printf("SHADER_COMPILATION_ERROR for: %s\n", type);
    }
  } else {
    glGetProgramiv(shader, GL_COMPILE_STATUS, &has_compiled);
    if (has_compiled == GL_FALSE) {
      glGetProgramInfoLog(shader, 1024, NULL, info_log);
      printf("SHADER_LINKING_ERROR for: %s\n", type);
    }
  }
}
