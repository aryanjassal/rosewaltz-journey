#include "game.h"
#include <GLFW/glfw3.h>

// Set up pointers to global objects for the game
SpriteRenderer *Renderer;
Camera::OrthoCamera *GameCamera;

Game::Game(unsigned int width, unsigned int height, std::string window_title) {
  // Set internal width and height
  this->width = width;
  this->height = height;
  this->GameTitle = window_title;

  // Initialise the mouse state
  this->MouseState = { 0, 0, { false, false, false }, nullptr };

  // Initialise GLFW
  if (!glfwInit()) {
    printf("ERROR: GLFW failed to initialise!\n");
  }

  // Create a window for the game
  set_window_hints();
  create_window(GameWindow, false);

  // Initialise OpenGL using GLAD
  gladLoadGL(glfwGetProcAddress);

  // Initialise the OpenGL viewport
  // In this case, the viewport goes from x=0 y=0 to x=width y=height (left to right, bottom to top)
  glViewport(0, 0, width, height);

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Game::~Game() {
  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();
  delete Renderer;

  // Clean up and close the game
  glfwDestroyWindow(GameWindow);
  glfwTerminate();
}

void Game::init() {
  // Set the clear colour of the scene background
  glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

  // Create a shader program, providing the vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "sprite");

  // Create the camera
  GameCamera = new Camera::OrthoCamera(this->width, this->height, -1.0f, 1.0f);

  // Create a sprite renderer instance
  Renderer = new SpriteRenderer(sprite_shader, GameCamera);

  // Load textures into the game
  ResourceManager::Texture::load("textures/windows-11.png", true, "windows-icon");

  // Set up the game objects
  GameObject::create("windows1", GameCamera, ResourceManager::Texture::get("windows-icon"), glm::vec2(100.0f), glm::vec2(100.0f), glm::vec2(100.0f));
  GameObject::create("windows2", GameCamera, ResourceManager::Texture::get("windows-icon"), glm::vec2(200.0f), glm::vec2(100.0f), glm::vec2(100.0f));
}

void Game::run() {
  // Main events loop
  while(!glfwWindowShouldClose(GameWindow)) {
    this->render();
    this->update();

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(GameWindow);
  }
}

void Game::update() {
  // Reset the pressed and released status of the mouse buttons
  //TODO: Why does this not work inside the input handler? Can it even be here after the delta-time update?
  MouseState.buttons.left_button_down = false;
  MouseState.buttons.left_button_up = false;
  
  // If left click has been released, then there should be no active object
  if (!MouseState.buttons.left_button) MouseState.active_object = nullptr;

  // Reset KeyState::pressed and KeyState::released
  //! This method is extremely unoptimised. The multiple loops need to be fixed for optimal performance.
  std::vector<int> released_keys;
  for (auto pair : this->KeyboardState) {
    if (pair.second.pressed) this->KeyboardState[pair.first].pressed = false;
    if (pair.second.released) released_keys.push_back(pair.first);
  }

  for (int key : released_keys) {
    this->KeyboardState.erase(this->KeyboardState.find(key));
  }

  // Poll GLFW for new events
  glfwPollEvents();

  // Print out debug keyboard info
  printf("\n");
  for (auto pair : this->KeyboardState) {
    printf("%i : %s, %s, %s\n", (char)pair.first, pair.second.pressed ? "true" : "false", pair.second.down ? "true" : "false", pair.second.released ? "true" : "false");
  }
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT);

  // If an object is selected, then move the object along with the mouse
  if (MouseState.active_object != nullptr) {
    MouseState.active_object->translate_to_point(glm::vec2(MouseState.x, MouseState.y));
  }

  // If we just released the mouse button and there is an object currently selected,
  // then reset the snap to 100.0f (temporary value) and update the position
  if (MouseState.buttons.left_button_up && MouseState.active_object != nullptr) {
    MouseState.active_object->snap = glm::vec2(100.0f, 100.0f);
    MouseState.active_object->update_position();
  }

  // Loop over every game object and check if the object is interactive and if the mouse intersects with it
  // If it does, set the snap to zero for smooth movement and make the current object focused
  // Then, render each object
  for (auto &object : GameObject::all()) {
    if (MouseState.buttons.left_button && MouseState.buttons.left_button_down && object->interactive && object->check_point_intersection(glm::vec2(MouseState.x, MouseState.y))) {
      object->snap = glm::vec2(0.0f);
      MouseState.active_object = object;
    }
    object->render(Renderer);
  }
}

void Game::update_viewport(int width, int height) {
  this->width = width;
  this->height = height;
}

void Game::set_window_hints() {
  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Use anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_RESIZABLE, false);
}

void Game::create_window(GLFWwindow *&window, bool fullscreen) {
  // Get current window dimensions
  if (!this->width && !this->height) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    this->width = mode->width;
    this->height = mode->height;
  }

  // Create a GLFW window
  window = glfwCreateWindow(this->width, this->height, "Rosewaltz Journey", fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // If the window does not exist, then something went wrong!
  if (window == NULL)  {
    printf("GLFW window failed to initialise!\n");
    glfwTerminate();
    exit(-1);
  }

  // Change some GLFW settings post-initialisation
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Game::set_callbacks(GLFWcursorposfun cursorpos_callback, GLFWmousebuttonfun cursorbutton_callback, GLFWkeyfun keyboard_callback) {
  glfwSetCursorPosCallback(GameWindow, cursorpos_callback);
  glfwSetMouseButtonCallback(GameWindow, cursorbutton_callback);
  glfwSetKeyCallback(GameWindow, keyboard_callback);
}
