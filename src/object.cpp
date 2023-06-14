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
  // if (this->tags[0] == "tile") n_transform.scale += glm::vec2(2.0f);

  if (this->active) GameObjects::Renderer->render(this->texture, n_transform, colour, focus);
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

    CollisionInfo vertical, horizontal;
    vertical.collision = (object->bounding_box.bottom >= this->bounding_box.top && object->bounding_box.top <= this->bounding_box.bottom);
    vertical.direction = vector_direction(glm::vec2(0.0f, difference.y));
    vertical.mtv = difference.y + (difference.y > 0 ? (this->transform.scale.y / -2.0f) + object->transform.scale.y : (this->transform.scale.y / 2.0f));

    horizontal.collision = (object->bounding_box.right >= this->bounding_box.left && object->bounding_box.left <= this->bounding_box.right);
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
  if (GameObjects::Renderer == nullptr) throw std::runtime_error("A SpriteRenderer must be set for GameObjects::Renderer\n");
  if (Prefabs.find(handle) != Prefabs.end()) throw std::runtime_error("Another Prefab already exists with the same handle!\n");

  GameObject object = GameObject();
  object.handle = handle;
  object.texture = texture;
  object.tags = tags;
  object.transform = transform;
  object.active = false;
  object.update_bounding_box();

  Prefabs[handle] = object;
  return &Prefabs[handle];
}

GameObject *GameObjects::create(std::string handle, Texture texture, std::vector<std::string> tags, Transform transform) {
  if (GameObjects::Renderer == nullptr) throw std::runtime_error("A SpriteRenderer must be set for GameObjects::Renderer\n");

  GameObject object = GameObject();
  object.handle = handle;
  object.id = instantiation_id;
  object.texture = texture;
  object.tags = tags;
  object.transform = transform;
  object.update_bounding_box();

  instantiation_id++;

  Objects[object.id] = object;
  return &Objects[object.id];
}

GameObject *GameObjects::instantiate(const char *prefab_handle) {
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

GameObject *GameObjects::instantiate(const char *prefab_handle, Transform transform) {
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

// GameObject *GameObjects::get(const char *handle) {
//   return &GameObjects::Objects[handle];
// }

GameObject *GameObjects::ObjectPrefabs::get(std::string handle) {
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
