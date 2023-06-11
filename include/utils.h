#ifndef __UTILS_H__
#define __UTILS_H__

#include "glm/glm.hpp"

// Declare commonly used global variables
extern glm::vec2 WindowSize;

// Struct holding the transformations to be applied to the object
typedef struct Transform {
  // Declare the constructors for a transform
  Transform(): position(glm::vec3(0.0f)), scale(glm::vec2(100.0f)), rotation(0.0f) { }
  Transform(glm::vec3 _position): position{_position}, scale(glm::vec2(100.0f)), rotation(0.0f) { }
  Transform(glm::vec3 _position, glm::vec2 _scale): position{_position}, scale{_scale}, rotation(0.0f) { }
  Transform(glm::vec3 _position, glm::vec2 _scale, float _rotation): position{_position}, scale{_scale}, rotation{_rotation} { }

  // Fields controlled by transform
  glm::vec3 position;
  glm::vec2 scale;
  float rotation;
};

#endif