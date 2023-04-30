#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "shader.h"
#include "texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"

class Sprite {
  public:
    // The constructor initialises the shader and the shape of the sprite
    Sprite(Shader &shader);

    // The deconstructor ensures that the memory is freed and that the sprite is deleted properly
    ~Sprite();

    // Actually render the sprite to the screen
    void render(Texture texture, glm::vec2 position = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(1.0f), float angle = 0.0f, glm::vec3 colour = glm::vec3(1.0f));

  private:
    Shader shader;
    unsigned int vao, ebo;
};

#endif