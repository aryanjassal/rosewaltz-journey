#include "object.h"

// Publically store the active Camera and the Renderer
OrthoCamera *GameObjects::Camera = new OrthoCamera(WindowSize.x, WindowSize.y, 1000.0f, -1000.0f);
SpriteRenderer *GameObjects::Renderer = nullptr;

// Store a list of all the GameObjects and Prefabs ever created
std::map<unsigned long, GameObject> Objects;
std::map<std::string, GameObject> Prefabs;

// Counter to keep track of the next id for instantiated GameObjects
static unsigned long instantiation_id = 0;

void GameObject::render(glm::vec4 colour, int focus) {
  Transform n_transform = this->transform;
  n_transform.position += this->position_offset;
  if (this->flip_x) {
    n_transform.position.x += n_transform.scale.x;
    n_transform.scale.x *= -1.0f;
  }
  if (this->flip_y) n_transform.scale.y *= -1.0f;
  // if (this->tags[0] == "tile") n_transform.scale += glm::vec2(2.0f);

  if (this->active) {
    if (this->transform.position.z >= GameObjects::Camera->far || this->transform.position.z <= GameObjects::Camera->near) {
      printf("[WARNING] Object (%i) %s has z-index out of the camera's range.", this->id, this->handle.c_str());
    }

    if (this->collider_revealed) {
      unsigned int t_vao, t_vbo;
      glGenVertexArrays(1, &t_vao);
      glGenBuffers(1, &t_vbo);
      glBindVertexArray(t_vao);
      glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      Shader shader = ResourceManager::Shader::get("default");
      shader.activate();
      shader.set_vector_4f("colour", glm::vec4(0.5f));
      shader.set_matrix_4f("projection", GameObjects::Camera->projection_matrix);
      shader.set_matrix_4f("model", glm::mat4(1.0f));
      shader.set_matrix_4f("view", glm::mat4(1.0f));
      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(t_vao);

      float xpos = this->transform.position.x + this->position_offset.x;
      float ypos = this->transform.position.y + this->position_offset.y;
      float w = this->transform.scale.x;
      float h = this->transform.scale.y;

      float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 1.0f },            
        { xpos,     ypos,       0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 0.0f },

        { xpos,     ypos + h,   0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 0.0f },
        { xpos + w, ypos + h,   1.0f, 1.0f }           
      };

      glBindTexture(GL_TEXTURE_2D, ResourceManager::Texture::get("blank").id);

      // Update the content of the VBO buffer
      glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // Draw the buffer onto the screen
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // Unbind the VAOs and the textures
      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    GameObjects::Renderer->render(this->texture[this->texture_index], n_transform, colour, focus);

    if (this->collider_revealed) {
      unsigned int t_vao, t_vbo;
      glGenVertexArrays(1, &t_vao);
      glGenBuffers(1, &t_vbo);
      glBindVertexArray(t_vao);
      glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      Shader shader = ResourceManager::Shader::get("default");
      shader.activate();
      shader.set_vector_4f("colour", glm::vec4(1.0f, 0.3f, 0.3f, 0.8f));
      shader.set_matrix_4f("projection", GameObjects::Camera ->projection_matrix);
      shader.set_matrix_4f("model", glm::mat4(1.0f));
      shader.set_matrix_4f("view", glm::mat4(1.0f));
      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(t_vao);

      float xpos = this->transform.position.x + this->position_offset.x + this->origin.x;
      float ypos = this->transform.position.y + this->position_offset.y + this->origin.y;
      float w = 10.0f;
      float h = 10.0f;

      float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 1.0f },            
        { xpos,     ypos,       0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 0.0f },

        { xpos,     ypos + h,   0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 0.0f },
        { xpos + w, ypos + h,   1.0f, 1.0f }           
      };

      glBindTexture(GL_TEXTURE_2D, ResourceManager::Texture::get("blank").id);

      // Update the content of the VBO buffer
      glBindBuffer(GL_ARRAY_BUFFER, t_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // Draw the buffer onto the screen
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // Unbind the VAOs and the textures
      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
}

