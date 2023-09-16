#include "object.h"
#include "physics.h"

// Publically store the active Camera and the Renderer
Camera *Entities::Camera = nullptr;
Renderer *Entities::Renderer = nullptr;

// Store a list of all the GameObjects and Prefabs ever created
std::map<unsigned long long, Entity> AllEntities;
std::map<std::string, Entity> AllPrefabs;

// Counter to keep track of the next ID for instantiated GameObjects
static unsigned long long instantiation_id = 0;

// Constructor of the entity class
Entity::Entity() {
  // Define the id number of the entity
  this->id = instantiation_id;
  instantiation_id++;

  // Ensure at least one collider is present
  create_colliders(1);
}

// Constructor of the entity class
Entity::Entity(std::string handle, TextureMap texture_map, std::vector<std::string> tags, Transform transform) {
  // Define the id number of the entity
  this->id = instantiation_id;
  instantiation_id++;

  // Ensure at least one collider is present
  create_colliders(1);

  // Assign attributes
  this->handle = handle;
  this->texture_map = texture_map;
  this->tags = tags;
  this->transform = transform;
}

// Render the current entity
void Entity::render(glm::vec4 colour) {
  if (this->active) {
    if (!Entities::Camera->in_range(this->transform.position, this->transform.z)) 
      printf("[WARN] [%i](%s)'s z-index (%.4f) is out of the camera's bounds (%.4f, %.4f)\n", this->id, this->handle.c_str(), this->transform.z, Entities::Camera->near_plane(), Entities::Camera->far_plane());

    Entities::Renderer->render(this->texture_map.texture(), this->transform, colour);

    // Show colliders
    if (this->debug_colliders) {
      for (Collider collider : this->colliders) {
        Transform transform;
        transform.position = glm::vec3(collider.x, collider.y, 0.0f);
        transform.scale = glm::vec2(collider.w, collider.h);

        glm::vec4 colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
        Entities::Renderer->render(ResourceManager::Texture::get("blank"), transform, colour);
      }

      Transform transform;
      transform.position = this->transform.position + this->transform.origin;
      transform.scale = glm::vec2(10.0f);

      glm::vec4 origin_colour = glm::vec4(1.0f, 0.3f, 0.3f, 0.7f);
      Entities::Renderer->render(ResourceManager::Texture::get("blank"), transform, origin_colour);
    }
  }
}

// Translate the object to a point on the game window
void Entity::translate(glm::vec2 point) {
  this->transform.position = point - this->transform.origin;
  this->update_colliders();
}

// Check collision between this object and a point
Collision Entity::check_collision(glm::vec2 point) {
  for (Collider collider : this->colliders) {
    Collision collision = Physics::collider_point_collision(collider, point);
    if (collision) return collision;
  }
  return Collision();
}

// Check collision between an object and this
Collision Entity::check_collision(Entity *object) {
  for (Collider t_collider : this->colliders) {
    for (Collider o_collider : object->colliders) {
      Collision collision = Physics::collider_collider_collision(t_collider, o_collider);
      if (collision) return collision;
    }
  }
  return Collision();
}

// Update all the colliders of the object
void Entity::update_colliders() {
  glm::vec2 collider_pos = this->transform.position;
  for (int i = 0; i < this->colliders.size(); i++) {
    Collider collider = this->colliders.at(i);
    this->update_colliders(i, collider_pos.x, collider_pos.y, collider.w <= 0 ? this->transform.scale.x : collider.w, collider.h <= 0 ? this->transform.scale.y : collider.h);
  }
}

// Update a collider at the specified index
void Entity::update_colliders(int i, float x, float y, float w, float h) {
  if (this->colliders.size() <= i) throw std::runtime_error("Collider at index " + std::to_string(i) + " hasn't been declared yet\n");
  this->colliders.at(i) = Collider(x, y, w, h);
}

