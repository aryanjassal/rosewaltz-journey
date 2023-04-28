#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Camera::Camera(glm::mat4 view, glm::mat4 projection) {
  view_matrix = view;
  projection_matrix = projection;
}

void Camera::translate(glm::vec3 translation) {
  projection_matrix = glm::translate(projection_matrix, translation);
}

void Camera::rotate(float radians, glm::vec3 rotation) {
  projection_matrix = glm::rotate(projection_matrix, radians, rotation);
}

void Camera::resolve_matrices(Shader& shader,  const char* view_uniform, const char* projection_uniform) {
  int view_shader = glGetUniformLocation(shader.id, view_uniform);
  glUniformMatrix4fv(view_shader, 1, GL_FALSE, glm::value_ptr(view_matrix));

  int projection_shader = glGetUniformLocation(shader.id, projection_uniform);
  glUniformMatrix4fv(projection_shader, 1, GL_FALSE, glm::value_ptr(projection_matrix));
}