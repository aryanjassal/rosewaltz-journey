#include "game.h"

// Declare an active game object to which all input events will go through
Game *ActiveGame;

// FPS
int max_fps = 480;
double min_frame_time = 1.0f / max_fps;
const double sleep_buffer = 0.0005f;
Timer fps_timer = Timer(0);

// SUPER ULTRA TEMP
void newfps(int fps) {
  if (fps == 0) {
    max_fps = 0;
    min_frame_time = 0;
    return;
  }
  max_fps = fps;
  min_frame_time = 1.0f / max_fps;
}

// Callbacks for GLFW
void mouse_button_callback(GLFWwindow* window, int button, int clicked, int mods) {
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT: {
      if (clicked) {
        ActiveGame->Mouse.left_button = true;
        ActiveGame->Mouse.left_button_down = true;
      } else {
        ActiveGame->Mouse.left_button = false;
        ActiveGame->Mouse.left_button_up = true;
      }
      break;
    }

    case GLFW_MOUSE_BUTTON_RIGHT: {
      if (clicked) {
        ActiveGame->Mouse.right_button = true;
        ActiveGame->Mouse.right_button_down = true;
      } else {
        ActiveGame->Mouse.right_button = false;
        ActiveGame->Mouse.right_button_up = true;
      }
      break;
    }
  }
}

void mouse_callback(GLFWwindow* window, double x, double y) {
  ActiveGame->Mouse.position = glm::vec2(x, y);
}

void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (key >= 0) {
    if (action == GLFW_PRESS) {
      ActiveGame->Keyboard[key] = { true, true, false };
    } else if (action == GLFW_RELEASE) {
      ActiveGame->Keyboard[key] = { false, false, true };
    }
  }
}

// Game constructor
Game::Game(unsigned int width, unsigned int height, std::string window_title, bool fullscreen) {
  // Set internal width and height
  this->width = width;
  this->height = height;
  this->title = window_title;

  // Initialise GLFW
  if (!glfwInit()) {
    error("GLFW failed to initialise!");
  }

  // Create a window for the game
  // this->fullscreen = false;
  set_window_hints();
  create_window();
  if (fullscreen) toggle_fullscreen();

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set the callbacks for the game
  glfwSetCursorPosCallback(this->window, mouse_callback);
  glfwSetMouseButtonCallback(this->window, mouse_button_callback);
  glfwSetKeyCallback(this->window, keyboard_callback);

  // Set the clear colour of the scene background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Create a shader program, providing the default vertex and fragment shaders
  Shader SpriteShader = ResourceManager::Shader::load("src/shaders/game.vs", "src/shaders/game.fs", "game");
  TextShader = ResourceManager::Shader::load("src/shaders/text.vs", "src/shaders/text.fs", "text");

  // Instantiate the camera and the renderer
  camera = new Camera(this->width, this->height, 0.001f, 100.0f);
  TextCamera = camera;
  // WindowSize = glm::vec2(this->width, this->height);
  renderer = new Renderer(SpriteShader, camera);

  // Assign the camera and the renderer as global renderers for the GameObject
  Entities::Camera = camera;
  Entities::Renderer = renderer;

  // Load the textures from a given R* file
  ResourceManager::Texture::load_from_file("required.textures");
}

// Game deconstructor
Game::~Game() {
  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();
  delete renderer;
  delete camera;

  // Clean up and close the game
  glfwDestroyWindow(this->window);
  glfwTerminate();
}

void Game::set_active() {
  ActiveGame = this;
}