void GameObject::translate(glm::vec2 point) {
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);
  this->transform.position = glm::vec3(point - origin, 0.0f);

  if (this->snap) this->update_snap_position();
  else this->update_bounding_box();
}

bool GameObject::check_point_intersection(glm::vec2 point) {
  return ((this->bounding_box.left <= point.x) 
    && (this->bounding_box.right >= point.x) 
    && (this->bounding_box.top <= point.y) 
    && (this->bounding_box.bottom >= point.y));
}

Collision GameObject::check_collision(GameObject *object) {
  if (object->bounding_box.right >= this->bounding_box.left
    && object->bounding_box.left <= this->bounding_box.right
    && object->bounding_box.bottom >= this->bounding_box.top
    && object->bounding_box.top <= this->bounding_box.bottom
  ) {
    glm::vec2 this_center = glm::vec2(this->transform.position + this->position_offset) + (this->transform.scale / glm::vec2(2.0f));
    glm::vec2 other_half_extent = object->transform.scale / glm::vec2(2.0f);
    glm::vec2 other_center = glm::vec2(object->transform.position + object->position_offset) + other_half_extent;
    
    glm::vec2 clamped = glm::clamp(this->transform.scale, -other_half_extent, other_half_extent);
    glm::vec2 closest = other_center + clamped;
    glm::vec2 difference = closest - this_center;

    Direction direction = vector_direction(difference);

    CollisionInfo vertical((object->bounding_box.bottom >= this->bounding_box.top && object->bounding_box.top <= this->bounding_box.bottom));
    // vertical.collision = (object->bounding_box.bottom >= this->bounding_box.top && object->bounding_box.top <= this->bounding_box.bottom);
    vertical.direction = vector_direction(glm::vec2(0.0f, difference.y));
    vertical.mtv = difference.y + (difference.y > 0 ? (this->transform.scale.y / -2.0f) + object->transform.scale.y : (this->transform.scale.y / 2.0f));

    CollisionInfo horizontal((object->bounding_box.right >= this->bounding_box.left && object->bounding_box.left <= this->bounding_box.right));
    // horizontal.collision = (object->bounding_box.right >= this->bounding_box.left && object->bounding_box.left <= this->bounding_box.right);
    horizontal.direction = vector_direction(glm::vec2(difference.x, 0.0f));
    horizontal.mtv = difference.x + (this->transform.scale.x / 2.0f);

    return Collision(true, horizontal, vertical);
  }

  return Collision();
}

void GameObject::update_bounding_box() {
  // Use the origin if originate is set, otherwise remove it from any calculations
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  // Use the offset transform to calculate the bounding boxes
  Transform n_transform = this->transform;
  n_transform.position += this->position_offset;

  // Update the bounding boxes using some super advanced math
  this->bounding_box.right = n_transform.position.x + this->transform.scale.x + origin.x;
  this->bounding_box.left = n_transform.position.x + origin.x;
  this->bounding_box.bottom = n_transform.position.y + this->transform.scale.y + origin.y;
  this->bounding_box.top = n_transform.position.y + origin.y;
  
  // if (this->handle == "goal") printf("[%s] [collider] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x + this->position_offset.x, this->transform.position.y + this->position_offset.y);
}

void GameObject::update_snap_position() {
  glm::vec2 origin = this->origin;
  glm::vec3 new_position;
  new_position.x = std::floor((this->transform.position.x + origin.x) / this->grid.x) * this->grid.x;
  new_position.y = std::floor((this->transform.position.y + origin.y) / this->grid.y) * this->grid.y;

  // If the new position is outside the dimensions, then just undo any translations and return it to its old position
  if (new_position.x < 0 || new_position.x > GameObjects::Camera->width - this->grid.x || new_position.y < 0 || new_position.y > GameObjects::Camera->height - this->grid.y) {
    this->transform.position = this->old_transform.position;
    this->update_bounding_box();
    return;
  }

  // Otherwise, update the delta transform, the object's position, and it's bounding box
  this->transform.position = new_position;
  this->update_bounding_box();
}

