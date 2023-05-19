#include "object.h"

void GameObject::render(SpriteRenderer *renderer) {
  if (this->active) renderer->render(this->texture, this->position, this->scale);
}

void GameObject::translate_to_point(glm::vec2 point) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  // Transform the point to the new origin provided
  glm::vec2 transformed_point = glm::vec2(0.0f);
  transformed_point.x = point.x - ((this->texture.width / (this->texture.width / this->scale.x)) * this->origin.x);
  transformed_point.y = point.y - ((this->texture.height / (this->texture.height / this->scale.y)) * this->origin.y);
  this->position = transformed_point;

  // Snap the object and update its bounding box
  if (this->snap) this->update_snap_position();
  this->update_bounding_box();
}

void GameObject::update_position() {
  this->translate_to_point(this->position);
}

bool GameObject::check_point_intersection(glm::vec2 point) {
  point = (point / this->window_dimensions) * glm::vec2(this->camera->width, this->camera->height);

  glm::vec2 transformed_point;
  transformed_point.x = point.x + (origin.x ? origin.x : this->origin.x);
  transformed_point.y = point.y + (origin.y ? origin.y : this->origin.y);

  return ((this->bounding_box.left <= transformed_point.x) 
    && (this->bounding_box.right >= transformed_point.x) 
    && (this->bounding_box.top <= transformed_point.y) 
    && (this->bounding_box.bottom >= transformed_point.y));
}



// void GameObject::update(std::string handle, GameObject::Object object) {
//   Objects[handle] = object;
// }

void GameObject::update_bounding_box() {
  glm::vec2 scale = (this->scale / glm::vec2(this->camera->width, this->camera->height)) * this->window_dimensions;

  this->bounding_box.right = this->position.x + (this->texture.width / (this->texture.width / scale.x));
  this->bounding_box.left = this->position.x;
  this->bounding_box.bottom = this->position.y + (this->texture.height / (this->texture.height / scale.y));
  this->bounding_box.top = this->position.y;
  
  // // DEBUG print the bounding box of the object along with its handle
  // printf("[%s] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->position.x, this->position.y);
}

void GameObject::update_snap_position() {
  // If snapping is disabled in the object settings, then do nothing
  if (!this->snap) return;

  // Otherwise, update the position to snap to the nearest snap point
  //TODO: lerp this position transition. Maybe return vec2 position and while this->pos != desired pos, use the current pos and keep lerping to the final position
  glm::vec2 new_position;
  new_position.x = round(this->position.x / this->grid.x) * this->grid.x;
  new_position.y = round(this->position.y / this->grid.y) * this->grid.y;
  this->position = new_position;

  this->update_bounding_box();

  // //DEBUG print the bounding box of the object along with its handle
  // printf("[%s] %.2f < x < %.2f; %.2f < y < %.2f [pos: %.2f, %.2f]\n", this->handle.c_str(), this->bounding_box.left, this->bounding_box.right, this->bounding_box.top, this->bounding_box.bottom, this->position.x, this->position.y);
}

GameObject *GameObjects::create(
  std::string handle, 
  Camera::OrthoCamera *camera, 
  Texture texture, 
  glm::vec2 window_dimensions, 
  glm::vec2 position, 
  glm::vec2 scale, 
  glm::vec2 origin,
  glm::vec2 grid) {
    GameObject object = GameObject();
    object.handle = handle;
    object.camera = camera;
    object.texture = texture;
    object.window_dimensions = window_dimensions;
    object.position = position;
    object.scale = scale;
    object.origin = origin;
    if (grid != glm::vec2(0.0f)) {
      object.grid = grid;
      object.snap = true;
    }
    object.update_bounding_box();
    object.update_position();

  GameObjects::Objects[handle] = object;
    return &GameObjects::Objects[handle];
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
