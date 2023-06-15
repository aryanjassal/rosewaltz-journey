#include "font.h"

// Set global character lookup variable 
std::map<std::string, std::map<char, Character>> CharacterLookup;
Shader TextShader;
OrthoCamera *TextCamera = nullptr;

void Fonts::init(FT_Library &ft) {
  if (FT_Init_FreeType(&ft)) {
    printf("[ERROR] Freetype could not be initialised!\n");
  }
}

void Text::render(std::string str, const char *font, Transform transform, glm::vec4 colour) {
  if (TextCamera == nullptr) throw std::runtime_error("[ERROR] TextCamera is undefined!");

  unsigned int t_vao, t_vbo;
  glGenVertexArrays(1, &t_vao);
  glGenBuffers(1, &t_vbo);
  glBindVertexArray(t_vao);
  glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
  // model = glm::rotate(model, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
  // model = glm::translate(model, glm::vec3(-0.5f));

  // Activate corresponding rendering shader	
  TextShader.activate();
  // TextShader.set_vector_4f("text_colour", colour);
  TextShader.set_vector_4f("text_colour", colour);
  TextShader.set_matrix_4f("projection", TextCamera->projection_matrix);
  // TextShader.set_matrix_4f("model", model);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(t_vao);

  // iterate through all characters
  std::string::const_iterator c;
  for (c = str.begin(); c != str.end(); c++) {
    Character ch = CharacterLookup[font][*c];

    float xpos = transform.position.x + ch.bearing.x * transform.scale.x;
    float ypos = transform.position.y - ch.bearing.y * transform.scale.y;

    float w = ch.size.x * transform.scale.x;
    float h = ch.size.y * transform.scale.y;
    // update VBO for each character
    float vertices[6][4] = {
      { xpos,     ypos + h,   0.0f, 1.0f },            
      { xpos,     ypos,       0.0f, 0.0f },
      { xpos + w, ypos,       1.0f, 0.0f },

      { xpos,     ypos + h,   0.0f, 1.0f },
      { xpos + w, ypos,       1.0f, 0.0f },
      { xpos + w, ypos + h,   1.0f, 1.0f }           
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.texture_id);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    transform.position.x += (ch.advance >> 6) * transform.scale.x; // bitshift by 6 to get value in pixels (2^6 = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