void GameObject::set_parent(GameObject *parent) {
  if (parent != nullptr) {
    this->unset_parent();
    this->parent = parent;
    parent->children.push_back(this);
  } else {
    this->unset_parent();
  }
}

void GameObject::unset_parent() {
  if (this->parent != nullptr) {
    if (std::find(this->parent->children.begin(), this->parent->children.end(), this) != this->parent->children.end()) {
      this->parent->children.erase(std::find(this->parent->children.begin(), this->parent->children.end(), this));
    }
    this->parent = nullptr;
  }
}

void GameObject::set_child(GameObject *child) {
  if (child != nullptr) {
    child->parent = this;
    this->children.push_back(child);
  }
}

void GameObject::unset_child(GameObject *child) {
  if (child != nullptr) {
    this->children.erase(std::find(this->children.begin(), this->children.end(), child));
    child->parent = nullptr;
  }
}

GameObject *GameObjects::ObjectPrefabs::create(std::string handle, Texture texture, std::vector<std::string> tags, Transform transform) {
  return GameObjects::ObjectPrefabs::create(handle, (std::vector<Texture>) { texture }, tags, transform);
}

GameObject *GameObjects::ObjectPrefabs::create(std::string handle, std::vector<Texture> texture, std::vector<std::string> tags, Transform transform) {
  if (GameObjects::Renderer == nullptr) throw std::runtime_error("A SpriteRenderer must be set for GameObjects::Renderer\n");
  if (Prefabs.find(handle) != Prefabs.end()) throw std::runtime_error("Another Prefab already exists with the same handle as " + handle + "'\n");

  GameObject object = GameObject();
  object.handle = handle;
  object.texture = (std::vector<Texture>) { texture };
  object.tags = tags;
  object.transform = transform;
  object.active = false;
  object.update_bounding_box();

  Prefabs[handle] = object;
  return &Prefabs[handle];
}

GameObject *GameObjects::ObjectPrefabs::create(std::string handle, GameObject prefab) {
  if (GameObjects::Renderer == nullptr) throw std::runtime_error("A SpriteRenderer must be set for GameObjects::Renderer\n");
  if (Prefabs.find(handle) != Prefabs.end()) throw std::runtime_error("Another Prefab already exists with the same handle as " + handle + "'\n");

  prefab.handle = handle;
  Prefabs[handle] = prefab;
  return &Prefabs[handle];
}

GameObject *GameObjects::create(std::string handle, std::vector<Texture> texture, std::vector<std::string> tags, Transform transform) {
  if (GameObjects::Renderer == nullptr) throw std::runtime_error("A SpriteRenderer must be set for GameObjects::Renderer\n");

  GameObject object = GameObject();
  object.handle = handle;
  object.id = instantiation_id;
  object.texture = (std::vector<Texture>) { texture };
  object.tags = tags;
  object.transform = transform;
  object.update_bounding_box();

  instantiation_id++;

  Objects[object.id] = object;
  return &Objects[object.id];
}

GameObject *GameObjects::create(std::string handle, Texture texture, std::vector<std::string> tags, Transform transform) {
  return GameObjects::create(handle, { texture }, tags, transform);
}

GameObject *GameObjects::instantiate(std::string prefab_handle) {
  if (Prefabs.find(prefab_handle) == Prefabs.end()) throw std::runtime_error("[ERROR] Prefab with handle '" + prefab_handle + "' doesn't exist!");

  GameObject *prefab = GameObjects::ObjectPrefabs::get(prefab_handle);
  prefab->id = instantiation_id;
  prefab->active = true;

  instantiation_id++;

  Objects[prefab->id] = *prefab;
  return &Objects[prefab->id];
}

GameObject *GameObjects::instantiate(GameObject prefab) {
  prefab.id = instantiation_id;
  prefab.active = true;

  instantiation_id++;

  Objects[prefab.id] = prefab;
  return &Objects[prefab.id];
}

