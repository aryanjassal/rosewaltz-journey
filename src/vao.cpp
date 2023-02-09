#include "vao.h"

VAO::VAO() {
  glGenVertexArrays(1, &id);
}

void VAO::link_attrib(VBO vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset) {
  vbo.bind();
  glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  vbo.unbind();
}

void VAO::bind() {
  glBindVertexArray(id);
}

void VAO::unbind() {
  glBindVertexArray(0);
}

void VAO::remove() {
  glDeleteVertexArrays(1, &id);
}