#ifndef __SHADERS_H__
#define __SHADERS_H__

#include "glad/gl.h"
#include <string>
#include <fstream>
#include <stdio.h>

std::string get_file_contents(const char* filename);

class Shader {
  public:
    GLuint id;
    Shader(const char* vertex_file, const char* fragment_file);

    void activate();
    void remove();

  private:
    void compile_errors(unsigned int shader, const char* type);
};

#endif