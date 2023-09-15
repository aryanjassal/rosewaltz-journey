#include "tutils.h"
#include <iostream>
#include <sstream>

void error(std::string message, bool fail) {
  printf("[ERROR] %s\n", message.c_str());  
  if (fail) exit(-1);
}

void warn(std::string message, bool fail) {
  printf("[WARN] %s\n", message.c_str());  
  if (fail) exit(-1);
}

void info(std::string message, bool fail) {
  printf("[INFO] %s\n", message.c_str());  
  if (fail) exit(-1);
}

float round(float val, float nearest = 1) {
  return std::round(val / nearest) * nearest;
}

const char *vtos(glm::vec2 vec, double p) {
  return std::format("({}, {})", round(vec.x, p), round(vec.y, p)).c_str();
}

const char *vtos(glm::vec3 vec, double p) {
  return std::format("({}, {}, {})", round(vec.x, p), round(vec.y, p), round(vec.z, p)).c_str();
}

const char *vtos(glm::vec4 vec, double p) {
  return std::format("({}, {}, {}, {})", round(vec.x, p), round(vec.y, p), round(vec.z, p), round(vec.w, p)).c_str();
}