void Game::run() {
  std::chrono::high_resolution_clock::time_point delta_start, delta_end;
  std::chrono::high_resolution_clock::time_point update_start, update_end;
  while(!glfwWindowShouldClose(this->window)) {
    std::chrono::duration<double> elapsed_seconds{delta_end - delta_start};
    Time::delta = elapsed_seconds.count();

    delta_start = std::chrono::high_resolution_clock::now();

    update_start = std::chrono::high_resolution_clock::now();
    this->update();
    this->render();
    update_end = std::chrono::high_resolution_clock::now();

    double sleep_time = min_frame_time - (std::chrono::duration<double>(update_end - update_start).count());
    if (sleep_time > 0.0f) {
      const auto t1 = std::chrono::high_resolution_clock::now();
      std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time - sleep_buffer));

      const auto t2 = std::chrono::high_resolution_clock::now();
      const std::chrono::duration<double> elapsed = t2 - t1;

      const auto t3 = std::chrono::high_resolution_clock::now();
      for (auto t = std::chrono::high_resolution_clock::now(); std::chrono::duration<double>(t - t3).count() < sleep_time - elapsed.count(); t = std::chrono::high_resolution_clock::now()) {
        // printf("t-t3: %.10f, sleep-elapsed: %.6f\n", std::chrono::duration<double>(t - t3).count(), sleep_time - elapsed.count());
      }

      // printf("meant to sleep: %.6f; slept for %.6f seconds\n", sleep_time, elapsed);
    }

    delta_end = std::chrono::high_resolution_clock::now();
    int FPS = 1.0f / std::chrono::duration<double>(delta_end - delta_start).count();

    if (fps_timer) {
      glfwSetWindowTitle(window, (this->title + " [FPS: " + std::to_string(FPS) + "]").c_str());
      fps_timer.reset();
    }
  }
}

void Game::update() {
  fps_timer.tick(Time::delta);

  if (Keyboard['F'].pressed) toggle_fullscreen();

  // If the escape key was pressed, then close the window
  if (Keyboard[GLFW_KEY_ESCAPE].pressed) glfwSetWindowShouldClose(this->window, true);

  if (Keyboard['0'].pressed) newfps(10);
  if (Keyboard['1'].pressed) newfps(60);
  if (Keyboard['2'].pressed) newfps(144);
  if (Keyboard['3'].pressed) newfps(480);
  if (Keyboard['4'].pressed) newfps(960);
  if (Keyboard['5'].pressed) newfps(1000);
  if (Keyboard['6'].pressed) newfps(3000);
  if (Keyboard['7'].pressed) newfps(7000);
  if (Keyboard['8'].pressed) newfps(10000);
  if (Keyboard['9'].pressed) newfps(0);

  // Reset KeyState::pressed and KeyState::released
  std::vector<int> released_keys;
  for (auto pair : this->Keyboard) {
    if (pair.second.pressed) this->Keyboard[pair.first].pressed = false;
    if (pair.second.released) released_keys.push_back(pair.first);
  }

  for (int key : released_keys) {
    this->Keyboard.erase(this->Keyboard.find(key));
  }

  // Reset the pressed and released status of the mouse buttons
  Mouse.reset();

  // Poll GLFW for new events
  glfwPollEvents();
}

void Game::render() {
  // Clear the screen (paints it to the predefined clear colour)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // // Render the current fps of the game
  // Text::render("FPS: " + std::to_string(FPS), "monocraft", Transform(glm::vec2(0.0f), glm::vec2(0.5f)), TEXT_BOTTOM_LEFT);
  
  // Actually display the updated images to the screen
  glfwSwapBuffers(this->window);
}

void Game::set_window_hints() {
  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Disable resizing as the code is not compatible with resizing the viewport/camera
  glfwWindowHint(GLFW_RESIZABLE, false);
}

void Game::create_window() {
  // Create a GLFW window
  this->window = glfwCreateWindow(width, height, this->title.c_str(), this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  glfwMakeContextCurrent(this->window);
  glfwSwapInterval(0);

  // Initialise OpenGL using GLAD
  gladLoadGL();

  // If the window does not exist, then something went wrong!
  if (this->window == nullptr) error("GLFW window failed to initialise!");

  // Set the GLFW cursor mode to normal
  glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Game::toggle_fullscreen() {
  // Actually toggle the fullscreen mode
  this->fullscreen = !this->fullscreen;

  // Get current window dimensions
  int width = this->width;
  int height = this->height;
  int refresh = 60;

  // If the window mode is fullscreen, then actually apply the fullscreen window dimensions
  if (this->fullscreen) {
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width;
    height = mode->height;
    refresh = mode->refreshRate;
  }

  // Update the OpenGL viewport
  glViewport(0, 0, width, height);

  // Here, the command `Camera->resize(width, height);` must be run for a Camera object if the objects are expected to
  // have absolute sizes irrespective of the screen size. If the objects are expected to keep the same size regardless of
  // the window size, then resize the camera's matrices here.
  WindowSize = glm::vec2(width, height);

  // Toggle the window fullscreen state
  glfwSetWindowMonitor(this->window, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, this->fullscreen ? refresh : GLFW_DONT_CARE);
}

