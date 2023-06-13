#include "shader.h"

void Shader::compile(const char *vertex_code, const char *fragment_code, const char *geometry_code) {
  // Create the vertex shader
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_code, NULL);
  glCompileShader(vertex_shader);
  compile_errors(vertex_shader, "VERTEX");

  // Create the fragment shader
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_code, NULL);
  glCompileShader(fragment_shader);
  compile_errors(fragment_shader, "FRAGMENT");

  // Create the geometry shader
  unsigned int geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometry_shader, 1, &geometry_code, NULL);
  glCompileShader(geometry_shader);
  compile_errors(geometry_shader, "GEOMETRY");

  // Attach the shaders to the OpenGL program
  id = glCreateProgram();
  glAttachShader(id, vertex_shader);
  glAttachShader(id, fragment_shader);
  glAttachShader(id, geometry_shader);
  glLinkProgram(id);
  compile_errors(id, "PROGRAM");

  // Delete the shaders as they have already been linked to the shader
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glDeleteShader(geometry_shader);
}

void Shader::compile(const char *vertex_code, const char *fragment_code) {
  // Create the vertex shader
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_code, NULL);
  glCompileShader(vertex_shader);
  compile_errors(vertex_shader, "VERTEX");

  // Create the fragment shader
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_code, NULL);
  glCompileShader(fragment_shader);
  compile_errors(fragment_shader, "FRAGMENT");

  // Attach the shaders to the OpenGL program
  id = glCreateProgram();
  glAttachShader(id, vertex_shader);
  glAttachShader(id, fragment_shader);
  glLinkProgram(id);
  compile_errors(id, "PROGRAM");

  // Delete the shaders as they have already been linked to the shader
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Shader::activate() {
  glUseProgram(id);
}

void Shader::set_float(const char *handle, float value) {
  glUniform1f(glGetUniformLocation(this->id, handle), value);
}

void Shader::set_integer(const char *handle, int value) {
  glUniform1i(glGetUniformLocation(this->id, handle), value);
}

void Shader::set_vector_2f(const char *handle, float x, float y) {
  glUniform2f(glGetUniformLocation(this->id, handle), x, y);
}

void Shader::set_vector_2f(const char *handle, const glm::vec2 &value) {
  glUniform2f(glGetUniformLocation(this->id, handle), value.x, value.y);
}

void Shader::set_vector_3f(const char *handle, float x, float y, float z) {
  glUniform3f(glGetUniformLocation(this->id, handle), x, y, z);
}

void Shader::set_vector_3f(const char *handle, const glm::vec3 &value) {
  glUniform3f(glGetUniformLocation(this->id, handle), value.x, value.y, value.z);
}

void Shader::set_vector_4f(const char *handle, float x, float y, float z, float w) {
  glUniform4f(glGetUniformLocation(this->id, handle), x, y, z, w);
}

void Shader::set_vector_4f(const char *handle, const glm::vec4 &value) {
  glUniform4f(glGetUniformLocation(this->id, handle), value.x, value.y, value.z, value.w);
}

void Shader::set_matrix_4f(const char *handle, const glm::mat4 &value) {
  glUniformMatrix4fv(glGetUniformLocation(this->id, handle), 1, false, glm::value_ptr(value));
}

void Shader::set_bool(const char *handle, bool value) {
  glUniform1i(glGetUniformLocation(this->id, handle), value ? 1 : 0);
}

// Check if the shaders compile
void Shader::compile_errors(unsigned int shader, const char *type) {
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
