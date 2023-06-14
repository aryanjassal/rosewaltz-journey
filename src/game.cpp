#include "game.h"

// //TEMP
// void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
// {
//   unsigned int VAO, VBO;
//   glGenVertexArrays(1, &VAO);
//   glGenBuffers(1, &VBO);
//   glBindVertexArray(VAO);
//   glBindBuffer(GL_ARRAY_BUFFER, VBO);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//   glEnableVertexAttribArray(0);
//   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
//   glBindVertexArray(0);
//     // activate corresponding render state	
//     // s.Use();
//     // glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
//     glActiveTexture(GL_TEXTURE0);
//     glBindVertexArray(VAO);
//
//     // iterate through all characters
//     std::string::const_iterator c;
//     for (c = text.begin(); c != text.end(); c++)
//     {
//         Character ch = ResourceManager::Font::Characters[*c];
//
//         float xpos = x + ch.bearing.x * scale;
//         float ypos = y - (ch.size.y - ch.bearing.y) * scale;
//
//         float w = ch.size.x * scale;
//         float h = ch.size.y * scale;
//         // update VBO for each character
//         float vertices[6][4] = {
//             { xpos,     ypos + h,   0.0f, 0.0f },            
//             { xpos,     ypos,       0.0f, 1.0f },
//             { xpos + w, ypos,       1.0f, 1.0f },
//
//             { xpos,     ypos + h,   0.0f, 0.0f },
//             { xpos + w, ypos,       1.0f, 1.0f },
//             { xpos + w, ypos + h,   1.0f, 0.0f }           
//         };
//         // render glyph texture over quad
//         glBindTexture(GL_TEXTURE_2D, ch.texture_id);
//         // update content of VBO memory
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         // render quad
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//         // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//         x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
//     }
//     glBindVertexArray(0);
//     glBindTexture(GL_TEXTURE_2D, 0);
// }


// Set up pointers to global objects for the game
SpriteRenderer *Renderer;
OrthoCamera *GameCamera;

