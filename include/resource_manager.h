#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "glad/gl.h"
#include "stb/stb_image.h"

#include "shader.h"
#include "texture.h"

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>

// A namespace hosting a myriad of functions related to managing game assets
// and resources during runtime. Each resource is stored for future use using
// a unique string handle.
namespace ResourceManager {
  static std::map<std::string, ::Shader>  Shaders;
  static std::map<std::string, ::Texture> Textures;
  
  // Manages all shaders used by the program
  namespace Shader {
    // Loads the vertex and fragment shader files to create a new shader in the resource manager
    ::Shader load(const char *vertex_shader_path, const char *fragment_shader_path, std::string handle);

    // Fetches a shader using the given handle
    ::Shader get(std::string handle);

    // Deallocates a shader with the given handle
    void deallocate(std::string handle);
  }

  // Manages all textures used by the program
  namespace Texture {
    // Loads an image from a file, creating a new texture object in the resource manager
    ::Texture load(const char *file_path, bool alpha, std::string handle);

    // Fetches a texture using the given handle
    ::Texture get(std::string handle);

    // Deallocates a texture with the given handle
    void deallocate(std::string handle);
  }

  // Manages loading and deallocating image files.
  // ResourceManager::Texture uses functions from this namespace.
  namespace Image {
    // Load raw pixel data from an image. Do not track it in the resource manager. 
    unsigned char *load(const char *file_path, int &width, int &height, int &colour_channels);
    unsigned char *load(const char *file_path);

    // Delete any hogged memory by removing image data no longer required
    void deallocate(unsigned char *image_data);
  }

  // Deallocates all resources being managed by the program, freeing up memory.
  void deallocate();
}

#endif