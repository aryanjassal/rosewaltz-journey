#ifndef __VAO_CLASS_H__
#define __VAO_CLASS_H__

#include "glad/gl.h"
#include "vbo.h"

class VAO {
  public:
    GLuint id;
    VAO();

    void link_vbo(VBO vbo, GLuint layout);
    void bind();
    void unbind();
    void remove();
};

#endif