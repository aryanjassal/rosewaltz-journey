#include "object.h"

void GameObject::Object::render(SpriteRenderer *renderer) {
  if (this->active) renderer->render(this->texture, this->position, this->scale);
}

void GameObject::Object::translate_to_point(glm::vec2 point, glm::vec2 origin) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  // Transform the point to the new origin provided
  glm::vec2 transformed_point = glm::vec2(0.0f);
  transformed_point.x = point.x - ((this->texture.width / (this->texture.width / this->scale.x)) * (origin.x ? origin.x : this->origin.x));
  transformed_point.y = point.y - ((this->texture.height / (this->texture.height / this->scale.y)) * (origin.y ? origin.y : this->origin.y));
  this->position = transformed_point;

  // Snap the object and update its bounding box
  if (this->snap) this->update_snap_position();
  this->update_bounding_box();
}

void GameObject::Object::update_position() {
  this->translate_to_point(this->position);
}

bool GameObject::Object::check_point_intersection(glm::vec2 point, glm::vec2 origin) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  glm::vec2 transformed_point;
  transformed_point.x = point.x + (origin.x ? origin.x : this->origin.x);
  transformed_point.y = point.y + (origin.y ? origin.y : this->origin.y);

  return ((this->bounding_box.left <= transformed_point.x) 
    && (this->bounding_box.right >= transformed_point.x) 
    && (this->bounding_box.top <= transformed_point.y) 
    && (this->bounding_box.bottom >= transformed_point.y));
}

GameObject::Object *GameObject::create(std::string handle, Camera::OrthoCamera *camera, Texture texture, glm::vec2 window_dimensions, glm::vec2 position, glm::vec2 scale, glm::vec2 snap) {
  GameObject::Object object = GameObject::Object();
  object.handle = handle;
  object.camera = camera;
  object.texture = texture;
  object.window_dimensions = window_dimensions;
  object.position = position;
  object.scale = scale;
  if (snap != glm::vec2(0.0f)) {
    object.snap_grid = snap;
    object.snap = true;
  }
  object.update_bounding_box();
  object.update_position();

  Objects[handle] = object;
  return &Objects[handle];
}

GameObject::Object *GameObject::get(std::string handle) {
  return &Objects[handle];
}

std::vector<GameObject::Object *> GameObject::all() {
  std::vector<GameObject::Object *> all_objects;
  for (auto &pair : GameObject::Objects) {
    if (pair.second.active) {
      all_objects.push_back(&pair.second);
    }
  }
  return all_objects;
}

void GameObject::update(std::string handle, GameObject::Object object) {
  Objects[handle] = object;
}

void GameObject::Object::update_bounding_box() {
  glm::vec2 scale = (this->scale / glm::vec2(this->camera->width, this->camera->height)) * this->window_dimensions;

  this->bounding_box.right = this->position.x + (this->texture.width / (this->texture.width / scale.x));
  this->bounding_box.left = this->position.x;
  this->bounding_box.bottom = this->position.y + (this->texture.height / (this->texture.height / scale.y));
  this->bounding_box.top = this->position.y;
  
  // // DEBUG print the bounding box of the object along with its handle
  // printf("[%s] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->position.x, this->position.y);
}

void GameObject::Object::update_snap_position() {
  // If snapping is disabled in the object settings, then do nothing
  if (!this->snap) return;

  // Otherwise, update the position to snap to the nearest snap point
  //TODO: lerp this position transition. Maybe return vec2 position and while this->pos != desired pos, use the current pos and keep lerping to the final position
  glm::vec2 new_position;
  new_position.x = round(this->position.x / this->snap_grid.x) * this->snap_grid.x;
  new_position.y = round(this->position.y / this->snap_grid.y) * this->snap_grid.y;
  this->position = new_position;

  this->update_bounding_box();

  // //DEBUG print the bounding box of the object along with its handle
  // printf("[%s] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->position.x, this->position.y);
}

GameObject::ObjectGroup *GameObject::get_group(std::string handle) {
  return &ObjectGroups[handle];
}

GameObject::ObjectGroup *GameObject::create_group(std::string handle) {
  GameObject::ObjectGroup new_group = GameObject::ObjectGroup();
  new_group.handle = handle;
  ObjectGroups[handle] = new_group;
  return &ObjectGroups[handle];
}

void GameObject::ObjectGroup::add(std::string handle, GameObject::Object *object) {
  this->Objects[handle] = *object;
  printf("Added new object [%s] to group [%s]\n", handle.c_str(), this->handle.c_str());
}

void GameObject::ObjectGroup::update(std::string handle, GameObject::Object object) {
  this->Objects[handle] = object;
}

void GameObject::ObjectGroup::render(SpriteRenderer *renderer) {
  for (auto &object : this->Objects) {
    object.second.render(renderer);
  }
}

void GameObject::ObjectGroup::render(SpriteRenderer *renderer, std::string handle) {
  this->Objects[handle].render(renderer);
}

std::vector<GameObject::Object *> GameObject::ObjectGroup::all() {
  std::vector<GameObject::Object *> all_objects;
  for (auto &pair : this->Objects) {
    if (pair.second.active) {
      all_objects.push_back(&pair.second);
    }
  }
  return all_objects;
}

GameObject::Object *GameObject::ObjectGroup::get(std::string handle) {
  return &this->Objects[handle];
}

void GameObject::update(std::string handle, GameObject::ObjectGroup *object_group) {
  ObjectGroups[handle] = *object_group;
}

void GameObject::ObjectGroup::remove(std::string handle) {
  this->Objects.erase(handle);
}

void GameObject::ObjectGroup::update_bounding_box() {
  glm::vec2 scale = (this->scale / glm::vec2(this->camera->width, this->camera->height)) * this->window_dimensions;

  BoundingBox best_bounding_box;

  for (auto object : this->Objects) {
    if (object.second.bounding_box.left < best_bounding_box.left) best_bounding_box.left = object.second.bounding_box.left;
    if (object.second.bounding_box.right > best_bounding_box.right) best_bounding_box.right = object.second.bounding_box.right;
    if (object.second.bounding_box.top < best_bounding_box.top) best_bounding_box.top = object.second.bounding_box.top;
    if (object.second.bounding_box.bottom > best_bounding_box.bottom) best_bounding_box.bottom = object.second.bounding_box.bottom;
  }

  this->bounding_box = best_bounding_box;

  printf("[%s] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->position.x, this->position.y);

  // this->bounding_box.right = this->position.x + (this->texture.width / (this->texture.width / scale.x));
  // this->bounding_box.left = this->position.x;
  // this->bounding_box.bottom = this->position.y + (this->texture.height / (this->texture.height / scale.y));
  // this->bounding_box.top = this->position.y;
}
