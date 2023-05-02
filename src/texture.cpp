#include "texture.h"

// This is a different way to set variable defaults
Texture::Texture() :
  width(0), 
  height(0), 
  texture_format(GL_RGB), 
  image_format(GL_RGB), 
  wrap_s(GL_CLAMP_TO_EDGE), 
  wrap_t(GL_CLAMP_TO_EDGE), 
  filter_min(GL_LINEAR), 
  filter_max(GL_LINEAR) {
    glGenTextures(1, &this->id);
}

void Texture::generate(unsigned int width, unsigned int height, unsigned char *data) {
  // Set the texture width and height of the texture
  this->width = width;
  this->height = height;
  
  // Actually generate the texture
  glBindTexture(GL_TEXTURE_2D, this->id);
  glTexImage2D(GL_TEXTURE_2D, 0, this->texture_format, width, height, 0, this->image_format, GL_UNSIGNED_BYTE, data);

  // //? Should I generate the mipmaps or not?
  // glGenerateMipmap(GL_TEXTURE_2D);

  // Set texture wrap and filter settings
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Unbind the texture
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, this->id);
}