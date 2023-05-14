#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

namespace Camera {
  class OrthoCamera {
    public:
      // Set the camera's width, height, its near and far plane, its scale factor, and it's identity view and projection matrices
      unsigned int width, height;
      float near, far;
      glm::vec2 scale_factor = glm::vec2(1.0f);
      glm::mat4 view_matrix = glm::mat4(1.0f);
      glm::mat4 projection_matrix = glm::mat4(1.0f);

      // The constructor takes in the parameters to create a projection matrix
      OrthoCamera(unsigned int width, unsigned int height, float near_plane, float far_plane);

      // // Translate the camera's view matrix
      // void translate(glm::vec3 translation);
      
      // // Rotate the camera's view matrix
      // void rotate(float radians, glm::vec3 rotation);

      // Scale the camera's view matrix
      void scale(float x, float y);
      void scale(glm::vec2 factor);

      // Scale the camera's projection matrix according to the width and height provided
      void resize(unsigned int width, unsigned int height);
  };
}

#endif