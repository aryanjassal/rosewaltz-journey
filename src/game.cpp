#include "game.h"

// Set up pointers to global objects for the game
Renderer *GameRenderer;
Camera *GameCamera;

// Forward-declare the tile size constant
glm::vec2 TileSize; 

// Forward-declare the default player object
Player *BasePlayer;

// FPS
int FPS = 0;
Timer fps_timer = Timer(500);

// Game constructor
Game::Game(unsigned int width, unsigned int height, std::string window_title, bool fullscreen) {
  // Set internal width and height
  this->width = width;
  this->height = height;
  this->GameTitle = window_title;

  // Initialise GLFW
  if (!glfwInit()) {
    error("GLFW failed to initialise!");
  }

  // Create a window for the game
  this->fullscreen = false;
  set_window_hints();
  create_window();
  if (fullscreen) toggle_fullscreen();

  // Enable alpha and transparency in OpenGL
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialise the game
  this->init();
}

// Game deconstructor
Game::~Game() {
  // Properly remove all the resources in resource manager's list
  ResourceManager::deallocate();
  delete GameRenderer;

  // Clean up and close the game
  glfwDestroyWindow(this->GameWindow);
  glfwTerminate();
}

void Game::load_level(const char *path) {
  this->GameState = std::map<std::string, bool>();

  Mouse.clicked_object = nullptr;

  for (Entity *&object : Entities::all()) {
    Entities::uninstantiate(object->id);
  }

  std::vector<std::vector<std::string>> instantiation_order;
  std::ifstream levelmap(path);
  std::string delimiter = ";";
  std::string line;
  if (levelmap.is_open()) {
    while(std::getline(levelmap, line)) {
      // Erase all spaces from the file
      line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char x) { return std::isspace(x); }), line.end());
      if (line.find("//") == 0 || !line.size()) continue;

      std::vector<std::string> layer;

      int pos = 0;
      pos = line.find(delimiter);
      while(pos != std::string::npos) {
        std::string token = line.substr(0, pos);
        layer.push_back(token);
        line.erase(0, pos + delimiter.length());
        pos = line.find(delimiter);
      }
      layer.push_back(line);
      instantiation_order.push_back(layer);
    }
  }

  // // Create GameObjects
  // for (int i = 0; i < instantiation_order.size(); i++) {
  //   for (int j = 0; j < instantiation_order.begin()->size(); j++) {
  //     std::string name = instantiation_order.at(i).at(j);
  //     if (name.find(">") != std::string::npos) {
  //       printf("%s\n", name.substr(0, name.find(">")).c_str());
  //       Entity *t = Entities::instantiate(name.substr(0, name.find(">")), Transform(glm::vec2(TileSize.x * j, TileSize.y * i), TileSize, 0.0f, 1.0f));
  //       if (name.substr(name.find(">") + 1) == "lock") t->interactive = false;
  //       else throw std::runtime_error("Invalid property\n");
  //     } else Entity *t = Entities::instantiate(name, Transform(glm::vec2(TileSize.x * j, TileSize.y * i), TileSize));
  //   }
  // }

  // if (Entities::filter("goal") == std::vector<Entity*>()) printf("[WARNING] Level has no goal tile!\n");
}

// Initialise the game by loading in and initialising all the required assets
void Game::init() {
  // Set the clear colour of the scene background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Create a shader program, providing the default vertex and fragment shaders
  Shader SpriteShader = ResourceManager::Shader::load("src/shaders/game.vs", "src/shaders/game.fs", "game");
  TextShader = ResourceManager::Shader::load("src/shaders/text.vs", "src/shaders/text.fs", "text");

  // Instantiate the camera and the renderer
  GameCamera = new Camera(this->width, this->height, 0.001f, 100.0f);
  TextCamera = GameCamera;
  // WindowSize = glm::vec2(this->width, this->height);
  GameRenderer = new Renderer(SpriteShader, GameCamera);

  // Initialise the font renderer
  // ResourceManager::Font::load("fonts/inclusive.ttf", "monocraft", 128, FILTER_LINEAR);
  ResourceManager::Texture::load("textures/blank.png", true, "blank");

  // Assign the camera and the renderer as global renderers for the GameObject
  Entities::Camera = GameCamera;
  Entities::Renderer = GameRenderer;

  // Load the textures from a given R* file
  ResourceManager::Texture::load_from_file("required.textures");
  
  // // Create the player
  // BasePlayer = Characters::Players::create("player", ResourceManager::Texture::get("blank"), Transform(glm::vec3(100.0f, 450.0f, 1.0f), glm::vec2(72.72f, 100.0f)), { "player" });
  // BasePlayer->texture_map = TextureMap(std::vector<Texture>(), true, 150.0);
  // BasePlayer->debug_colliders = true;

  // // Load the player's animation sprites
  // std::string base_name = "run";
  // std::string base_path = "textures/player/";
  // for (int i = 0; i < 5; i++) {
  //   BasePlayer->texture_map.textures.push_back(ResourceManager::Texture::load((base_path + base_name + std::to_string(i) + ".png").c_str(), true, "player-" + base_name + "-" + std::to_string(i)));
  // }
  
  // Load the ObjectPrefabs from the Prefabs R* file
  // Entities::Prefabs::load_from_file("required.prefabs");
}

