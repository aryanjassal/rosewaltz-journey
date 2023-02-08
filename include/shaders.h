#ifndef __SHADERS_H__
#define __SHADERS_H__

#include "glad/gl.h"
#include <string>

std::string get_file_contents(const char* filename);

class Shader {
  public:
    GLuint id;
    Shader(const char* vertex_file, const char* fragment_file);

    void activate();
    void remove();
};

#endif