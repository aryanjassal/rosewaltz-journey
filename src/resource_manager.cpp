#include "resource_manager.h"

::Shader ResourceManager::Shader::load(const char *vertex_shader_path, const char *fragment_shader_path, std::string handle) {
  std::string vertex_shader_code, fragment_shader_code;

  try {
    // Open input file streams to read file contents
    std::ifstream vertex_shader_file(vertex_shader_path);
    std::ifstream fragment_shader_file(fragment_shader_path);

    // Prepare string streams to store the file buffer contents
    std::stringstream vertex_shader_stream, fragment_shader_stream;

    // Read the file by copying the file buffeer to the string stream
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();

    // Close the shader files to free up memory
    vertex_shader_file.close();
    fragment_shader_file.close();

    // Convert the string streams to usable code
    vertex_shader_code = vertex_shader_stream.str();
    fragment_shader_code = fragment_shader_stream.str();
  } catch (std::exception e) {
    std::cout << "ERROR: could not read shader files" << std::endl;
  }

  // Create a shader with the given vertex and fragment shader and add it to the resource manager
  ::Shader shader;
  const char *vertex_shader = vertex_shader_code.c_str();
  const char *fragment_shader = fragment_shader_code.c_str();
  shader.compile(vertex_shader, fragment_shader);
  Shaders[handle] = shader;
  return shader;
}

::Shader ResourceManager::Shader::get(std::string handle) {
  return Shaders[handle];
}

void ResourceManager::Shader::deallocate(std::string handle) {
  glDeleteProgram(Shaders[handle].id);
}

::Texture ResourceManager::Texture::load(const char *file_path, bool alpha, std::string handle) {
  ::Texture texture;

  // If alpha channels are required, then reflect this in the texture options
  if (alpha) {
    texture.texture_format = GL_RGBA;
    texture.image_format = GL_RGBA;
  }

  // Load the image file as raw bytes
  int width, height, color_channels;
  unsigned char* data = stbi_load(file_path, &width, &height, &color_channels, 0);

  // Generate the texture
  texture.generate(width, height, data);

  // Clean up the memory used by loading the texture
  stbi_image_free(data);

  // Add the texture to the resource manager and return the texture
  Textures[handle] = texture;
  return texture;
}

::Texture ResourceManager::Texture::get(std::string handle) {
  return Textures[handle];
}

void ResourceManager::Texture::deallocate(std::string handle) {
  glDeleteTextures(1, &Textures[handle].id);
}

void ResourceManager::deallocate() {
  // Deallocate all shaders
  for (auto shader : Shaders) {
    glDeleteProgram(shader.second.id);
  }

  // Deallocate all textures
  for (auto texture : Textures) {
    glDeleteTextures(1, &texture.second.id);
  }
}