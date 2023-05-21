#include "object.h"

void GameObject::render(SpriteRenderer *renderer) {
  // Transform new_transform = this->transform;
  // new_transform.position += this->origin;
  if (this->active) renderer->render(this->texture, this->transform);
}

void GameObject::translate_to_point(glm::vec2 point) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);
  printf("[%s] [translate] point: %.2f, %.2f; origin: %.2f, %.2f\n", this->handle.c_str(), point.x, point.y, origin.x, origin.y);

  // Transform the point to the new origin provided
  // glm::vec2 transformed_point = glm::vec2(0.0f);
  // transformed_point.x = point.x - (this->texture.width / (this->texture.width / this->transform.scale.x));
  // transformed_point.y = point.y - (this->texture.height / (this->texture.height / this->transform.scale.y));
  glm::vec2 transformed_point = point;
  this->transform.position = transformed_point - origin;

  // Snap the object and update its bounding box
  if (this->snap) this->update_snap_position();
  this->update_bounding_box();
}

void GameObject::update_position() {
  this->translate_to_point(this->transform.position);
}

bool GameObject::check_point_intersection(glm::vec2 point) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  // [REDUNDANT]?
  glm::vec2 transformed_point;
  transformed_point = point;

  return ((this->bounding_box.left <= transformed_point.x) 
    && (this->bounding_box.right >= transformed_point.x) 
    && (this->bounding_box.top <= transformed_point.y) 
    && (this->bounding_box.bottom >= transformed_point.y));
}

void GameObject::update_bounding_box() {
  glm::vec2 scale = (this->transform.scale / glm::vec2(this->camera->width, this->camera->height)) * this->window_dimensions;
  glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);

  this->bounding_box.right = this->transform.position.x + (this->texture.width / (this->texture.width / scale.x)) + origin.x;
  this->bounding_box.left = this->transform.position.x + origin.x;
  this->bounding_box.bottom = this->transform.position.y + (this->texture.height / (this->texture.height / scale.y)) + origin.y;
  this->bounding_box.top = this->transform.position.y + origin.y;
  
  // // DEBUG print the bounding box of the object along with its handle
  // printf("[%s] [collider] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x, this->transform.position.y);
}

void GameObject::update_snap_position() {
  // If snapping is disabled in the object settings, then do nothing
  if (!this->snap) return;

  // Otherwise, update the position to snap to the nearest snap point
  // glm::vec2 origin = this->originate ? this->origin : glm::vec2(0.0f);
  glm::vec2 origin = this->origin;
  glm::vec2 new_position;
  new_position.x = std::floor((this->transform.position.x + origin.x) / this->grid.x) * this->grid.x;
  new_position.y = std::floor((this->transform.position.y + origin.y) / this->grid.y) * this->grid.y;
  // this->transform.position = new_position + (this->originate ? this->origin : glm::vec2(0.0f));
  this->transform.position = new_position;

  this->update_bounding_box();

  //DEBUG print the bounding box of the object along with its handle
  printf("[%s] [snap] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->transform.position.x, this->transform.position.y);
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
    object.originate = true;
    if (grid != glm::vec2(0.0f)) {
      object.grid = grid;
      object.snap = true;
    }
    object.update_snap_position();
    // object.update_position();

    GameObjects::Objects[handle] = object;
    return &GameObjects::Objects[handle];
}

GameObject *GameObjects::create(
  std::string handle, 
  Camera::OrthoCamera *camera, 
  Texture texture, 
  glm::vec2 window_dimensions, 
  std::vector<std::string> tags,
  glm::vec2 position,
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
  for (auto &pair : GameObjects::Objects) {
    if (pair.second.active) {
      for (std::string o_tag : pair.second.tags)
        if (tag == o_tag)
          filtered_objects.push_back(&pair.second);
    }
  }
  return filtered_objects;
}
