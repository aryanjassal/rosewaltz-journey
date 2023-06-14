#include "font.h"

// Set a few global variables
bool Fonts::initialised = false;

// Declare global font-related variables
FT_Library Fonts::lib;

void Fonts::init() {
  if (FT_Init_FreeType(&Fonts::lib)) {
    printf("%s\n", "ERROR::FREETYPE: Could not init FreeType Library");
  }
  initialised = true;
}

