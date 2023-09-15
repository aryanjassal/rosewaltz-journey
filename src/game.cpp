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
Timer fps_timer = Timer(100);

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

  // Create GameObjects
  for (int i = 0; i < instantiation_order.size(); i++) {
    for (int j = 0; j < instantiation_order.begin()->size(); j++) {
      std::string name = instantiation_order.at(i).at(j);
      if (name.find(">") != std::string::npos) {
        printf("%s\n", name.substr(0, name.find(">")).c_str());
        Entity *t = Entities::instantiate(name.substr(0, name.find(">")), Transform(glm::vec2(TileSize.x * j, TileSize.y * i), TileSize, 0.0f, 1.0f));
        if (name.substr(name.find(">") + 1) == "lock") t->interactive = false;
        else throw std::runtime_error("Invalid property\n");
      } else Entity *t = Entities::instantiate(name, Transform(glm::vec2(TileSize.x * j, TileSize.y * i), TileSize));
    }
  }

  printf("len: %i\n", Entities::all().size());
  for (auto &entity : Entities::all()) {
    printf("  id: [%i]%s\n", entity->id, entity->handle.c_str());
    printf("    transform: %.2f, %.2f, %.2f\n", entity->transform.position.x, entity->transform.position.y, entity->transform.z);
  }

  if (BasePlayer != nullptr) {
    BasePlayer->parent = nullptr;
    BasePlayer->translate(glm::vec2(100.0f, 450.0f));
    BasePlayer->velocity = glm::vec2(0.0f);
    BasePlayer->walk_speed = 100.0f; 
    BasePlayer->grounded = false; 
    BasePlayer->movable = true; 
    BasePlayer->won = false; 
    BasePlayer->dead = false; 
  }

  // if (Entities::filter("goal") == std::vector<Entity*>()) printf("[WARNING] Level has no goal tile!\n");
}

// Initialise the game by loading in and initialising all the required assets
void Game::init() {
  // Set the clear colour of the scene background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Create a shader program, providing the default vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "default");
  TextShader = ResourceManager::Shader::load("src/shaders/text.vert", "src/shaders/text.frag", "text");

  // Instantiate the camera and the renderer
  GameCamera = new Camera(this->width, this->height, 0.001f, 100.0f);
  TextCamera = GameCamera;
  WindowSize = glm::vec2(this->width, this->height);
  GameRenderer = new Renderer(sprite_shader, GameCamera);

  // Initialise the font renderer
  ResourceManager::Font::load("fonts/monocraft.ttf", "monocraft", 128, FILTER_NEAREST);
  ResourceManager::Texture::load("textures/blank.png", true, "blank");

  // Assign the camera and the renderer as global renderers for the GameObject
  Entities::Camera = GameCamera;
  Entities::Renderer = GameRenderer;

  TileSize = GameCamera->dimensions / glm::vec2(3.0f, 2.0f);

  // Load the textures from a given R* file
  ResourceManager::Texture::load_from_file("required.textures");
  
  // Create the player
  BasePlayer = Characters::Players::create("player", ResourceManager::Texture::get("blank"), Transform(glm::vec3(100.0f, 450.0f, 1.0f), glm::vec2(72.72f, 100.0f)), { "player" });
  BasePlayer->texture_map = TextureMap(std::vector<Texture>(), true, 150.0);
  BasePlayer->debug_colliders = true;

  // Load the player's animation sprites
  std::string base_name = "run";
  std::string base_path = "textures/player/";
  for (int i = 0; i < 5; i++) {
    BasePlayer->texture_map.textures.push_back(ResourceManager::Texture::load((base_path + base_name + std::to_string(i) + ".png").c_str(), true, "player-" + base_name + "-" + std::to_string(i)));
  }
  
  // Load the ObjectPrefabs from the Prefabs R* file
  Entities::Prefabs::load_from_file("required.prefabs");

  // CriticalGameState["level"] = "0.level";

  // // Load the level from a level R* file
  // this->load_level(cstate("level").c_str());

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
    // Time::delta = std::chrono::duration_cast<std::chrono::milliseconds>(delta_end_point - delta_start_point).count() / 1000.0f;
    std::chrono::duration<double> elapsed_seconds{delta_end_point - delta_start_point};
    Time::delta = elapsed_seconds.count();
    if (fps_timer) {
      FPS = 1.0f / Time::delta;
      fps_timer.reset();
    }
    // Time::delta = 1.0f / 60.0f;

    delta_start_point = std::chrono::high_resolution_clock::now();
    this->update();
    this->render();
    delta_end_point = std::chrono::high_resolution_clock::now();
  }
}

