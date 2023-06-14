#ifndef __FONT_H__
#define __FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdio>

#include "glm/glm.hpp"

// Struct storing character information
typedef struct Character {
  unsigned int texture_id;  // The id of the glyph
  glm::ivec2 size;          // The physical size of the glyph
  glm::ivec2 bearing;       // Offset from baseline to top-left of the glyph
  unsigned int advance;     // Offset to advance to the next glyph
};

// Struct to generally deal with fonts
namespace Fonts {
  // Has the library been initialised?
  // WARNING: This variable should never be written to manually.
  extern bool initialised;

  // Refering the intialised font variable
  extern FT_Library lib;

  void init();
}

#endif
