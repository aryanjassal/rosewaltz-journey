#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <vector>

#include "glad/glad.h"
#include "stb/stb_image.h"

class Texture {
  public:
    // Holds the ID of the texture, which is how it will be referenced in the future
    unsigned int id;

    // Holds the dimensions of the texture
    unsigned int width, height;

    // Sets the pixel formatting for the texture
    unsigned int texture_format, image_format;

    // Miscellaneous texture settings
    unsigned int wrap_s;        // Wrapping on the S axis
    unsigned int wrap_t;        // Wrapping on the T axis
    unsigned int filter_min;    // Filtering mode if texture pixels < screen pixels
    unsigned int filter_max;    // Filtering mode if texture pixels > screen pixels

    // The texture constructor which assigns a default value to every option
    Texture();

    // Generate the texture given image width, height, and pixel data
    void generate(unsigned int width, unsigned int height, unsigned char *data);

    // Binds the texture as current active GL_TEXTURE_2D texture object
    void bind() const;
};

typedef struct TextureMap {
  // Constructors for the struct and their default values
  TextureMap():
    textures{Texture()}, animate{false}, index(0), time_per_frame{0.0f}, timer(0.0f) { }
  TextureMap(Texture texture, bool animate = false, float time_per_frame_milliseconds = 100.0f):
    textures{texture}, animate{animate}, index(0), time_per_frame{time_per_frame_milliseconds}, timer(0.0f) { }
  TextureMap(std::vector<Texture> texture, bool animate = false, float time_per_frame_milliseconds = 100.0f):
    textures{texture}, animate{animate}, index(0), time_per_frame{time_per_frame_milliseconds}, timer(0.0f) { }

  // Get the current texture
  Texture texture() {
    return this->textures.at(this->index);
  }

  // Process the next frame for the texture
  void tick(double delta_milliseconds) {
    if (!this->animate) return;

    this->timer += delta_milliseconds;
    if (this->timer >= time_per_frame) {

      // NOTE: testing needed here
      this->index = (this->index + 1) % this->textures.size();
      this->timer = 0.0f;
    }
  }

  // Values of the texture map, which supports animation
  std::vector<Texture> textures;
  int index;
  float time_per_frame;
  float timer;
  bool animate;
};

#endif
