#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include "glm/glm.hpp"
#include "shader.h"

class Camera {
  public:
    // The camera's dimensions
    glm::vec2 dimensions;
    float width() const { return this->dimensions.x; }
    float height() const { return this->dimensions.y; }

    // Returns the camera's matrices
    glm::mat4 view_matrix() const { return this->view; }
    glm::mat4 projection_matrix() const { return this->projection; }

    // Return the near and far plane
    float near_plane() const { return this->near; }
    float far_plane() const { return this->far; }

    // The constructor takes in the parameters to create a projection matrix
    Camera() : dimensions(glm::vec2()), near(0.0f), far(0.0f) { }
    Camera(unsigned int width, unsigned int height, float near_plane, float far_plane);

    // Scale the camera's view matrix
    void scale(float x, float y);
    void scale(glm::vec2 factor);

    // Resize the camera's projection matrix
    void resize(unsigned int width, unsigned int height);
    void resize(glm::vec2 dimensions);

    // Are the coordinates in range of the camera?
    bool in_range(glm::vec2 position, float z);

  private:
    // The closest and the farthest plane the camera will render
    float near, far;

    // The camera view and projection matrix.
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
};

#endif
