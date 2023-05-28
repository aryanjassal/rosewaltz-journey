#include "game.h"

// Set up pointers to global objects for the game
SpriteRenderer *Renderer;
Camera::OrthoCamera *GameCamera;

Game::Game(unsigned int width, unsigned int height, std::string window_title, bool fullscreen) {
  // Set internal width and height
  this->width = width;
  this->height = height;
  this->GameTitle = window_title;
  this->fullscreen = fullscreen;

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

  // Initialise the game
  this->init();
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
  // glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Create a shader program, providing the vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "default");

  // Create the camera
  GameCamera = new Camera::OrthoCamera(this->width, this->height, -1.0f, 1.0f);

  // Create a sprite renderer instance
  Renderer = new SpriteRenderer(sprite_shader, GameCamera);

  // Load the background layers
  Texture background_bg = ResourceManager::Texture::load("textures/background/background-bg.png", true, "background-bg");
  Texture background_far = ResourceManager::Texture::load("textures/background/background-far.png", true, "background-far");
  Texture background_mid = ResourceManager::Texture::load("textures/background/background-mid.png", true, "background-mid");
  Texture background_near = ResourceManager::Texture::load("textures/background/background-near.png", true, "background-near");

  // Load textures into the game
  Texture nothing = ResourceManager::Texture::load("textures/nothing.png", true, "nothing");
  Texture windows = ResourceManager::Texture::load("textures/windows-11.png", true, "windows");
  Texture gigachad = ResourceManager::Texture::load("textures/gigachad.jpg", true, "gigachad");
  Texture tile_floor = ResourceManager::Texture::load("textures/tiles/tile-floor.png", true, "tile-floor");
  
  // Set up the global variables to create GameObjects
  glm::vec2 w_dimensions = glm::vec2(this->width, this->height);
  glm::vec2 grid = glm::vec2((float)width / 3.0f, (float)height / 2.0f);

  // Create the player
  Characters::Players::create("player", GameCamera, gigachad, w_dimensions, glm::vec3(100.0f, 100.0f, 0.0f));

  // Create GameObjects
  GameObjects::create("tile1", GameCamera, gigachad, w_dimensions, { "tile1", "tile" }, glm::vec3(0.0f), grid, 0.0f, grid / glm::vec2(2.0f), grid);
  GameObjects::create("tile1-floor", GameCamera, tile_floor, w_dimensions, { "tile1" }, glm::vec3(0.0f, (float)height / 2.0f - (float)height / 8.85, 0.0f), glm::vec2((float)width / 3.0f, (float)height / 8.85f), 0.0f);

  GameObjects::create("tile2", GameCamera, windows, w_dimensions, { "tile2", "tile" }, glm::vec3(grid.x, 0.0f, 0.0f), grid, 0.0f, grid / glm::vec2(2.0f), grid);
  GameObjects::create("tile2-floor", GameCamera, tile_floor, w_dimensions, { "tile2" }, glm::vec3(grid.x, grid.y - (float)height / 8.85, 0.0f), glm::vec2(grid.x, (float)height / 8.85f), 0.0f);

  for (GameObject *&object : GameObjects::all()) {
    object->interactive = false;
    object->rigidbody = true;
  }

  for (GameObject *&object : GameObjects::filter("tile")) {
    object->interactive = true;
    object->swap = true;
    object->rigidbody = false;
  }
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
  // Loop over every game object and check if the object is interactive and if the mouse intersects with it
  // If it does, set the snap to zero for smooth movement and make the current object focused
  for (GameObject *&object : GameObjects::all()) {
    // If the left mouse button was just pressed, then add that object and the objects with the same first tag
    // as the clicked object to the focused objects. No need to do this each frame while left click is being
    // held down
    if (Mouse.left_button_down && object->interactive && object->check_point_intersection(Mouse.position)) {
      object->old_transform.position = object->transform.position;
      Mouse.clicked_object = object;
      for (GameObject *&f_object : GameObjects::filter(object->tags[0])) {
        f_object->snap = false;
        f_object->originate = true;
        Mouse.focused_objects.push_back(f_object);
      }
    }

    // Update the bounding box of each object
    object->update_bounding_box();
  }

  // Update all player entities
  for (Player *player : Characters::Players::all()) {
    player->resolve_collisions();
    player->resolve_vectors();
  }

  // If an object is selected, then move the object along with the mouse
  if (Mouse.focused_objects != std::vector<GameObject *>()) {
    Mouse.clicked_object->originate = true;
    Mouse.clicked_object->translate_to_point(Mouse.position);
    for (GameObject *&object : Mouse.focused_objects) {
      if (object != Mouse.clicked_object) {
        object->originate = true;
        object->transform.position += Mouse.clicked_object->delta_transform.position;
      }
    }
  }

  // If we just released the left mouse button, then update the snap position of each currently
  // focused object while respecting their swap setting
  if (Mouse.left_button_up && Mouse.focused_objects != std::vector<GameObject *>()) {
    Mouse.clicked_object->snap = true;
    Mouse.clicked_object->originate = false;
    glm::vec3 old_pos = Mouse.clicked_object->transform.position;
    Mouse.clicked_object->update_snap_position();

    // If the object has swap enabled, then, if the target snap locaton is already occupied, snap
    // the other object already there to the position where the first object was, effectly swapping
    // their locations. Note that this process is quite taxing due to the fact that there are two
    // nested loops and objects need to be filtered by tags twice, adding further overhead
    for (GameObject *&object : GameObjects::except(Mouse.clicked_object->tags[0])) {
      if (object->swap && Mouse.clicked_object->swap) {
        if (object->transform.position == Mouse.clicked_object->transform.position) {
          object->old_transform.position = object->transform.position;
          object->translate_to_point(Mouse.clicked_object->old_transform.position);
          
          // Update the position of all the objects with the same tag as the object which got
          // displaced from its original spot (not the currently selected object)
          glm::vec3 delta = object->transform.position - object->old_transform.position;
          for (GameObject *&f_object : GameObjects::filter(object->tags[0])) {
            if (f_object->handle == object->handle) continue;
            f_object->originate = false;
            f_object->update_snap_position();
            f_object->transform.position += delta;
          }
        }
      }
    }

    // If the object does not get swapped, then update each object in focus and all other objects
    // which share the first tag with the parent object
    glm::vec3 delta = Mouse.clicked_object->transform.position - old_pos;
    for (GameObject *&object : Mouse.focused_objects) {
      if (object != Mouse.clicked_object) {
        object->originate = false;
        object->update_snap_position();
        object->transform.position += delta;
      }
    }
  }

  // If the F key is pressed, toggle fullscreen
  if (this->Keyboard['F'].pressed) toggle_fullscreen();

  // Reset the pressed and released status of the mouse buttons
  Mouse.left_button_down = false;
  Mouse.left_button_up = false;
  
  // If left click has been released, then there should be no active object
  if (!Mouse.left_button) {
    Mouse.focused_objects = std::vector<GameObject *>();
    Mouse.clicked_object = nullptr;
  }

  // If the escape key was pressed, then close the window
  if (Keyboard[256].pressed) glfwSetWindowShouldClose(this->GameWindow, true);

  // Reset KeyState::pressed and KeyState::released
  //! This method is extremely unoptimised. The multiple loops need to be fixed for optimal performance.
  std::vector<int> released_keys;
  for (auto pair : this->Keyboard) {
    if (pair.second.pressed) this->Keyboard[pair.first].pressed = false;
    if (pair.second.released) released_keys.push_back(pair.first);
  }

  for (int key : released_keys) {
    this->Keyboard.erase(this->Keyboard.find(key));
  }

  // Poll GLFW for new events
  glfwPollEvents();
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT);

  // Render the parallax background
  glm::vec2 scale = glm::vec2(width + 100.0f, height + 100.0f);
  Renderer->render(ResourceManager::Texture::get("background-bg"), { glm::vec3(0.0f), scale, 0.0f });
  Renderer->render(ResourceManager::Texture::get("background-far"), { glm::vec3((Mouse.position / glm::vec2(150.0f)) - glm::vec2(50.0f), 0.0f), scale, 0.0f });
  Renderer->render(ResourceManager::Texture::get("background-mid"), { glm::vec3((Mouse.position / glm::vec2(100.0f)) - glm::vec2(50.0f), 0.0f), scale, 0.0f });
  Renderer->render(ResourceManager::Texture::get("background-near"), { glm::vec3((Mouse.position / glm::vec2(50.0f)) - glm::vec2(50.0f), 0.0f), scale, 0.0f });

  // Render each GameObject
  for (GameObject *&object : GameObjects::all()) {
    object->render(Renderer);
  }

  // Render each Player
  for (Player *&player : Characters::Players::all()) {
    player->render(Renderer);
  }
}

void Game::set_window_hints() {
  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Use anti-aliasing
  glfwWindowHint(GLFW_SAMPLES, 16);

  // Disable resizing as the code is not compatible with resizing the viewport/camera
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

  // Set the GLFW cursor mode to normal
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
  for (auto &object : GameObjects::all()) {
    object->window_dimensions = glm::vec2(width, height);
  }

  // Toggle the window fullscreen state
  glfwSetWindowMonitor(this->GameWindow, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, 0);
}
