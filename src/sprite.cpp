#include "sprite.h"

Sprite::Sprite(Shader &shader) {
  this->shader = shader;

  // Set up the vertices
  unsigned int vbo;

  // // Set vertices, color, and texture coordinates of a square
  // // The sprites by default have origin at middle-center
  // float vertices[] = {
  // /* VERTEX COORDINATES \\\ TEXTURE COORDINATES  */
  //   -0.5f, -0.5f,              0.0f, 0.0f,
  //   -0.5f,  0.5f,              0.0f, 1.0f,
  //    0.5f,  0.5f,              1.0f, 1.0f,
  //    0.5f, -0.5f,              1.0f, 0.0f
  // };

  float vertices[] = { 
    // pos      // tex
    -0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 

    -0.5f, 0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 1.0f, 0.0f
  };

  // unsigned int indices[] = {
  //   0, 2, 1,
  //   0, 3, 2
  // };

  // Initialise the VAO, VBO, and EBO
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &vbo);
  // glGenBuffers(1, &this->ebo);

  // Bind the VBO and set some options for it
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // // Bind the EBO and set some options for it
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Actually use the VAO and the VBO to configure the sprite rendering
  glBindVertexArray(this->vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  // Free up memory by unbinding the VAO, VBO, and the EBO
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // // Delete the VBO buffer as it is no longer needed
  // //! Need to confirm whether this should be here or not
  // glDeleteBuffers(1, &vbo);
}

Sprite::~Sprite() {
  // This runs whenever the sprite is destroyed, so it destroys itself automatically.
  glDeleteVertexArrays(1, &this->vao);
  // glDeleteBuffers(1, &this->ebo);
}

void Sprite::render(Texture texture, glm::vec2 position, glm::vec2 scale, float angle, glm::vec3 colour) {
  // Activate the shader to actually do the model transformations
  this->shader.activate();

  // Create a model transformation matrix
  glm::mat4 model_transform = glm::mat4(1.0f);

  // Apply transformations and translations to the sprite
  // Note that first the object is scaled, then rotated, then translated. However, these transformations
  // need to be listed in reverse order (probably an internal stack)
  model_transform = glm::translate(model_transform, glm::vec3(position, 0.0f));
  model_transform = glm::rotate(model_transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
  model_transform = glm::scale(model_transform, glm::vec3(scale, 0.0f));

  // Actually apply these transformations to the sprite
  this->shader.set_matrix_4f("model", model_transform);

  // Render the textured sprite
  this->shader.set_vector_3f("colour", colour);
  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  glBindVertexArray(this->vao);
  // glDrawArrays(GL_TRIANGLES, 0, 4);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}