GameObject *GameObjects::instantiate(std::string prefab_handle, Transform transform) {
  if (Prefabs.find(prefab_handle) == Prefabs.end()) throw std::runtime_error("[ERROR] Prefab with handle '" + prefab_handle + "' doesn't exist!");

  GameObject *prefab = GameObjects::ObjectPrefabs::get(prefab_handle);
  prefab->active = true;
  prefab->transform = transform;
  prefab->update_bounding_box();
  prefab->id = instantiation_id;
  instantiation_id++;

  Objects[prefab->id] = *prefab;

  for (GameObject *&child : prefab->children) {
    GameObject *c = GameObjects::instantiate(*child);
    c->set_parent(&Objects[prefab->id]);
    c->translate(prefab->transform.position);
  }

  return &Objects[prefab->id];
}

GameObject *GameObjects::instantiate(GameObject prefab, Transform transform) {
  prefab.active = true;
  prefab.transform = transform;
  prefab.update_bounding_box();
  prefab.id = instantiation_id;
  instantiation_id++;

  Objects[prefab.id] = prefab;
  return &Objects[prefab.id];
}

void GameObjects::uninstantiate(std::string handle) {
  for (GameObject *&object : GameObjects::all()) 
    if (object->handle == handle) 
      Objects.erase(Objects.find(object->id));
}

void GameObjects::uninstantiate(unsigned long id) {
  if (id <= instantiation_id && Objects.find(id) != Objects.end()) 
    Objects.erase(id);
}


std::vector<GameObject *> GameObjects::all() {
  std::vector<GameObject *> all_objects;

  // Get all objects if they are active
  for (auto &pair : Objects) {
    if (pair.second.active) {
      all_objects.push_back(&pair.second);
    }
  }
  return all_objects;
}

GameObject *GameObjects::get(std::string handle) {
  for (GameObject *&object : GameObjects::all()) {
    if (object->handle == handle)
      return &Objects[object->id];
  }
}

GameObject *GameObjects::ObjectPrefabs::get(std::string handle) {
  if (Prefabs.find(handle) == Prefabs.end()) throw std::runtime_error("Prefab with handle '" + handle + "' does not exist!");
  return &Prefabs[handle];
}

std::vector<GameObject *> GameObjects::filter(std::vector<std::string> tags) {
  std::vector<GameObject *> filtered_objects;

  // For each object, if the object is active, then check if it has all the tags
  // if it doesn't, then just skip that object. Otherwise, add that object to the
  // output vector
  for (auto &pair : Objects) {
    if (pair.second.active) {
      bool contains_tags = true;
      for (std::string tag : tags) {
        // If even one of the tag is not found in the GameObject, then ignore the object
        if (std::find(pair.second.tags.begin(), pair.second.tags.end(), tag) == pair.second.tags.end()) {
          contains_tags = false;
          break;
        }
      }
      if (contains_tags) filtered_objects.push_back(&pair.second);
    }
  }
  return filtered_objects;
}

std::vector<GameObject *> GameObjects::filter(std::string tag) {
  std::vector<GameObject *> filtered_objects;

  // For each object, if the object is active, check all its tags and if
  // it has the same tag as the one required, then add it to the output vector
  for (auto &pair : Objects) {
    if (pair.second.active)
      for (std::string o_tag : pair.second.tags)
        if (tag == o_tag)
          filtered_objects.push_back(&pair.second);
  }
  return filtered_objects;
}

std::vector<GameObject *> GameObjects::except(std::string tag) {
  std::vector<GameObject *> filtered_objects;

  // For each GameObject, if it is active, check all its tags against the filter tag.
  // If the tag isn't in linked with the object, then just ignore that object. Otherwise, 
  // add that object to the output vector
  for (auto &pair : Objects) {
    if (pair.second.active) {
      bool found = true;
      for (std::string o_tag : pair.second.tags) {
        if (tag == o_tag) {
          found = false;
          break;
        }
      }
      if (found) filtered_objects.push_back(&pair.second);
    }
  }
  return filtered_objects;
}

void p_error(std::string error, bool fail = true) {
  printf("[R* PARSING ERROR]\n  %s\n", error.c_str());
  if (fail) exit(0);
}