glm::vec2 TileSize; 

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
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Create a shader program, providing the default vertex and fragment shaders
  Shader sprite_shader = ResourceManager::Shader::load("src/shaders/default.vert", "src/shaders/default.frag", "default");

  // Instantiate the camera and the renderer
  GameCamera = new OrthoCamera(this->width, this->height, -100.0f, 100.0f);
  WindowSize = glm::vec2(this->width, this->height);
  Renderer = new SpriteRenderer(sprite_shader, GameCamera);

  // Initialise the font renderer
  Fonts::init();
  ResourceManager::Font::load("fonts/monocraft.ttf", "monocraft", 128);

  // Assign the camera and the renderer as global renderers for the GameObject
  GameObjects::Camera = GameCamera;
  GameObjects::Renderer = Renderer;

  // Set the tile width and height settings
  TileSize = glm::vec2(GameCamera->width / 3.0f, GameCamera->height / 2.0f);

  // Load the background layers
  Texture background_bg = ResourceManager::Texture::load("textures/background/background-bg.png", true, "background-bg");
  Texture background_far = ResourceManager::Texture::load("textures/background/background-far.png", true, "background-far");
  Texture background_mid = ResourceManager::Texture::load("textures/background/background-mid.png", true, "background-mid");
  Texture background_near = ResourceManager::Texture::load("textures/background/background-near.png", true, "background-near");

  // Load textures into the game
  Texture nothing = ResourceManager::Texture::load("textures/nothing.png", true, "nothing");
  Texture windows = ResourceManager::Texture::load("textures/windows-11.png", true, "windows");
  Texture gigachad = ResourceManager::Texture::load("textures/gigachad.jpg", true, "gigachad");
  Texture journey = ResourceManager::Texture::load("textures/journey.jpg", true, "journey");
  Texture cross = ResourceManager::Texture::load("textures/cross.png", true, "cross");
  Texture floor = ResourceManager::Texture::load("textures/tiles/tile-floor.png", true, "tile-floor");
  Texture treasure = ResourceManager::Texture::load("textures/tiles/treasure.png", true, "treasure");
  Texture treasure_open = ResourceManager::Texture::load("textures/tiles/treasure-open.png", true, "treasure-open");
  
  // Set up the global variables to create GameObjects
  float ratio = (float)height / 8.85f;

  // Create the player
  Player *player = Characters::Players::create("player", gigachad, Transform(glm::vec3(100.0f, 450.0f, 1.0f), glm::vec2(100.0f)), { "player" });
  Characters::Players::ActivePlayer = player;

  // Create ObjectPrefabs
  GameObject *tile = GameObjects::ObjectPrefabs::create("tile", nothing, { "tile" }, Transform(glm::vec3(0.0f), TileSize));
  tile->origin = TileSize / glm::vec2(2.0f); 
  tile->grid = TileSize;
  tile->interactive = true;
  tile->swap = true;

  GameObject *tile_floor = GameObjects::ObjectPrefabs::create("tile-floor", floor, { "tile-floor" }, Transform(glm::vec3(0.0f), glm::vec2(TileSize.x, ratio)));
  tile_floor->rigidbody = true;
  tile_floor->position_offset = glm::vec3(0.0f, TileSize.y - ratio, 1.0f);
  tile_floor->set_parent(tile);

  GameObject *goal = GameObjects::ObjectPrefabs::create("goal", treasure, { "goal" });
  goal->position_offset = glm::vec3((TileSize.x / 2.0f) - (goal->transform.scale.x / 2.0f), TileSize.y - ratio - goal->transform.scale.y, 1.0f);

  GameObject *immovable = GameObjects::ObjectPrefabs::create("immovable", nothing, { "tile", "locked" }, Transform(glm::vec3(0.0f), TileSize));
  immovable->origin = TileSize / glm::vec2(2.0f); 
  immovable->grid = TileSize;
  immovable->swap = true;
  immovable->locked = true;

  // Create GameObjects
  for (int i = 0; i < 3; i++) {
    GameObject *t = GameObjects::instantiate("tile", Transform(glm::vec3(TileSize.x * i, TileSize.y, 1.0f), TileSize));
    // t->texture = (i == 0) ? gigachad : (i == 1) ? windows : journey;

    if (i == 2) {
      GameObject *g = GameObjects::instantiate("goal");
      g->set_parent(t);
      g->translate(t->transform.position);
    }

    GameObject *im = GameObjects::instantiate("immovable", Transform(glm::vec3(TileSize.x * i, 0.0f, 1.0f), TileSize));
  }

  // for (auto &object : GameObjects::all()) {
  //   printf("[exists] (%i) [%.2f, %.2f] %s\n", object->id, object->transform.position.x, object->transform.position.y, object->handle.c_str());
  // }
}

void Game::run() {
  std::chrono::high_resolution_clock::time_point start_point, end_point;
  while(!glfwWindowShouldClose(this->GameWindow)) {
    Time::delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - start_point).count() / 1000.0f;

    start_point = std::chrono::high_resolution_clock::now();
    this->update();
    this->render();
    end_point = std::chrono::high_resolution_clock::now();
  }
}

