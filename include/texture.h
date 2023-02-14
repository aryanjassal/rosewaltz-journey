#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "glad/gl.h"
#include "stb/stb_image.h"
#include "shaders.h"

class Texture {
  public:
    GLuint id;
    GLenum type;
    Texture(const char* image, GLenum texture_type, GLenum slot, GLenum format, GLenum pixel_type);

    void texture_unit(Shader& shader, const char* uniform, GLuint unit);
    void bind();
    void unbind();
    void remove();
};

#endif