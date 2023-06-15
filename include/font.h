#ifndef __FONT_H__
#define __FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdio>
#include <string>
#include <map>
#include <stdexcept>

#include "glm/glm.hpp"

#include "shader.h"
#include "utils.h"
#include "camera.h"

// Struct storing character information
typedef struct Character {
  unsigned int texture_id;  // The id of the glyph
  glm::ivec2 size;          // The physical size of the glyph
  glm::ivec2 bearing;       // Offset from baseline to top-left of the glyph
  unsigned int advance;     // Offset to advance to the next glyph
};

extern std::map<std::string, std::map<char, Character>> CharacterLookup;
extern Shader TextShader; 
extern OrthoCamera *TextCamera;

// Namespace to generally deal with text rendering
namespace Text {
  void render(std::string str, const char *font, Transform transform, glm::vec4 colour = glm::vec4(1.0f));
}

// Namespace to deal with font management
namespace Fonts {
  void init(FT_Library &ft);
}

#endif
