#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "glad/gl.h"
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

#endif