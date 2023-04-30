#ifndef __SHADER_H__
#define __SHADER_H__

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>

class Shader {
  public:
    // The ID of the shader
    unsigned int id;

    // For std::map to work (see resource_manager.h), the constructor needs to take no parameters
    Shader() { }

    // Compile and link the vertex and fragment code to the shader
    void compile(const char *vertex_code, const char *fragment_code);

    // Activate our shader
    void activate();

    // Modify a uniform in the shader
    void set_float      (const char *handle, float value);
    void set_integer    (const char *handle, int value);
    void set_vector_2f  (const char *handle, float x, float y);
    void set_vector_2f  (const char *handle, const glm::vec2 &value);
    void set_vector_3f  (const char *handle, float x, float y, float z);
    void set_vector_3f  (const char *handle, const glm::vec3 &value);
    void set_vector_4f  (const char *handle, float x, float y, float z, float w);
    void set_vector_4f  (const char *handle, const glm::vec4 &value);
    void set_matrix_4f  (const char *handle, const glm::mat4 &value);

  private:
    // Checks the shader files and prints out errors if any are found
    void compile_errors(unsigned int shader, const char *type);
};

#endif