#include "resource_manager.h"

//TODO: For deallocation, also remove the reference to their handle and everything from the map, as even
//TODO: the memory has been freed, there are some pointers still referencing what it used to be.

std::map<std::string, ::Shader> Shaders;
std::map<std::string, ::Texture> Textures;

// std::map<char, Character> ResourceManager::Font::Characters;

void ResourceManager::Font::load(const char *path, std::string font_name, unsigned int num_chars, short filtering) {
  // Declare the freetype library and fontface variables
  FT_Library freetype;

  Fonts::init(freetype);

  FT_Face face;
  if (FT_New_Face(freetype, path, 0, &face)) {
    printf("[ERROR] ResourceManager failed to load font from file %s\n", path);  
    return;
  }

  // Set global glyph sizes
  FT_Set_Pixel_Sizes(face, 0, 48);

  // Disable byte-alignment restriction
  // NOTE: Without this, the code can SEG_FAULT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   

  // Load each character glyph from the font until the num_chars limit is reached 
  for (unsigned char ch = 0; ch < num_chars; ch++) {
    if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
      printf("[ERROR] ResourceManager failed to load glyph '%c' from font '%s'\n", ch, font_name.c_str()); 
      continue;
    }

    // Generate the texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

    // Set the texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (filtering == FILTER_NEAREST) ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering == FILTER_NEAREST) ? GL_NEAREST : GL_LINEAR);

    // Now store character for later use
    Character character = {
      texture, 
      glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      static_cast<unsigned int>(face->glyph->advance.x)
    };

    CharacterLookup[font_name].insert(std::pair<char, Character>(ch, character));
  }

  FT_Done_Face(face);
  FT_Done_FreeType(freetype);
}

::Shader ResourceManager::Shader::load(const char *vertex_shader_path, const char *fragment_shader_path, const char *geometry_shader_path, std::string handle) {
  std::string vertex_shader_code, fragment_shader_code, geometry_shader_code;

  try {
    // Open input file streams to read file contents
    std::ifstream vertex_shader_file(vertex_shader_path);
    std::ifstream fragment_shader_file(fragment_shader_path);
    std::ifstream geometry_shader_file(geometry_shader_path);

    // Prepare string streams to store the file buffer contents
    std::stringstream vertex_shader_stream, fragment_shader_stream, geometry_shader_stream;

    // Read the file by copying the file buffeer to the string stream
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();
    geometry_shader_stream << geometry_shader_file.rdbuf();

    // Close the shader files to free up memory
    vertex_shader_file.close();
    fragment_shader_file.close();
    geometry_shader_file.close();

    // Convert the string streams to usable code
    vertex_shader_code = vertex_shader_stream.str();
    fragment_shader_code = fragment_shader_stream.str();
    geometry_shader_code = geometry_shader_stream.str();
  } catch (std::exception e) {
    printf("[ERROR] Could not read the shader files!\n");
  }

  // Create a shader with the given vertex and fragment shader and add it to the resource manager
  ::Shader shader;
  const char *vertex_shader = vertex_shader_code.c_str();
  const char *fragment_shader = fragment_shader_code.c_str();
  const char *geometry_shader = geometry_shader_code.c_str();
  shader.compile(vertex_shader, fragment_shader, geometry_shader);
  Shaders[handle] = shader;
  return shader;
}

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
    printf("[ERROR] Could not read the shader files!\n");
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

  // Load the requested image as raw bytes
  int width, height, colour_channels;
  unsigned char *data = ResourceManager::Image::load(file_path, width, height, colour_channels);

  // Generate the texture
  texture.generate(width, height, data);

  // Clean up the memory used by loading the texture
  ResourceManager::Image::deallocate(data);

  // Add the texture to the resource manager and return the texture
  Textures[handle] = texture;
  return texture;
}

::Texture ResourceManager::Texture::get(std::string handle) {
  if (Textures.find(handle) != Textures.end())
    return Textures[handle];
  else throw std::runtime_error("Texture '" + handle + "' doesn't exist!");
}

void ResourceManager::Texture::deallocate(std::string handle) {
  glDeleteTextures(1, &Textures[handle].id);
}

unsigned char *ResourceManager::Image::load(const char *file_path, int &width, int &height, int &colour_channels) {
  // Load the image file as raw bytes
  unsigned char* data = stbi_load(file_path, &width, &height, &colour_channels, 4);

  // If the pixel data is empty, then throw a runtime error
  if (!data) {
    std::string error_message = "Failure to load image \"";
    error_message.append(file_path);
    error_message.append("\"\n");

    printf("\n");
    if (stbi_failure_reason()) printf("[ERROR] stbi_failure on file \"%s\" [%s]\n", file_path, stbi_failure_reason());
    throw std::runtime_error(error_message);
  }

  return data;
}

unsigned char *ResourceManager::Image::load(const char *file_path) {
  int width, height, colour_channels;
  return ResourceManager::Image::load(file_path, width, height, colour_channels);
}

void ResourceManager::Image::deallocate(unsigned char *image_data) {
  stbi_image_free(image_data);
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
