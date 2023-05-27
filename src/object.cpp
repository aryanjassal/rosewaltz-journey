#include "object.h"

void GameObject::render(SpriteRenderer *renderer) {
  if (this->active) renderer->render(this->texture, this->transform);
}

void GameObject::translate_to_point(glm::vec2 point, bool convert) {
  // This will be used to set the delta transform of the object
  glm::vec2 old_pos = this->transform.position;

  // Convert the mouse coordinates to camera-space
  if (convert) point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  // Set the origin if originate is set, otherwise don't affect the calculations
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  // Actually change the transform
  this->transform.position = glm::vec3(point - origin, 0.0f);

  // Snap the object and update its bounding box
  if (this->snap) this->update_snap_position();
  else this->update_bounding_box();

  // Set the delta transform to be used for offsetting similarly tagged objects
  this->delta_transform.position = this->transform.position - glm::vec3(old_pos, 0.0f); 
}

void GameObject::update_position() {
  // This translates the object back to its position to recalculate everything else
  // Useful for (re)calculating bounding boxes snap or something else
  this->translate_to_point(this->transform.position, false);
}

bool GameObject::check_point_intersection(glm::vec2 point, bool convert) {
  // Convert the screen-space coordinate to camera-space coordinates
  if (convert) point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  return ((this->bounding_box.left <= point.x) 
    && (this->bounding_box.right >= point.x) 
    && (this->bounding_box.top <= point.y) 
    && (this->bounding_box.bottom >= point.y));
}

void GameObject::update_bounding_box() {
  // Use the origin if originate is set, otherwise remove it from any calculations
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  // Update the bounding boxes using some super advanced math
  this->bounding_box.right = this->transform.position.x + this->transform.scale.x + origin.x;
  this->bounding_box.left = this->transform.position.x + origin.x;
  this->bounding_box.bottom = this->transform.position.y + this->transform.scale.y + origin.y;
  this->bounding_box.top = this->transform.position.y + origin.y;
  
  // // DEBUG print the bounding box of the object along with its handle
  // printf("[%s] [collider] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x, this->transform.position.y);
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
  Camera::OrthoCamera *camera, 
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
    if (grid != glm::vec2(0.0f)) {
      object.grid = grid;
      object.snap = true;
      object.update_snap_position();
    } else {
      object.update_position();
    }

    GameObjects::Objects[handle] = object;
    return &GameObjects::Objects[handle];
}

GameObject *GameObjects::create(
  std::string handle, 
  Camera::OrthoCamera *camera, 
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
