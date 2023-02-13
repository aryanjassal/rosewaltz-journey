#include "texture.h"

Texture::Texture(const char* image, GLenum texture_type, GLenum slot, GLenum format, GLenum pixel_type) {
  // Set the type of texture
  type = texture_type;
  
  // Load an image as raw bytes
  int img_width, img_height, img_color_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* bytes = stbi_load(image, &img_width, &img_height, &img_color_channels, 0);

  // Convert the loaded image into a texture
  glGenTextures(1, &id);
  glActiveTexture(slot);
  glBindTexture(texture_type, id);

  // Set some texture parameters
  glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Actually generate the texture
  glTexImage2D(texture_type, 0, GL_RGBA, img_width, img_height, 0, format, pixel_type, bytes);
  glGenerateMipmap(texture_type);

  // Clean up the memory used by loading the texture
  stbi_image_free(bytes);
  glBindTexture(texture_type, 0);
}

void Texture::texture_unit(Shader shader, const char* uniform, GLuint unit) {
  // Use uniform value from the fragment shader
  GLuint texture_uniform = glGetUniformLocation(shader.id, uniform);
  shader.activate();
  glUniform1i(texture_uniform, unit);
}

void Texture::bind() {
  glBindTexture(type, id);
}

void Texture::unbind() {
  glBindTexture(type, 0);
}

void Texture::remove() {
  glDeleteTextures(1, &id);
}