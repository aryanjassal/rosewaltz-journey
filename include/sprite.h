#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"

class SpriteRenderer {
  public:
    // The constructor initialises the shader and the shape of the sprite
    SpriteRenderer(Shader &shader, OrthoCamera *camera);

    // The deconstructor ensures that the memory is freed and that the sprite is deleted properly
    ~SpriteRenderer();

    // Actually render the sprite to the screen.
    // Note that only an orthographic camera is supported for now.
    void render(
      Texture texture, 
      Transform transform,
      glm::vec4 colour = glm::vec4(1.0f)
    ); 

  private:
    Shader shader;
    OrthoCamera *camera;
    unsigned int vao, ebo;
};

#endif
