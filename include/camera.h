#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include "glm/glm.hpp"

class Camera {
  public:
    glm::mat4 model_matrix;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    Camera(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void translate(glm::vec3 translation);
    void rotate(float radians, glm::vec3 rotation);
};

#endif