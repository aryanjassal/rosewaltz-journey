#ifndef __EBO_CLASS_H__
#define __EBO_CLASS_H__

#include "glad/gl.h"

class EBO {
  public:
    GLuint id;
    EBO(GLuint* indices, GLsizeiptr size);

    void bind();
    void unbind();
    void remove();
};

#endif