void Entity::snap() {
  glm::vec2 origin = this->transform.origin;
  glm::vec3 new_position;
  new_position.x = std::floor((this->transform.position.x + origin.x) / this->grid.x) * this->grid.x;
  new_position.y = std::floor((this->transform.position.y + origin.y) / this->grid.y) * this->grid.y;

  // If the new position is outside the dimensions, then just undo any translations and return it to its old position
  if (new_position.x < 0 || new_position.x > Entities::Camera->dimensions.x - this->grid.x || new_position.y < 0 || new_position.y > Entities::Camera->dimensions.y - this->grid.y) {
    this->transform.position = this->old_transform.position;
    this->update_colliders();
    return;
  }

  // Otherwise, update the delta transform, the object's position, and it's bounding box
  this->transform.position = new_position;
  this->update_colliders();
}

// Set the parent of an entity to the provided parent
void Entity::set_parent(Entity *parent) {
  if (parent != nullptr) {
    this->unset_parent();
    this->parent = parent;
    parent->children.push_back(this);
  } else {
    this->unset_parent();
  }
}

// Unset the parent of this object
void Entity::unset_parent() {
  try {
    if (this->parent != nullptr) {
      if (std::find(this->parent->children.begin(), this->parent->children.end(), this) != this->parent->children.end()) {
        this->parent->children.erase(std::find(this->parent->children.begin(), this->parent->children.end(), this));
      }
      this->parent = nullptr;
    }
  } catch (...) {
    warn("Could not properly unset parent of object (" + std::to_string(this->id) + ") " + this->handle);
    this->parent = nullptr;
  }
}

// Set an object as the child of this object
void Entity::set_child(Entity *child) {
  if (child != nullptr) {
    child->parent = this;
    this->children.push_back(child);
  }
}

// Remove a particular object as this object's child
void Entity::unset_child(Entity *child) {
  if (child != nullptr) {
    this->children.erase(std::find(this->children.begin(), this->children.end(), child));
    child->parent = nullptr;
  }
}

// Create a number of default colliders
void Entity::create_colliders(int n) {
  for (int i = 0; i < n; i++)
    this->colliders.push_back(Collider());
}

// Create a number of colliders with the given parameters
void Entity::create_colliders(int n, float x, float y, float w, float h) {
  for (int i = 0; i < n; i++) {
    this->colliders.push_back(Collider(x, y, w, h));
  }
}

// Create a number of colliders with the given parameters
void Entity::create_colliders(int n, float x, float y, float w, float h, float angle) {
  for (int i = 0; i < n; i++) {
    this->colliders.push_back(Collider(x, y, w, h, angle));
  }
}

// Validate whether the state of the renderer and the handle are valid
void validate_state(std::string handle) {
  if (Entities::Renderer == nullptr) throw std::runtime_error("A Renderer must be set for Entities::Renderer\n");
  if (AllPrefabs.find(handle) != AllPrefabs.end()) throw std::runtime_error("Another prefab already exists with the same handle as '" + handle + "'\n");
}

// Validate whether the state of the renderer and the handle are valid
void validate_state() {
  if (Entities::Renderer == nullptr) throw std::runtime_error("A Renderer must be set for Entities::Renderer\n");
}

// Create a new entity using the gievn parameters and returns it
Entity create_entity(std::string handle, TextureMap texture_map, std::vector<std::string> tags, Transform transform) {
  validate_state(handle);

  Entity entity = Entity();
  entity.handle = handle;
  entity.texture_map = texture_map;
  entity.tags = tags;
  entity.transform = transform;
  entity.active = false;
  entity.update_colliders();
  return entity;
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, TextureMap texture_map) {
  AllPrefabs[handle] = create_entity(handle, texture_map, {}, Transform());
  return &AllPrefabs[handle];
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, Texture texture) {
  AllPrefabs[handle] = create_entity(handle, TextureMap(texture), {}, Transform());
  return &AllPrefabs[handle];
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, TextureMap texture_map, std::vector<std::string> tags, Transform transform) {
  AllPrefabs[handle] = create_entity(handle, texture_map, tags, transform);
  return &AllPrefabs[handle];
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, Texture texture, std::vector<std::string> tags, Transform transform) {
  return Entities::Prefabs::create(handle, TextureMap(texture), tags, transform);
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, Texture texture, std::string tag, Transform transform) {
  return Entities::Prefabs::create(handle, TextureMap(texture), std::vector<std::string>(1, tag), transform);
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, TextureMap texture_map, std::string tag, Transform transform) {
  return Entities::Prefabs::create(handle, texture_map, std::vector<std::string>(1, tag), transform);
}

