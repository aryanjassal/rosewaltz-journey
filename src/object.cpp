#include "object.h"

void GameObject::render(SpriteRenderer *renderer, glm::vec4 colour) {
  Transform n_transform = this->transform;
  if (this->position_offset != glm::vec3(0.0f)) n_transform.position += this->position_offset;

  if (this->active) renderer->render(this->texture, n_transform, colour);
}

void GameObject::translate_to_point(glm::vec2 point, bool convert) {
  // Set some useful variables
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  // Convert the mouse coordinates to camera-space
  if (convert) point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  // Update the transform
  this->transform.position = glm::vec3(point - origin, 0.0f);

  // Snap the object and update its bounding box
  if (this->snap) this->update_snap_position();
  else this->update_bounding_box();
}

bool GameObject::check_point_intersection(glm::vec2 point, bool convert) {
  // Convert the screen-space coordinate to camera-space coordinates
  if (convert) point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  return ((this->bounding_box.left <= point.x) 
    && (this->bounding_box.right >= point.x) 
    && (this->bounding_box.top <= point.y) 
    && (this->bounding_box.bottom >= point.y));
}

Collision GameObject::check_collision(GameObject *object) {
  // printf("[%s] [collision] %.2f < x < %.2f; %.2f < y < %.2f [%.2f < x < %.2f; %.2f < y < %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, object->bounding_box.left, object->bounding_box.right, object->bounding_box.top, object->bounding_box.bottom);

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

    return { true, vertical, horizontal };
  }

  CollisionInfo vertical, horizontal;
  return { false, vertical, horizontal };
}

void GameObject::update_bounding_box() {
  // Use the origin if originate is set, otherwise remove it from any calculations
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  // Use the offset transform to calculate the bounding boxes
  Transform n_transform = this->transform;
  if (this->position_offset != glm::vec3(0.0f)) n_transform.position += this->position_offset;

  // Update the bounding boxes using some super advanced math
  this->bounding_box.right = n_transform.position.x + this->transform.scale.x + origin.x;
  this->bounding_box.left = n_transform.position.x + origin.x;
  this->bounding_box.bottom = n_transform.position.y + this->transform.scale.y + origin.y;
  this->bounding_box.top = n_transform.position.y + origin.y;
  
  // // DEBUG print the bounding box of the object along with its handle
  // if (this->tags[0] != "player") printf("[%s] [collider] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x, this->transform.position.y);
}

void GameObject::update_snap_position() {
  // If snapping is disabled in the object settings, then do nothing
  if (!this->snap) return;

  // Otherwise, update the position to snap to the nearest snap point
  glm::vec3 old_pos = this->transform.position;
  glm::vec2 origin = this->origin;
  glm::vec3 new_position;
  new_position.x = std::floor((this->transform.position.x + origin.x) / this->grid.x) * this->grid.x;
  new_position.y = std::floor((this->transform.position.y + origin.y) / this->grid.y) * this->grid.y;

  // If the new position is outside the dimensions, then just undo any translations and return it to its old position
  if (new_position.x < 0 || new_position.x > this->camera->width - this->grid.x || new_position.y < 0 || new_position.y > this->camera->height - this->grid.y) {
    this->transform.position = this->old_transform.position;
    this->update_bounding_box();
    return;
  }

  // Otherwise, update the delta transform, the object's position, and it's bounding box
  this->delta_transform.position = this->transform.position - old_pos; 
  this->transform.position = new_position;
  this->update_bounding_box();

  // //DEBUG print the bounding box of the object along with its handle
  // printf("[%s] [snap] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x, this->transform.position.y);
}

GameObject *GameObjects::create(
  std::string handle, 
  OrthoCamera *camera, 
  Texture texture, 
  glm::vec2 window_dimensions, 
  std::vector<std::string> tags,
  Transform transform,
  glm::vec2 origin,
  glm::vec2 grid) {
    GameObject object = GameObject();
    object.handle = handle;
    object.camera = camera;
    object.texture = texture;
    object.window_dimensions = window_dimensions;
    object.tags = tags;
    object.transform = transform;
    object.origin = origin;
    object.originate = false;
    object.locked = false;
    object.bounding_box = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (grid != glm::vec2(0.0f)) {
      object.grid = grid;
      object.snap = true;
      object.update_snap_position();
    } else {
      // object.update_position();
      object.translate_to_point(object.transform.position);
    }

    GameObjects::Objects[handle] = object;
    return &GameObjects::Objects[handle];
}

GameObject *GameObjects::create(
  std::string handle, 
  OrthoCamera *camera, 
  Texture texture, 
  glm::vec2 window_dimensions, 
  std::vector<std::string> tags,
  glm::vec3 position,
  glm::vec2 scale,
  float rotation,
  glm::vec2 origin,
  glm::vec2 grid) {
    Transform transform;
    transform.position = position;
    transform.scale = scale;
    transform.rotation = rotation;
    return GameObjects::create(handle, camera, texture, window_dimensions, tags, transform, origin, grid);
}

std::vector<GameObject *> GameObjects::all() {
  std::vector<GameObject *> all_objects;

  // Get all objects if they are active
  for (auto &pair : GameObjects::Objects) {
    if (pair.second.active) {
      all_objects.push_back(&pair.second);
    }
  }
  return all_objects;
}

GameObject *GameObjects::get(std::string handle) {
  return &GameObjects::Objects[handle];
}

std::vector<GameObject *> GameObjects::filter(std::vector<std::string> tags) {
  std::vector<GameObject *> filtered_objects;

  // For each object, if the object is active, then check if it has all the tags
  // if it doesn't, then just skip that object. Otherwise, add that object to the
  // output vector
  for (auto &pair : GameObjects::Objects) {
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
  for (auto &pair : GameObjects::Objects) {
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
  for (auto &pair : GameObjects::Objects) {
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

Direction vector_direction(glm::vec2 target) {
  glm::vec2 compass[] = {
    glm::vec2(0.0f, 1.0f),	// up
    glm::vec2(1.0f, 0.0f),	// right
    glm::vec2(0.0f, -1.0f),	// down
    glm::vec2(-1.0f, 0.0f)	// left
  };

  float max = 0.0f;
  unsigned int match = -1;

  for (int i = 0; i < 4; i++) {
    float dot = glm::dot(glm::normalize(target), compass[i]);
    if (dot > max) {
      max = dot;
      match = i;
    }
  }

  return (Direction)match;
}
