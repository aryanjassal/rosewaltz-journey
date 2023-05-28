#include "camera.h"

Camera::Camera(unsigned int width, unsigned int height, float near_plane, float far_plane, glm::mat4 projection_matrix, glm::mat4 view_matrix) {
  // Set the camera width and height
  this->width = width;
  this->height = height;
  this->near = near_plane;
  this->far = far_plane;
  this->projection_matrix = projection_matrix;
}

void Camera::scale(float x, float y) {
  this->view_matrix = glm::scale(this->view_matrix, glm::vec3(x, y, 0.0f));
}

void Camera::scale(glm::vec2 factor) {
  this->view_matrix = glm::scale(this->view_matrix, glm::vec3(factor, 0.0f));
}

OrthoCamera::OrthoCamera(unsigned int width, unsigned int height, float near_plane, float far_plane)
: Camera(width, height, near_plane, far_plane, glm::mat4(1.0f)) {
  this->width = width;
  this->height = height;
  this->near = near_plane;
  this->far = far_plane;
  this->projection_matrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, near_plane, far_plane);
}

void OrthoCamera::resize(unsigned int width, unsigned int height) {
  this->width = width;
  this->height = height;
  this->projection_matrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, this->near, this->far);
}