void Game::update() {
  GameObject *p_parent = nullptr;
  for (GameObject *&object : GameObjects::all()) {
    if (Mouse.left_button_down && !Mouse.left_button_up && Characters::Players::ActivePlayer->parent != nullptr) {
      if (object->interactive && !Characters::Players::ActivePlayer->locked && object->check_point_intersection(screen_to_world(Mouse.position))) {
        object->old_transform = object->transform;
        object->snap = false;
        Mouse.clicked_object = object;

        if (object->check_collision(Characters::Players::ActivePlayer).collision) {
          Characters::Players::ActivePlayer->old_transform = Characters::Players::ActivePlayer->transform;
          Characters::Players::ActivePlayer->position_offset = glm::vec3(std::fmod(Characters::Players::ActivePlayer->transform.position.x, TileSize.x), std::fmod(Characters::Players::ActivePlayer->transform.position.y, TileSize.y), 0.0f);
          Characters::Players::ActivePlayer->rigidbody = false;
          Characters::Players::ActivePlayer->set_parent(object);
        }

        for (GameObject *&child : object->children) {
          if (child == Characters::Players::ActivePlayer) continue;
          child->snap = false;
          child->originate = true;
          child->rigidbody = false;
          Mouse.focused_objects.push_back(child);
        }
      }
    }

    // If the tile is a background tile, is colliding with the player, and no tile is selected by the mouse, then set the tile as the player's parent tile
    if (!Mouse.left_button_down && Mouse.clicked_object == nullptr && object->tags[0] == "tile" && object->check_collision(Characters::Players::ActivePlayer).collision) {
      p_parent = object;
    }

    // Always update each object's bounding box
    object->update_bounding_box();
  }

  // If no object has been clicked, then the parent of the object will be whatever tile the player is colliding with.
  // Otherwise, the parent will not be updated.
  if (Mouse.clicked_object == nullptr) Characters::Players::ActivePlayer->set_parent(p_parent);
  
  // If an object has been selected, then move it and all its children with the mouse
  if (Mouse.focused_objects != std::vector<GameObject *>()) {
    Mouse.clicked_object->originate = true;
    Mouse.clicked_object->translate(screen_to_world(Mouse.position));

    for (GameObject *&child : Mouse.clicked_object->children) {
      child->originate = true;
      child->translate(glm::vec2(child->parent->transform.position));
    }
  }

  if (Mouse.left_button_up && !Mouse.left_button_down && !Mouse.left_button && Mouse.clicked_object != nullptr) {
    Mouse.clicked_object->snap = true;
    Mouse.clicked_object->originate = false;
    Mouse.clicked_object->update_snap_position();

    if (Mouse.clicked_object->swap) {
      for (GameObject *&object : GameObjects::all()) {
        if (object == Mouse.clicked_object) continue;
        if (object->swap && (int)object->transform.position.x == (int)Mouse.clicked_object->transform.position.x && (int)object->transform.position.y == (int)Mouse.clicked_object->transform.position.y) {
          if (object->locked) {
            Mouse.clicked_object->translate(Mouse.clicked_object->old_transform.position);
            for (GameObject *child : Mouse.clicked_object->children) {
              if (child == Characters::Players::ActivePlayer) {
                Characters::Players::ActivePlayer->translate(object->transform.position + glm::vec3(std::fmod(Characters::Players::ActivePlayer->transform.position.x, TileSize.x), std::fmod(Characters::Players::ActivePlayer->transform.position.y, TileSize.y), Characters::Players::ActivePlayer->transform.position.z));
              } else {
                child->translate(object->transform.position);
              }
            }
            break;
          }

          object->translate(Mouse.clicked_object->old_transform.position);
          for (GameObject *&child : object->children) {
            if (child == Characters::Players::ActivePlayer) {
              Characters::Players::ActivePlayer->translate(object->transform.position + glm::vec3(std::fmod(Characters::Players::ActivePlayer->transform.position.x, TileSize.x), std::fmod(Characters::Players::ActivePlayer->transform.position.y, TileSize.y), Characters::Players::ActivePlayer->transform.position.z));
            } else {
              child->translate(object->transform.position);
            }
          }
          break;
        }
      }
    }

    // Update the position of all tiled objects
    for (GameObject *&object : Mouse.focused_objects) {
      if (object->handle != "tile" || object->handle != "player") {
        if (object->handle != "goal") {
          object->originate = false;
          object->rigidbody = true;
        }
        object->translate(object->parent->transform.position);
      }
    }

    // Update the position of the player
    Characters::Players::ActivePlayer->rigidbody = true;
    
    if (Characters::Players::ActivePlayer->parent == Mouse.clicked_object) {
      glm::vec3 position_offset = Characters::Players::ActivePlayer->position_offset;
      Characters::Players::ActivePlayer->position_offset = glm::vec3(0.0f);
      Characters::Players::ActivePlayer->translate(Characters::Players::ActivePlayer->parent->transform.position + position_offset);
    }

    Mouse.clicked_object = nullptr;
    Mouse.focused_objects = std::vector<GameObject *>();
  }

  // Update all player entities
  for (Player *player : Characters::Players::all()) {
    if (Mouse.clicked_object == nullptr) {
      player->resolve_vectors();
      player->update();
      player->resolve_collisions();
    } else {
      player->update();
    }
  }

  if (this->Keyboard['F'].pressed) toggle_fullscreen();

  // Debug keybinds
  float factor = (Characters::Players::ActivePlayer->walk_speed / std::fabs(Characters::Players::ActivePlayer->walk_speed));
  factor = (std::fabs(factor) == 1.0f) ? factor : 1.0f;
  if (this->Keyboard[GLFW_KEY_UP].pressed) Characters::Players::ActivePlayer->walk_speed += 100.0f * factor;
  if (this->Keyboard[GLFW_KEY_DOWN].pressed) Characters::Players::ActivePlayer->walk_speed -= 100.0f * factor;
  if (this->Keyboard['I'].pressed) Characters::Players::ActivePlayer->acceleration.y *= -1.0f;
  if (this->Keyboard['U'].pressed) Characters::Players::ActivePlayer->walk_speed *= -1.0f;


  // Reset the pressed and released status of the mouse buttons
  Mouse.left_button_down = false;
  Mouse.left_button_up = false;
  
  // If left click has been released, then there should be no active object
  if (!Mouse.left_button) {
    Mouse.focused_objects = std::vector<GameObject *>();
    Mouse.clicked_object = nullptr;
  }

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render the parallax background
  const float zoom = 100.0f;
  const float z_index = 1.0f;
  glm::vec2 scale = glm::vec2(width + zoom, height + zoom);
  Renderer->render(ResourceManager::Texture::get("background-bg"), Transform(glm::vec3(0.0f, 0.0f, z_index), scale));
  Renderer->render(ResourceManager::Texture::get("background-far"), Transform(glm::vec3((Mouse.position / glm::vec2(150.0f)) - glm::vec2(zoom / 2.0f), z_index), scale));
  Renderer->render(ResourceManager::Texture::get("background-mid"), Transform(glm::vec3((Mouse.position / glm::vec2(100.0f)) - glm::vec2(zoom / 2.0f), z_index), scale));
  Renderer->render(ResourceManager::Texture::get("background-near"), Transform(glm::vec3((Mouse.position / glm::vec2(50.0f)) - glm::vec2(zoom / 2.0f), z_index), scale));

  // Render everything except the tile GameObject
  for (GameObject *&object : GameObjects::all()) {
    if (object->tags[0] != "tile" && std::find(Mouse.focused_objects.begin(), Mouse.focused_objects.end(), object) == Mouse.focused_objects.end() && object != Mouse.clicked_object) {
      object->render();
    }
  }

  // Render each tile GameObject
  for (GameObject *&object : GameObjects::all()) {
    if (object->tags[0] == "tile") {
      object->render(glm::vec4(1.0f), (object->tags[1] == "locked") ? 0 : -1);
    }
  }

  // Render the current active Player
  Characters::Players::ActivePlayer->render();

  // Render the selected object to render them in the front
  if (Mouse.clicked_object != nullptr && Mouse.focused_objects != std::vector<GameObject *>()) {
    for (GameObject *&object : Mouse.focused_objects) {
      object->render();
    }

    Mouse.clicked_object->render(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), 1);
  }

  // RenderText("X", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

  // Actually display the updated images to the screen
  glfwSwapBuffers(this->GameWindow);
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

  // Here, the command `*Camera->resize(width, height);` must be run for a Camera object if the objects are expected to
  // have absolute sizes irrespective of the screen size. If the objects are expected to keep the same size regardless of
  // the window size, then resize the camera's matrices here.
  WindowSize = glm::vec2(width, height);

  // Toggle the window fullscreen state
  glfwSetWindowMonitor(this->GameWindow, this->fullscreen ? glfwGetPrimaryMonitor() : nullptr, 0, 0, width, height, refresh);
}
