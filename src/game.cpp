#include "game.h"
#include "object.h"
#include <GLFW/glfw3.h>

// Set up pointers to global objects for the game
SpriteRenderer *Renderer;
Camera::OrthoCamera *GameCamera;

Game::Game(unsigned int width, unsigned int height, std::string window_title) {
  // Set internal width and height
  this->width = width;
  this->height = height;
  this->GameTitle = window_title;

  // Initialise GLFW
  if (!glfwInit()) {
    printf("ERROR: GLFW failed to initialise!\n");
  }

  // Create a window for the game
  this->fullscreen = false;
  set_window_hints();
  create_window(this->GameWindow);

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
  glfwDestroyWindow(this->GameWindow);
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

  // // Set an icon for the game
  // GLFWimage icons[1];
  // icons[0].pixels = ResourceManager::Image::load("icon.png");
  // glfwSetWindowIcon(this->GameWindow, sizeof(icons), icons);
  // for (auto icon : icons) ResourceManager::Image::deallocate(icon.pixels);

  // Load textures into the game
  ResourceManager::Texture::load("textures/gigachad.jpg", true, "gigachad");
  ResourceManager::Texture::load("textures/windows-11.png", true, "windows");

  // Set up the game objects
  glm::vec2 w_dimensions = glm::vec2(this->width, this->height);
  // glm::vec2 grid = glm::vec2(this->width / 3, this->height / 2);
  glm::vec2 grid = glm::vec2(100.0f);
  GameObject::create("gigachad", GameCamera, ResourceManager::Texture::get("gigachad"), w_dimensions, glm::vec2(0.0f), glm::vec2(100.0f), grid);
  GameObject::create("windows", GameCamera, ResourceManager::Texture::get("windows"), w_dimensions, glm::vec2(100.0f), glm::vec2(100.0f), grid);

  GameObject::ObjectGroup *tile1 = GameObject::create_group("tile1");
  tile1->add("gigachad", GameObject::get("gigachad"));
  tile1->add("windows", GameObject::get("windows"));
  for (auto &object : tile1->all()) {
    object->interactive = false;
    printf("[%s] interactivity of object [%s] set to '%s'\n", tile1->handle.c_str(), object->handle.c_str(), tile1->get(object->handle)->interactive ? "true" : "false");
    // tile1->update(object->handle, *object);
  }
  GameObject::update("tile1", tile1);
}

void Game::run() {
  // Main events loop
  while(!glfwWindowShouldClose(this->GameWindow)) {
    this->render();
    this->update();

    // Swap the buffers to actually render what we are drawing to the screen
    glfwSwapBuffers(this->GameWindow);
  }
}

void Game::update() {
  // Reset the pressed and released status of the mouse buttons
  //TODO: Why does this not work inside the input handler? Can it even be here after the delta-time update?
  MouseState.buttons.left_button_down = false;
  MouseState.buttons.left_button_up = false;
  
  // If left click has been released, then there should be no active object
  if (!MouseState.buttons.left_button) MouseState.active_object = nullptr;

  // If the escape key was pressed, then close the window
  if (KeyboardState[256].pressed) glfwSetWindowShouldClose(this->GameWindow, true);

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
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT);

  // If an object is selected, then move the object along with the mouse
  if (MouseState.active_object != nullptr) {
    MouseState.active_object->translate_to_point(glm::vec2(MouseState.x, MouseState.y), glm::vec2(0.5f));
  }

  // If we just released the mouse button and there is an object currently selected,
  // then reset the snap to 100.0f (temporary value) and update the position
  if (MouseState.buttons.left_button_up && MouseState.active_object != nullptr) {
    MouseState.active_object->snap = true;
    MouseState.active_object->update_snap_position();
  }

  // Loop over every game object and check if the object is interactive and if the mouse intersects with it
  // If it does, set the snap to zero for smooth movement and make the current object focused
  // Then, render each object
  for (auto &object : GameObject::all()) {
    if (MouseState.buttons.left_button && MouseState.buttons.left_button_down && object->interactive && object->check_point_intersection(glm::vec2(MouseState.x, MouseState.y), glm::vec2(0.5f))) {
      object->snap = false;
      MouseState.active_object = object;
    }
    // object->render(Renderer);
  }

  auto tile = GameObject::get_group("tile1");
  tile->render(Renderer);

  // If the F key is pressed, toggle fullscreen
  if (this->KeyboardState['F'].pressed) toggle_fullscreen();
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

void Game::create_window(GLFWwindow *&window) {
  // Create a GLFW window
  window = glfwCreateWindow(width, height, this->GameTitle.c_str(), this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
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
  glfwSetCursorPosCallback(this->GameWindow, cursorpos_callback);
  glfwSetMouseButtonCallback(this->GameWindow, cursorbutton_callback);
  glfwSetKeyCallback(this->GameWindow, keyboard_callback);
}

void Game::toggle_fullscreen() {
  // Actually toggle the fullscreen mode
  this->fullscreen = !this->fullscreen;

  // Get current window dimensions
  int width = this->width;
  int height = this->height;

  // If the window mode is fullscreen, then actually apply the fullscreen window dimensions
  if (this->fullscreen) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width;
    height = mode->height;
  }

  // Update the OpenGL viewport
  glViewport(0, 0, width, height);

  // Here, the command `*Camera->resize(width, height);` must be run for a Camera object if the objects are expected to
  // have absolute sizes irrespective of the screen size. If the objects are expected to keep the same size regardless of
  // the window size, then resize the camera's matrices here.
  for (auto &object : GameObject::all()) {
    object->window_dimensions = glm::vec2(width, height);
    GameObject::update(object->handle, *object);
  }

  // Toggle the window fullscreen state
  glfwSetWindowMonitor(this->GameWindow, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, 0);
}
