#include "game.h"

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
  Renderer = new SpriteRenderer(sprite_shader, GameCamera);

  // Load textures into the game
  ResourceManager::Texture::load("textures/windows-11.png", true, "windows-icon");

  // Set up the game objects
  GameObject::create("windows1", GameCamera, ResourceManager::Texture::get("windows-icon"), glm::vec2(100.0f), glm::vec2(100.0f), glm::vec2(100.0f));
  GameObject::create("windows2", GameCamera, ResourceManager::Texture::get("windows-icon"), glm::vec2(200.0f), glm::vec2(100.0f), glm::vec2(100.0f));
}

void Game::update() {
  // Poll GLFW for new events
  mouse.buttons.left_button_down = false;
  mouse.buttons.left_button_up = false;
  if (!mouse.buttons.left_button) mouse.active_object = nullptr;
  glfwPollEvents();
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT);

  // If an object is selected, then move the object along with the mouse
  if (mouse.active_object != nullptr) {
    mouse.active_object->translate_to_point(glm::vec2(mouse.x, mouse.y));
  }

  // If we just released the mouse button and there is an object currently selected,
  // then reset the snap to 100.0f (temporary value) and update the position
  if (mouse.buttons.left_button_up && mouse.active_object != nullptr) {
    mouse.active_object->snap = glm::vec2(100.0f, 100.0f);
    mouse.active_object->update_position();
  }

  // Loop over every game object and check if the object is interactive and if the mouse intersects with it
  // If it does, set the snap to zero for smooth movement and make the current object focused
  // Then, render each object
  for (auto &object : GameObject::all()) {
    if (mouse.buttons.left_button && mouse.buttons.left_button_down && object->interactive && object->check_point_intersection(glm::vec2(mouse.x, mouse.y))) {
      object->snap = glm::vec2(0.0f);
      mouse.active_object = object;
    }
    object->render(Renderer);
  }
}

void Game::update_viewport(int width, int height) {
  this->width = width;
  this->height = height;
}