#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

class Camera {
 public:
  // Set the camera's dimensions
  unsigned int width, height;

  // The camera scale, view, and projection matrix.
  glm::mat4 view_matrix = glm::mat4(1.0f);
  glm::mat4 projection_matrix = glm::mat4(1.0f);

  // The constructor takes in the parameters to create a projection matrix
  Camera(unsigned int width, unsigned int height, float near_plane, float far_plane, glm::mat4 projection_matrix, glm::mat4 view_matrix = glm::mat4(1.0f));

  // Scale the camera's view matrix
  void scale(float x, float y);
  void scale(glm::vec2 factor);

  // The closest and the farthest plane the camera will render
  float near, far;

// protected:
//   // The closest and the farthest plane the camera will render
//   float near, far;
};

class OrthoCamera : public Camera {
  public:
    // Define a constructor for the class
    OrthoCamera(unsigned int width, unsigned int height, float near_plane, float far_plane);

    // Scale the camera's projection matrix according to the width and height provided
    void resize(unsigned int width, unsigned int height);
};

#endif
