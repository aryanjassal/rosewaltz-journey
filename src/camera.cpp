#include "camera.h"

Camera::Camera(unsigned int width, unsigned int height, float near_plane, float far_plane) {
  this->dimensions = glm::vec2(width, height);
  this->near = near_plane;
  this->far = far_plane;
  this->projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, near_plane, far_plane);
}

void Camera::scale(float x, float y) {
  this->view = glm::scale(this->view, glm::vec3(x, y, 0.0f));
}

void Camera::scale(glm::vec2 factor) {
  this->view = glm::scale(this->view, glm::vec3(factor, 0.0f));
}

void Camera::resize(unsigned int width, unsigned int height) {
  this->dimensions = glm::vec2(width, height);
  this->projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, this->near, this->far);
}

void Camera::resize(glm::vec2 dimensions) {
  this->projection = glm::ortho(0.0f, dimensions.x, dimensions.y, 0.0f, this->near, this->far);
}

bool Camera::in_range(glm::vec2 position, float z) {
  return (z >= this->near && z <= this->far);
}