std::vector<std::string> p_csstr(std::string list) {
  int pos = list.find(",");
  std::vector<std::string> out;

  while (pos != std::string::npos) {
    pos = list.find(",");
    if (pos == std::string::npos) break;
    out.push_back(list.substr(0, pos));
    // printf("tag: %s\n", list.substr(0, pos).c_str());
    list.erase(0, pos + 1);
  }
  // printf("tag: %s\n", list.c_str());
  out.push_back(list);
  return out;
}

int line_num = 0;

std::vector<float> p_csfloat(std::string list, std::map<std::string, float> constants) {
  int pos = list.find(",");
  std::vector<float> out;

  while (pos != std::string::npos) {
    pos = list.find(",");
    if (pos == std::string::npos) break;
    std::string val = list.substr(0, pos);
    if (val.substr(0, 1) == "*") {
      try {
        out.push_back(static_cast<float>(constants[val]));
      } catch (...) {
        p_error("Invalid syntax at line " + std::to_string(line_num) + " (invalid constant)");
      }
    } else {
      try {
        out.push_back(std::stof(val));
      } catch (...) {
        p_error("Invalid syntax at line " + std::to_string(line_num) + " (invalid symbol)");
      }
    }

    // printf("tag: %s\n", list.substr(0, pos).c_str());
    list.erase(0, pos + 1);
  }

  if (list.substr(0, 1) == "*") {
    try {
      out.push_back(static_cast<float>(constants[list]));
    } catch (...) {
      p_error("Invalid syntax at line " + std::to_string(line_num) + " (invalid constant)");
    }
  } else {
    try {
      out.push_back(std::stof(list));
    } catch (...) {
      p_error("Invalid syntax at line " + std::to_string(line_num) + " (invalid symbol)");
    }
  }

  return out;
}