// Create a new prefab
Entity *Entities::Prefabs::create(std::string handle, Entity prefab) {
  validate_state(handle);

  prefab.handle = handle;
  AllPrefabs[handle] = prefab;
  return &AllPrefabs[handle];
}

// Instantiate a prefab using a given prefab handle (as handles cannot be duplicated)
Entity instantiate_entity(std::string prefab_handle) {
  if (AllPrefabs.find(prefab_handle) == AllPrefabs.end()) error("Prefab with handle '" + prefab_handle + "' doesn't exist!");

  Entity *prefab = Entities::Prefabs::get(prefab_handle);
  prefab->id = instantiation_id++;
  prefab->active = true;

  AllEntities[prefab->id] = *prefab;
  return AllEntities[prefab->id];
}

// Instantiate all the children of a given prefab
void instantiate_children(Entity parent) {
  for (Entity *&child : parent.children) {
    Entity *c = Entities::instantiate(*child);
    c->set_parent(&AllEntities[parent.id]);
    c->translate(parent.transform.position);
  }
}

// Instantiate a prefab using an existing prefab
Entity instantiate_entity(Entity prefab) {
  prefab.id = instantiation_id++;
  prefab.active = true;

  instantiate_children(prefab);

  AllEntities[prefab.id] = prefab;
  return AllEntities[prefab.id];
}

// Instantiate a prefab using its handle
Entity *Entities::instantiate(std::string prefab_handle) {
  Entity instance = instantiate_entity(prefab_handle);
  return &AllEntities[instance.id];
}

// Instantiate a prefab using an existing prefab
Entity *Entities::instantiate(Entity prefab) {
  Entity instance = instantiate_entity(prefab);
  return &AllEntities[instance.id];
}

// Instantiate a prefab using its handle and a custom transform
Entity *Entities::instantiate(std::string prefab_handle, Transform transform) {
  Entity instance = instantiate_entity(prefab_handle);
  instance.transform = transform;
  instance.update_colliders();
  AllEntities[instance.id] = instance;

  return &AllEntities[instance.id];
}

// Instantiate a prefab using an existing prefab and a custom transform
Entity *Entities::instantiate(Entity prefab, Transform transform) {
  Entity instance = instantiate_entity(prefab);
  instance.transform = transform;
  instance.update_colliders();
  AllEntities[instance.id] = instance;

  return &AllEntities[instance.id];
}

// Uninstantiate all entities matching a given handle
// When you remove the entity from all entities, it stops getting updated/rendered
void Entities::uninstantiate(std::string handle) {
  for (Entity *&entity : Entities::all()) 
    if (entity->handle == handle) 
      AllEntities.erase(AllEntities.find(entity->id));
}

// Uninstantiate all entities matching a given handle
// When you remove the entity from all entities, it stops getting updated/rendered
void Entities::uninstantiate(unsigned long id) {
  if (AllEntities.find(id) != AllEntities.end()) 
    AllEntities.erase(id);
}

// Get all the active entities currently being kept track of
std::vector<Entity*> Entities::all() {
  std::vector<Entity*> all_objects;

  // Get all objects if they are active
  for (auto &pair : AllEntities) {
    if (pair.second.active) {
      all_objects.push_back(&pair.second);
    }
  }
  return all_objects;
}

