#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
  model_matrix = model;
  view_matrix = view;
  projection_matrix = projection;
}

void Camera::translate(glm::vec3 translation) {
  projection_matrix = glm::translate(projection_matrix, translation);
}

void Camera::rotate(float radians, glm::vec3 rotation) {
  projection_matrix = glm::rotate(projection_matrix, radians, rotation);
}