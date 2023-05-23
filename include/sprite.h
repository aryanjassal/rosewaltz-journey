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
  glm::vec3 position;
  glm::vec2 scale;
  float rotation;
};

class SpriteRenderer {
  public:
    // The constructor initialises the shader and the shape of the sprite
    SpriteRenderer(Shader &shader, Camera::OrthoCamera *camera);

    // The deconstructor ensures that the memory is freed and that the sprite is deleted properly
    ~SpriteRenderer();

    // Actually render the sprite to the screen
    // Note that only an orthographic camera is supported for now.
    //! The program unefficiently resets the projection and view matrices every frame, when once is enough. Fix this
    void render(
      Texture texture, 
      Transform transform,
      glm::vec3 colour = glm::vec3(1.0f)
    ); 

  private:
    Shader shader;
    Camera::OrthoCamera *camera;
    unsigned int vao, ebo;
};

#endif
