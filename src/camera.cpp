#include "camera.h"

Camera::OrthoCamera::OrthoCamera(unsigned int width, unsigned int height, unsigned int near_plane, unsigned int far_plane) {
  // Set the camera width and height
  this->width = width;
  this->height = height;

  // Create and set the projection matrix
  this->projection_matrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, (float)near_plane, (float)far_plane);
}

void Camera::OrthoCamera::scale(float x, float y) {
  this->view_matrix = glm::scale(this->view_matrix, glm::vec3(x, y, 0.0f));
}

void Camera::OrthoCamera::scale(glm::vec2 factor) {
  this->view_matrix = glm::scale(this->view_matrix, glm::vec3(factor, 0.0f));
}