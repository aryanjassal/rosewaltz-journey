#include "utils.h"
#include "object.h"

double Time::delta = 0.0f;

glm::vec2 WindowSize = glm::vec2(0.0f);

glm::vec2 screen_to_world(glm::vec2 screen_point) {
  return (screen_point / WindowSize) * Entities::Camera->dimensions;
}

void Timer::tick(double time_seconds) {
  double time_milliseconds = time_seconds * 1000.0f;
  this->time -= time_milliseconds;
}

bool Timer::test() {
  return (this->time <= 0.0f);
}

void Timer::reset() {
  this->time = this->max_time;
}
