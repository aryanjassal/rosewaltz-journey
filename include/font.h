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

#define TEXT_TOP_LEFT 0
#define TEXT_TOP_CENTER 1
#define TEXT_TOP_RIGHT 2
#define TEXT_MIDDLE_LEFT 3
#define TEXT_MIDDLE_CENTER 4
#define TEXT_MIDDLE_RIGHT 5
#define TEXT_BOTTOM_LEFT 6
#define TEXT_BOTTOM_CENTER 7
#define TEXT_BOTTOM_RIGHT 8

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
  void render(std::string str, const char *font, Transform transform, short alignment = TEXT_TOP_LEFT, glm::vec4 colour = glm::vec4(1.0f));
}

// Namespace to deal with font management
namespace Fonts {
  void init(FT_Library &ft);
}

#endif