bool Game::state(std::string state) {
  if (this->GameState.find(state) != this->GameState.end())
    return this->GameState[state];
  else
    return false;
}

std::string Game::cstate(std::string state) {
  if (this->CriticalGameState.find(state) != this->CriticalGameState.end())
    return this->CriticalGameState[state];
  else
    return "";
}

void Game::run() {
  std::chrono::high_resolution_clock::time_point delta_start_point, delta_end_point;
  while(!glfwWindowShouldClose(this->GameWindow)) {
    std::chrono::duration<double> elapsed_seconds{delta_end_point - delta_start_point};
    Time::delta = elapsed_seconds.count();
    if (fps_timer) {
      FPS = 1.0f / Time::delta;
      glfwSetWindowTitle(GameWindow, (this->GameTitle + " [FPS: " + std::to_string(FPS) + "]").c_str());
      fps_timer.reset();
    }

    delta_start_point = std::chrono::high_resolution_clock::now();
    this->update();
    this->render();
    delta_end_point = std::chrono::high_resolution_clock::now();
  }
}

void Game::update() {
  if (!this->state("game-over")) {
    fps_timer.tick(Time::delta);
  }

  if (this->Keyboard['F'].pressed) toggle_fullscreen();

  // Reset the pressed and released status of the mouse buttons
  Mouse.reset();

  // If the escape key was pressed, then close the window
  if (Keyboard[GLFW_KEY_ESCAPE].pressed) glfwSetWindowShouldClose(this->GameWindow, true);

  // Reset KeyState::pressed and KeyState::released
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
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);

  // // Render the parallax background
  // const float zoom = 100.0f;
  // const float z_index = 1.0f;
  // const glm::vec2 scale = glm::vec2(width + zoom, height + zoom);
  // GameRenderer->render(ResourceManager::Texture::get("background-bg"), Transform(glm::vec2(0.0f), scale, 0.0f, z_index));
  // GameRenderer->render(ResourceManager::Texture::get("background-far"), Transform((Mouse.position / glm::vec2(150.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));
  // GameRenderer->render(ResourceManager::Texture::get("background-mid"), Transform((Mouse.position / glm::vec2(100.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));
  // GameRenderer->render(ResourceManager::Texture::get("background-near"), Transform((Mouse.position / glm::vec2(50.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));

  // // Render the current fps of the game
  // Text::render("FPS: " + std::to_string(FPS), "monocraft", Transform(glm::vec2(0.0f), glm::vec2(0.5f)), TEXT_BOTTOM_LEFT);
  
  // Actually display the updated images to the screen
  glfwSwapBuffers(this->GameWindow);
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
  this->GameWindow = glfwCreateWindow(width, height, this->GameTitle.c_str(), this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  glfwMakeContextCurrent(this->GameWindow);
  glfwSwapInterval(0);

  // Initialise OpenGL using GLAD
  gladLoadGL();

  // If the window does not exist, then something went wrong!
  if (this->GameWindow == NULL) error("GLFW window failed to initialise!");

  // Set the GLFW cursor mode to normal
  glfwSetInputMode(this->GameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
  glfwSetWindowMonitor(this->GameWindow, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, this->fullscreen ? refresh : GLFW_DONT_CARE);
}
