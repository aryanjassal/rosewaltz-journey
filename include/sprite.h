#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "shader.h"
#include "texture.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glad/gl.h"

// Struct holding the transformations to be applied to the object
typedef struct Transform {
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec2 scale = glm::vec2(1.0f);
  float rotation = 0.0f;
};

class SpriteRenderer {
  public:
    // The constructor initialises the shader and the shape of the sprite
    SpriteRenderer(Shader &shader, Camera::OrthoCamera *camera);

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
    Camera::OrthoCamera *camera;
    unsigned int vao, ebo;
};

#endif