// Get an entity which has the required handle
Entity *Entities::get(std::string handle) {
  for (Entity *&object : Entities::all()) {
    if (object->handle == handle)
      return &AllEntities[object->id];
  }
}

// Get a prefab using its handle
Entity *Entities::Prefabs::get(std::string handle) {
  if (AllPrefabs.find(handle) == AllPrefabs.end()) warn("Prefab with handle '" + handle + "' does not exist!");
  return &AllPrefabs[handle];
}

// Get a vector of entities matching the vector of tags
std::vector<Entity*> Entities::filter(std::vector<std::string> tags) {
  std::vector<Entity*> filtered_objects;

  // For each object, if the object is active, then check if it has all the tags
  // if it doesn't, then just skip that object. Otherwise, add that object to the
  // output vector
  for (auto &pair : AllEntities) {
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

// Get a vector of entities matching the tag
std::vector<Entity*> Entities::filter(std::string tag) {
  std::vector<Entity*> filtered_objects;

  // For each object, if the object is active, check all its tags and if
  // it has the same tag as the one required, then add it to the output vector
  for (auto &pair : AllEntities) {
    if (pair.second.active)
      for (std::string o_tag : pair.second.tags)
        if (tag == o_tag)
          filtered_objects.push_back(&pair.second);
  }
  return filtered_objects;
}

// Get a vector of entities which do not match the tag
std::vector<Entity*> Entities::except(std::string tag) {
  std::vector<Entity*> filtered_objects;

  // For each GameObject, if it is active, check all its tags against the filter tag.
  // If the tag isn't in linked with the object, then just ignore that object. Otherwise, 
  // add that object to the output vector
  for (auto &pair : AllEntities) {
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
  if (!fail) return;
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

// PARSE PREFABS FILE
int line_num = 0;

std::vector<float> p_csfloat(std::string list, Entity *object) {
  // glm::vec2 TileSize = glm::vec2(Entities::Camera->dimensions.x / 3.0f, Entities::Camera->height / 2.0f);
  glm::vec2 TileSize = Entities::Camera->dimensions / glm::vec2(3.0f, 2.0f);
  float ratio = (float)WindowSize.y / 8.85f;
  
  std::map<std::string, float> constants;
  constants["*TSX"] = TileSize.x;
  constants["*TSY"] = TileSize.y;
  constants["*TSX/2"] = TileSize.x / 2.0f;
  constants["*TSY/2"] = TileSize.y / 2.0f;
  constants["*TSX/3"] = TileSize.x / 3.0f;
  constants["*TSX-TSX/3"] = TileSize.x - (TileSize.x / 3.0f);
  constants["*TSY/3"] = TileSize.y / 3.0f;
  constants["*TSY-R"] = TileSize.y - ratio;
  constants["*TSX-SX"] = TileSize.x - object->transform.scale.x;
  constants["*TSY-R-SY"] = TileSize.y - ratio - object->transform.scale.y;
  constants["*TXSC"] = (TileSize.x / 2.0f) - (object->transform.scale.x / 2.0f);
  constants["*TYSC"] = (TileSize.y / 2.0f) - (object->transform.scale.y / 2.0f);
  constants["*R"] = ratio;
  constants["*SX"] = object->transform.scale.x;
  constants["*SY"] = object->transform.scale.y;
  constants["*PX"] = object->transform.position.x;
  constants["*PY"] = object->transform.position.y;

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

void Entities::Prefabs::load_from_file(const char *file_path) {
  std::ifstream file(file_path);
  std::string line;

  // Variables for the scripting language
  bool fail_on_texture_not_found = true;
  bool editing_object = false;
  int objects_loaded = 0;

  #define DEBUG false
  #define DEBUG_LEVEL 5

  Entity *default_object = new Entity();
  Entity *object = default_object;

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
          object = Entities::Prefabs::create(obj_name.c_str(), *Entities::Prefabs::get(derived_from));
          objects_loaded++;
          if (DEBUG && DEBUG_LEVEL >= 2) printf("Loaded object '%s' from file!\n\n", object->handle.c_str());
          if (pos == std::string::npos) continue;
        } else {
          object = Entities::Prefabs::create(line.c_str(), *default_object);
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
          object->texture_map = TextureMap(textures);
        } else if (substr == "tags") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<std::string> tags = p_csstr(line);
          object->tags.insert(object->tags.begin(), tags.begin(), tags.end());
          if (DEBUG && DEBUG_LEVEL >= 5) for (std::string tag : tags) printf(" tag: %s\n", tag.c_str());
        } else if (substr == "position") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> position = p_csfloat(line, object);
          try {
            object->transform.position = glm::vec2(position[0], position[1]); 
            object->transform.z = position[2];
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "scale") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> scale = p_csfloat(line, object);
          try {
            object->transform.scale = glm::vec2(scale[0], scale[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "origin") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> origin = p_csfloat(line, object);
          try {
            object->transform.origin = glm::vec2(origin[0], origin[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "grid") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          std::vector<float> grid = p_csfloat(line, object);
          try {
            object->grid = glm::vec2(grid[0], grid[1]);
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          }
        } else if (substr == "position-offset") {
          p_error("WARNING: at line " + std::to_string(line_num) + ": 'position-offset' is deprecated!", false);
          // line.erase(0, pos + 1);
          // if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (list cannot be empty)");
          // std::vector<float> position_offset = p_csfloat(line, object);
          // try {
          //   // object->position_offset = glm::vec3(position_offset[0], position_offset[1], position_offset[2]);
          //   // p_error("WARNING: at line " + std::to_string(line_num) + ": 'position-offset' is deprecated!");
          // } catch (...) {
          //   p_error("Invalid syntax at line " + std::to_string(line_num) + " (unsufficient parameters)");
          // }
        } else if (substr == "interactive") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->interactive = true;
          else if (line == "false") object->interactive = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "swap") {
          p_error("WARNING: at line " + std::to_string(line_num) + ": 'swap' is deprecated!", false);
          // line.erase(0, pos + 1);
          // if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");
          //
          // if (line == "true") object->swap = true;
          // else if (line == "false") object->swap = false;
          // else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "locked") {
          p_error("WARNING: at line " + std::to_string(line_num) + ": 'locked' is deprecated!", false);
          // line.erase(0, pos + 1);
          // if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");
          //
          // if (line == "true") object->locked = true;
          // else if (line == "false") object->locked = false;
          // else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "rigidbody") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->rigidbody = true;
          else if (line == "false") object->rigidbody = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "active") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");

          if (line == "true") object->active = true;
          else if (line == "false") object->active = false;
          else p_error("Invalid syntax at line " + std::to_string(line_num) + " (unknown value)");
        } else if (substr == "parent") {
          line.erase(0, pos + 1);
          if (!line.size()) p_error("Invalid syntax at line " + std::to_string(line_num) + " (attribute cannot be empty)");
          
          try {
            object->set_parent(Entities::Prefabs::get(line));
          } catch (...) {
            p_error("Invalid syntax at line " + std::to_string(line_num) + " (parent does not exist)");
          }
        } else if (substr == ">nochild") {
          object->children = std::vector<Entity*>();
        } else if (substr == ">showcollider") {
          object->debug_colliders = true;
        } else if (substr == "}") {
          object->update_colliders(0, object->transform.position.x, object->transform.position.y, object->transform.scale.x, object->transform.scale.y);
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

  // if (DEBUG && DEBUG_LEVEL >= 2) printf("Loaded %i objects from file!\n", objects_loaded);
  // printf("Loaded %i objects from file!\n", objects_loaded);

  // for (auto object : Prefabs) {
  //   auto o = object.second;
  //   printf("---[%s]---\n", o.handle.c_str());
  //   for (auto tag : o.tags) printf("[tag] %s\n", tag.c_str());
  //   printf("\n");
  // }
}

