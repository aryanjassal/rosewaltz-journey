#include "utils.h"
#include "object.h"

double Time::delta = 0.0f;

glm::vec2 WindowSize = glm::vec2(0.0f);

glm::vec2 screen_to_world(glm::vec2 screen_point) {
  // printf("windowsize: %.2f, %.2f\n", WindowSize.x, WindowSize.y);
  // printf("%.2f, %.2f\n", ((screen_point / WindowSize) * glm::vec2(GameObjects::Camera->width, GameObjects::Camera->height)).x, ((screen_point / WindowSize) * glm::vec2(GameObjects::Camera->width, GameObjects::Camera->height)).y);
  return (screen_point / WindowSize) * glm::vec2(GameObjects::Camera->width, GameObjects::Camera->height);
}

glm::vec3 screen_to_world(glm::vec3 screen_point) {
  return (screen_point / glm::vec3(WindowSize, 0.0f)) * glm::vec3(GameObjects::Camera->width, GameObjects::Camera->height, 0.0f);
}
