#ifndef __VBO_CLASS_H__
#define __VBO_CLASS_H__

#include "glad/gl.h"

class VBO {
  public:
    GLuint id;
    VBO(GLfloat* vertices, GLsizeiptr size);

    void bind();
    void unbind();
    void remove();
};

#endif