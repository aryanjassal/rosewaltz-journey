#include "physics.h"

Direction vector_direction(glm::vec2 target) {
  glm::vec2 compass[] = {
    glm::vec2(0.0f, 1.0f),	// Up
    glm::vec2(1.0f, 0.0f),	// Right
    glm::vec2(0.0f, -1.0f),	// Down
    glm::vec2(-1.0f, 0.0f)	// Left
  };

  float max = 0.0f;
  unsigned int match = -1;

  for (int i = 0; i < 4; i++) {
    float dot = glm::dot(glm::normalize(target), compass[i]);
    if (dot > max) {
      max = dot;
      match = i;
    }
  }

  return (Direction)match;
}
