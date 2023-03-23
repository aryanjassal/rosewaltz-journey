#ifndef __CAMERA_CLASS_H__
#define __CAMERA_CLASS_H__

#include "glm/glm.hpp"
#include "shaders.h"

class Camera {
  public:
    Camera(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void translate(glm::vec3 translation);
    void rotate(float radians, glm::vec3 rotation);
    void resolve_matrices(Shader& shader, const char* model_uniform, const char* view_uniform, const char* projection_uniform);
  
  private:
    // Camera matrices
    glm::mat4 model_matrix;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    // Camera position
    glm::vec3 camera_position;
};

#endif