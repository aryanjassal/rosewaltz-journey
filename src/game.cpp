#include "game.h"

//!TEMP
#include <stdio.h>
bool selected = false;
glm::vec2 position = glm::vec2(100.0f, 100.0f);

// Set up pointers to global objects for the game
SpriteRenderer *Renderer;
Camera::OrthoCamera *GameCamera;

Game::Game(unsigned int width, unsigned int height) {
  // Set internal width and height
  this->width = width;
  this->height = height;
}

Game::~Game() {
  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();
  delete Renderer;
}

void Game::init() {
  // Set the clear colour of the scene background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

  // Create a shader program, providing the vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "sprite");

  // Create the camera
  GameCamera = new Camera::OrthoCamera(this->width, this->height, -1.0f, 1.0f);

  // Actually create a sprite renderer instance
  Renderer = new SpriteRenderer(sprite_shader, *GameCamera);

  // Load textures into the game
  ResourceManager::Texture::load("textures/windows-11.png", true, "windows-icon");
}

void Game::update() {
  // Poll GLFW for new events
  mouse.buttons.left_button_down = false;
  mouse.buttons.left_button_up = false;
  glfwPollEvents();
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT);

  // Render a sprite at the location specified by scaling the mouse position with the camera's scale factor
  Texture texture = ResourceManager::Texture::get("windows-icon");
  glm::vec2 scale = glm::vec2(100.0f, 100.0f);

  int right = position.x + ((texture.width / 2) / (texture.width / scale.x));
  int left = position.x - ((texture.width / 2) / (texture.width / scale.x));
  int bottom = position.y + ((texture.height / 2) / (texture.height / scale.y));
  int top = position.y - ((texture.height / 2) / (texture.height / scale.y));

  if (!mouse.buttons.left_button) selected = false;
  if (mouse.buttons.left_button && mouse.buttons.left_button_down && ((left <= mouse.x) && (right >= mouse.x) && (top <= mouse.y) && (bottom >= mouse.y))) {
    selected = true;
  }

  if (selected) {
    position = glm::vec2(mouse.x / GameCamera->scale_factor.x, mouse.y / GameCamera->scale_factor.y);
  } else {
    float snap = 100;
    position = glm::vec2(round(position.x / snap) * snap, round(position.y / snap) * snap);
  }
  Renderer->render(ResourceManager::Texture::get("windows-icon"), position, scale);
}