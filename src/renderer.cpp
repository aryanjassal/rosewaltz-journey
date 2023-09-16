#include "renderer.h"
#include <string>

Renderer::Renderer(Shader &shader, Camera *camera) {
  this->shader = shader;
  this->camera = camera;

  // Set up the vertices
  unsigned int vbo;

  // Set vertices, color, and texture coordinates of a square
  // The sprites by default have origin at top-left corner
  float vertices[] = {
  /* VERTEX COORDINATES \\\ TEXTURE COORDINATES  */
    0.0f, 0.0f,              0.0f, 0.0f,
    0.0f, 1.0f,              0.0f, 1.0f,
    1.0f, 1.0f,              1.0f, 1.0f,
    1.0f, 0.0f,              1.0f, 0.0f
  };

  // Set the index order of the vertices
  // Note that the number of indices reflect the number of draw calls to be passed in glDrawElements()
  unsigned int indices[] = {
    0, 1, 2,
    0, 3, 2
  };

  // Initialise the VAO, VBO, and EBO
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &this->ebo);

  // Bind the VAO to prepare for everthing else
  glBindVertexArray(this->vao);

  // Bind the VBO and set some options for it
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Bind the EBO and set some options for it
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Edit some options for the VAOs after modifying both the EBO and the VBO
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Free up memory by unbinding the VAO, VBO, and the EBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Delete the VBO buffer as it is no longer needed
  glDeleteBuffers(1, &vbo);

  // Apply the projection and the view matrix to the camera
  this->shader.activate();
  this->shader.set_matrix_4f("projection", camera->projection_matrix());
  this->shader.set_matrix_4f("view", camera->view_matrix());
}

Renderer::~Renderer() {
  // This runs whenever the sprite renderer is destroyed, so it destroys itself automatically.
  glDeleteVertexArrays(1, &this->vao);
  glDeleteBuffers(1, &this->ebo);
}

void Renderer::render(Texture texture, Transform transform, glm::vec4 colour) {
  // Is the position in range of the camera?
  // if (this->camera->in_range(transform.position, transform.z)) {
  //   std::string warning = "Object's transform (";
  //   warning += std::to_string(transform.position.x);
  //   warning += ", ";
  //   warning += std::to_string(transform.position.y);
  //   warning += ", ";
  //   warning += std::to_string(transform.z);
  //   warning += ") is outside the camera's range (";
  //   warning += std::to_string(camera->near);
  //   warning += ", ";
  //   warning += std::to_string(camera->far);
  //   warning += ")";
  //   // warn("Object's transform (" + std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ", " + std::to_string(transform.z) + ") is out of the camera's range.");
  //   warn(warning);
  // }

  // Active this shader before starting the rendering process
  this->shader.activate();

  // Create a model transformation matrix and apply any origin transformations, if any
  glm::mat4 model_transform = glm::mat4(1.0f);

  // Apply transformations and translations to the sprite
  // Note that first the object is scaled, then rotated, then translated. However, these transformations
  // need to be listed in reverse order

  // Translate the object
  model_transform = glm::translate(model_transform, glm::vec3(transform.position, 0.0f));

  // Change the origin to the center before rotating the sprite
  model_transform = glm::translate(model_transform, glm::vec3(0.5f, 0.5f, 0.0f));
  model_transform = glm::rotate(model_transform, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
  model_transform = glm::translate(model_transform, glm::vec3(-0.5f, -0.5f, 0.0f));

  // Properly scale the object
  model_transform = glm::scale(model_transform, glm::vec3(transform.scale, 0.0f));

  // Actually apply these transformations and set other matrices in the shader
  this->shader.set_matrix_4f("model", model_transform);
  this->shader.set_matrix_4f("view", camera->view_matrix());
  this->shader.set_vector_4f("colour", colour);

  // Prepare the texture
  glActiveTexture(GL_TEXTURE0);
  texture.bind();

  // Bind the VAO, render the sprite, then unbind the VAO
  glBindVertexArray(this->vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
