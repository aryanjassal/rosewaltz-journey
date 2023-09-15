#ifndef __TERMINAL_UTILS_H__
#define __TERMINAL_UTILS_H__

#include <string>
#include <stdio.h>
#include <format>

#include "glm/glm.hpp"

void error(std::string message, bool fail = true);
void warn(std::string message, bool fail = false);
void info(std::string message, bool fail = false);

const char *vtos(glm::vec2 vec, double p = 1);
const char *vtos(glm::vec3 vec, double p = 1);
const char *vtos(glm::vec4 vec, double p = 1);

#endif
