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
