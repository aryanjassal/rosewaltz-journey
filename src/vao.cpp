#include "vao.h"

VAO::VAO() {
  glGenVertexArrays(1, &id);
}

void VAO::link_vbo(VBO vbo, GLuint layout) {
  vbo.bind();
  glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
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