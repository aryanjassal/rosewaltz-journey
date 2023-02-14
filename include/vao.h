#ifndef __VAO_CLASS_H__
#define __VAO_CLASS_H__

#include "glad/gl.h"
#include "vbo.h"

class VAO {
  public:
    GLuint id;
    VAO();

    void link_attrib(VBO& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset);
    void bind();
    void unbind();
    void remove();
};

#endif