void GameObjects::ObjectPrefabs::load_from_file(const char *file_path) {
  std::ifstream file(file_path);
  std::string line;

  // Variables for the scripting language
  bool fail_on_texture_not_found = true;
  bool editing_object = false;
  int objects_loaded = 0;

  glm::vec2 TileSize = glm::vec2(GameObjects::Camera->width / 3.0f, GameObjects::Camera->height / 2.0f);
  float ratio = (float)WindowSize.y / 8.85f;
  GameObject *default_object = GameObjects::ObjectPrefabs::create("default", ResourceManager::Texture::get("blank"), {}, Transform());
  GameObject *object = default_object;

  #define DEBUG true
  #define DEBUG_LEVEL 5

  #define SX object->transform.scale.x
  #define SY object->transform.scale.y
  #define PX object->transform.position.x
  #define PY object->transform.position.y

  std::map<std::string, float> constants;
  constants["*TSX"] = TileSize.x;
  constants["*TSY"] = TileSize.y;
  constants["*TSX/2"] = TileSize.x / 2.0f;
  constants["*TSY/2"] = TileSize.y / 2.0f;
  constants["*TSY-R"] = TileSize.y - ratio;
  constants["*TSY-R-SY"] = TileSize.y - ratio - SY;
  constants["*TXSC"] = (TileSize.x / 2.0f) - (SX / 2.0f);
  constants["*TYSC"] = (TileSize.y / 2.0f) - (SY / 2.0f);
  constants["*R"] = ratio;
  constants["*SX"] = SX;
  constants["*SY"] = SY;
  constants["*PX"] = PX;
  constants["*PY"] = PY;

  if (DEBUG && DEBUG_LEVEL >= 2) printf("\n");

  // std::getline(file, line);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      line_num++;
      // Erase all spaces from the file
      line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char x) { return std::isspace(x); }), line.end());
      if (line.find("//") == 0 || !line.size()) continue;

      // Parses the file
      if (line.substr(0, 1) == "%") {
        line.erase(0, 1);
        int pos = line.find("=");
        if (pos != std::string::npos) {
          line.erase(0, pos + 1);
          if (line == "failure") fail_on_texture_not_found = true;
          else if (line == "placeholder") fail_on_texture_not_found = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (invalid value)");
        } else p_error("Invalid syntax at line " + std::to_string(line_num) + " (missing '=')");
      } else if (line.substr(0, 1) == "#") {
        line.erase(0, 1);
        try {
          ResourceManager::Texture::get(line);
        } catch (...) { 
          if (fail_on_texture_not_found) p_error("TEXTURE_FAILURE at line " + std::to_string(line_num) + " (texture '" + line + "' not found)"); 
        }
      } else if (line.substr(0, 1) == "$") {
        if (editing_object) p_error("Invalid syntax at line " + std::to_string(line_num) + " (cannot have nested object declarations)");

        line.erase(0, 1);
        int pos = line.find("{");
        if (pos != std::string::npos) line.erase(pos, 1);

        int dpos = line.find(":");
        if (dpos != std::string::npos) {
          std::string derived_from = line.substr(dpos + 1, line.find("{"));
          std::string obj_name = line.substr(0, dpos);
          object = GameObjects::ObjectPrefabs::create(obj_name.c_str(), *GameObjects::ObjectPrefabs::get(derived_from));
        } else {
          object = GameObjects::ObjectPrefabs::create(line.c_str(), *default_object);
        }

        if (pos != std::string::npos) {
          editing_object = true;
        } else {
          editing_object = false;
        }
      } else if (editing_object) {
        int pos = line.find("=");
        std::string substr = line.substr(0, pos);
        if (DEBUG && DEBUG_LEVEL >= 4 && substr != "}") printf("substr: %s\n", substr.c_str());
        if (substr == "texture") {
          std::vector<std::string> cs_textures = p_csstr(line.substr(pos + 1));
          std::vector<Texture> textures;
          for (std::string texture : cs_textures) {
            try {
               textures.push_back(ResourceManager::Texture::get(texture));
            } catch (...) {
              if (fail_on_texture_not_found) p_error("Invalid syntax at line " + std::to_string(line_num) + " ('" + texture.c_str() + "' unknown texture)");
            }
          }
          object->texture = textures;
        } else if (substr == "tags") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<std::string> tags = p_csstr(line);
          object->tags.insert(object->tags.begin(), tags.begin(), tags.end());
          if (DEBUG && DEBUG_LEVEL >= 5) for (std::string tag : tags) printf(" tag: %s\n", tag.c_str());
        } else if (substr == "position") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> position = p_csfloat(line, constants);
          try {
            object->transform.position = glm::vec3(position[0], position[1], position[2]); 
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "scale") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> scale = p_csfloat(line, constants);
          try {
            object->transform.scale = glm::vec2(scale[0], scale[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "origin") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> origin = p_csfloat(line, constants);
          try {
            object->origin = glm::vec2(origin[0], origin[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "grid") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> grid = p_csfloat(line, constants);
          try {
            object->grid = glm::vec2(grid[0], grid[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "position-offset") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> position_offset = p_csfloat(line, constants);
          try {
            object->position_offset = glm::vec3(position_offset[0], position_offset[1], position_offset[2]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }

        } else if (substr == "interactive") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->interactive = true;
          else if (line == "false") object->interactive = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "swap") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->swap = true;
          else if (line == "false") object->swap = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "locked") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->locked = true;
          else if (line == "false") object->locked = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "rigidbody") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->rigidbody = true;
          else if (line == "false") object->rigidbody = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "parent") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");
          
          try {
            object->set_parent(GameObjects::ObjectPrefabs::get(line));
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (parent does not exist)");
          }
        } else if (substr == "}") {
          objects_loaded++;
          editing_object = false;
          if (DEBUG && DEBUG_LEVEL >= 2) printf("Loaded object '%s' from file!\n\n", object->handle.c_str());
        } else {
          p_error("Invalid syntax at line " + std::to_string(line_num) + " ('" + substr + "' unknown attribute)");
        }
      } else p_error("Invalid syntax at line " + std::to_string(line_num) + " (UNKNOWN SYNTAX)");
    }
  }

  if (editing_object) p_error("EOF before object was fully initialised (missing '}')");

  if (DEBUG && DEBUG_LEVEL >= 2) printf("Loaded %i objects from file!\n", objects_loaded);
  // exit(0);
}