void Game::update() {
  if (!this->state("game-over")) {
    fps_timer.tick(Time::delta);

  //   if (Mouse.right_button_down) {
  //     BasePlayer->transform.position = glm::vec3(Mouse.position, 0.0f);
  //     BasePlayer->grounded = false;
  //   }
  //
  //   Entity *p_parent = nullptr;
  //   // printf("\n");
  //   for (Entity *&object : Entities::filter("tile")) {
  //     if (Mouse.left_button_down && !Mouse.left_button_up && BasePlayer->parent != nullptr) {
  //       if (object->interactive && BasePlayer->movable && object->check_collision(screen_to_world(Mouse.position))) {
  //         object->old_transform = object->transform;
  //         object->can_snap = false;
  //         Mouse.clicked_object = object;
  //
  //         if (object->check_collision(BasePlayer)) {
  //           BasePlayer->old_transform = BasePlayer->transform;
  //           // BasePlayer->position_offset = glm::vec3(std::fmod(BasePlayer->transform.position.x, TileSize.x), std::fmod(BasePlayer->transform.position.y, TileSize.y), 0.0f);
  //           BasePlayer->rigidbody = false;
  //           BasePlayer->set_parent(object);
  //         }
  //
  //         for (Entity *&child : object->children) {
  //           if (child == BasePlayer) continue;
  //           child->can_snap = false;
  //           child->rigidbody = false;
  //         }
  //       }
  //     }
  //
  //     // If the tile is a background tile, is colliding with the player, and no tile is selected by the mouse, then set the tile as the player's parent tile
  //     if (!Mouse.left_button_down && Mouse.clicked_object == nullptr && object->tags[0] == "tile" && object->check_collision(BasePlayer)) {
  //       p_parent = object;
  //     }
  //
  //     // Always update each object's bounding box
  //     // if (object == BasePlayer) {
  //     //   object->update_colliders(0, object->transform.position.x, object->transform.position.y + (object->transform.scale.y / 2.0f), object->transform.scale.x, object->transform.scale.y / 2.0f);
  //     //   object->update_colliders(1, object->transform.position.x, object->transform.position.y - (object->transform.scale.y / 2.0f), object->transform.scale.x, object->transform.scale.y / 2.0f);
  //     // }
  //     // else object->update_colliders();
  //
  //     // If tiles are being filtered, then the object can never be the player
  //     // if (object != BasePlayer) object->update_colliders();
  //     object->update_colliders();
  //   }
  //
  //   // If no object has been clicked, then the parent of the object will be whatever tile the player is colliding with.
  //   // Otherwise, the parent will not be updated.
  //   if (Mouse.clicked_object == nullptr) BasePlayer->set_parent(p_parent);
  //   
  //   // If an object has been selected, then move it and all its children with the mouse
  //   if (Mouse.clicked_object != nullptr) {
  //     Mouse.clicked_object->translate(screen_to_world(Mouse.position));
  //
  //     for (Entity *&child : Mouse.clicked_object->children) {
  //       child->translate(glm::vec2(child->parent->transform.position));
  //     }
  //   }
  //
  //   if (Mouse.left_button_up && !Mouse.left_button_down && !Mouse.left_button && Mouse.clicked_object != nullptr) {
  //     Mouse.clicked_object->can_snap = true;
  //     Mouse.clicked_object->snap();
  //
  //     if (Mouse.clicked_object->interactive) {
  //       for (Entity *&object : Entities::all()) {
  //         if (object == Mouse.clicked_object) continue;
  //         if (object->interactive && (int)object->transform.position.x == (int)Mouse.clicked_object->transform.position.x && (int)object->transform.position.y == (int)Mouse.clicked_object->transform.position.y) {
  //           // if (object->locked) {
  //           //   Mouse.clicked_object->translate(Mouse.clicked_object->old_transform.position);
  //           //   for (GameObject *child : Mouse.clicked_object->children) {
  //           //     if (child == BasePlayer) {
  //           //       BasePlayer->translate(object->transform.position + glm::vec3(std::fmod(BasePlayer->transform.position.x, TileSize.x), std::fmod(BasePlayer->transform.position.y, TileSize.y), BasePlayer->transform.position.z));
  //           //     } else {
  //           //       child->translate(object->transform.position);
  //           //     }
  //           //   }
  //           //   break;
  //           // }
  //
  //           object->translate(Mouse.clicked_object->old_transform.position);
  //           for (Entity *&child : object->children) {
  //             if (child == BasePlayer) {
  //               BasePlayer->translate(object->transform.position + glm::vec2(std::fmod(BasePlayer->transform.position.x, TileSize.x), std::fmod(BasePlayer->transform.position.y, TileSize.y)));
  //             } else {
  //               child->translate(object->transform.position);
  //             }
  //           }
  //           break;
  //         }
  //       }
  //     }
  //
  //     // Update the position of all tiled objects
  //     for (Entity *&object : Mouse.clicked_object->children) {
  //       if (object->handle != "tile" || object->handle != "player") {
  //         if (object->handle != "goal") {
  //           object->rigidbody = true;
  //         }
  //         object->translate(object->parent->transform.position);
  //       }
  //     }
  //
  //     // Update the position of the player
  //     BasePlayer->rigidbody = true;
  //     
  //     if (BasePlayer->parent == Mouse.clicked_object) {
  //       BasePlayer->translate(BasePlayer->parent->transform.position);
  //     }
  //
  //     Mouse.clicked_object = nullptr;
  //   }
  //
    // // Update all player entities
    // for (Player *player : Characters::Players::all()) {
    //   // if (Mouse.clicked_object == nullptr) {
    //   //   // player->resolve_vectors();
    //   //   player->update();
    //   //   player->resolve_collisions();
    //   // } else {
    //   //   player->update();
    //   // }
    //   player->update();
    // }
  //
  //   if (BasePlayer->won) GameState["game-over"] = true;
  // }
  //
  // if (BasePlayer->dead) {
  //   GameState["game-over"] = true;
  //   GameState["lost"] = true;
  }

  if (this->Keyboard['F'].pressed) toggle_fullscreen();

  // // Debug keybinds
  // float factor = (BasePlayer->walk_speed / std::fabs(BasePlayer->walk_speed));
  // factor = (std::fabs(factor) == 1.0f) ? factor : 1.0f;
  // if (this->Keyboard[GLFW_KEY_UP].pressed) BasePlayer->walk_speed += 100.0f * factor;
  // if (this->Keyboard[GLFW_KEY_DOWN].pressed) BasePlayer->walk_speed -= 100.0f * factor;
  // if (this->Keyboard['I'].pressed) BasePlayer->acceleration.y *= -1.0f;
  // if (this->Keyboard['U'].pressed) {
  //   BasePlayer->walk_speed *= -1.0f;
  //   // if (BasePlayer->walk_speed < 0) BasePlayer->flip_x = true;
  //   // else BasePlayer->flip_x = false;
  // }
  // if (this->Keyboard['C'].pressed) {
  //   this->GameState = std::map<std::string, bool>();
  //   Entities::get("goal")->texture_map.index = 0;
  // }
  // if (this->Keyboard['R'].pressed) this->load_level(cstate("level").c_str());
  //
  // if (this->Keyboard['1'].pressed) {
  //   CriticalGameState["level"] = "1.level";
  //   this->load_level("1.level");
  // } else if (this->Keyboard['2'].pressed) {
  //   CriticalGameState["level"] = "2.level";
  //   this->load_level("2.level");
  // }

  // if (this->Keyboard['K'].pressed) {
  //   DELAY -= 1000000;
  // } else if (this->Keyboard['L'].pressed) {
  //   DELAY += 1000000;
  // }
  //
  // for (long i = 0; i < DELAY; i++);

  //
  // // Reset the pressed and released status of the mouse buttons
  // Mouse.left_button_down = false;
  // Mouse.left_button_up = false;
  // Mouse.right_button_down = false;
  // Mouse.right_button_up = false;
  // 
  // // If left click has been released, then there should be no active object
  // if (!Mouse.left_button) {
  //   Mouse.clicked_object = nullptr;
  // }

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

  // Render the parallax background
  const float zoom = 100.0f;
  const float z_index = 1.0f;
  const glm::vec2 scale = glm::vec2(width + zoom, height + zoom);
  GameRenderer->render(ResourceManager::Texture::get("background-bg"), Transform(glm::vec2(0.0f), scale, 0.0f, z_index));
  GameRenderer->render(ResourceManager::Texture::get("background-far"), Transform((Mouse.position / glm::vec2(150.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));
  GameRenderer->render(ResourceManager::Texture::get("background-mid"), Transform((Mouse.position / glm::vec2(100.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));
  GameRenderer->render(ResourceManager::Texture::get("background-near"), Transform((Mouse.position / glm::vec2(50.0f) - glm::vec2(zoom / 2.0f)), scale, 0.0f, z_index));

  // // Render everything except the tile GameObject
  // for (Entity *&object : Entities::all()) {
  //   if (object->tags[0] != "tile" && std::find(Mouse.clicked_object->children.begin(), Mouse.clicked_object->children.end(), object) == Mouse.clicked_object->children.end() && object != Mouse.clicked_object) {
  //     object->render();
  //   }
  // }
  //
  // // Render each tile GameObject
  // for (Entity *&object : Entities::all()) {
  //   if (object->tags[0] == "tile") {
  //     object->render(glm::vec4(1.0f));
  //   }
  // }
  //
  // // Render the current active Player
  // // if (Mouse.clicked_object == nullptr && !state("game-over")) BasePlayer->animate();
  // if (Mouse.clicked_object != BasePlayer->parent || Mouse.clicked_object == nullptr) BasePlayer->render(BasePlayer->dead ? glm::vec4(0.97f, 0.2f, 0.2f, 1.0f) : glm::vec4(1.0f));
  //
  // // Render the selected object to render them in the front
  // if (Mouse.clicked_object != nullptr) {
  //   for (Entity *&object : Mouse.clicked_object->children) {
  //     object->render();
  //   }
  //
  //   Mouse.clicked_object->render(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
  //   if (Mouse.clicked_object == BasePlayer->parent) BasePlayer->render();
  // }

  // TEMP AND DEBUG AND WARN
  // BasePlayer->render();

  // if (Mouse.left_button_down && !BasePlayer->interactive) GameState["immovable-player"] = true;
  // if (state("immovable-player") && BasePlayer->interactive) GameState["immovable-player"] = false;
  //
  // if (state("immovable-player")) 
  //   Text::render("Cannot move tiles when player is between two tiles", "monocraft", Transform(glm::vec3(0.0f), glm::vec2(0.6f)), TEXT_MIDDLE_CENTER);
  //
  // if (state("game-over") && !state("lost"))
  //   Text::render("YOU WON!", "monocraft", Transform(glm::vec2(0.0f), glm::vec2(2.0f)), TEXT_MIDDLE_CENTER);
  // else if (state("game-over") && state("lost"))
  //   Text::render("YOU LOST!", "monocraft", Transform(glm::vec2(0.0f), glm::vec2(2.0f)), TEXT_MIDDLE_CENTER);

  Text::render("FPS: " + std::to_string(FPS), "monocraft", Transform(glm::vec2(0.0f), glm::vec2(0.5f)), TEXT_BOTTOM_LEFT);
  
  // Actually display the updated images to the screen
  glfwSwapBuffers(this->GameWindow);
}

void Game::set_window_hints() {
  // Tell GLFW which version and profile of OpenGL to use
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // // Use anti-aliasing
  // glfwWindowHint(GLFW_SAMPLES, 4);

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
  // GameCamera->scale(glm::vec2(width, height));

  // Toggle the window fullscreen state
  glfwSetWindowMonitor(this->GameWindow, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, this->fullscreen ? refresh : GLFW_DONT_CARE);
}
