#ifndef __TERMINAL_UTILS_H__
#define __TERMINAL_UTILS_H__

#include <string>
#include <cstdio>
#include <format>

#include "glm/glm.hpp"

void error(std::string message, bool fail = true);
void warn(std::string message, bool fail = false);
void info(std::string message, bool fail = false);

#endif
