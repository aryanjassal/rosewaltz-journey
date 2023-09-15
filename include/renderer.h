#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "utils.h"
#include "tutils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/glad.h"

class Renderer {
  public:
    // The constructor initialises the shader and the shape of the sprite
    Renderer(Shader &shader, Camera *camera);

    // The deconstructor ensures that the memory is freed and that the sprite is deleted properly
    ~Renderer();

    // Render a sprite to the screen.
    void render(Texture texture, Transform transform, glm::vec4 colour = glm::vec4(1.0f)); 

  private:
    Shader shader;
    Camera *camera;
    unsigned int vao, ebo;
};

#endif
