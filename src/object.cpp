#include "object.h"

GameObject::Object::Object() {
  this->update_bounding_box();
  this->active = true;
  this->interactive = true;
}

void GameObject::Object::render(SpriteRenderer *renderer) {
  if (this->active)
    renderer->render(this->texture, this->position, this->scale * (this->absolute_scaling ? 1.0f : this->scale_factor));
}

void GameObject::Object::translate_to_point(glm::vec2 point) {
  this->position = point;
  if (this->snap != glm::vec2(0.0f)) this->update_snap_position();
  this->update_bounding_box();
}

void GameObject::Object::update_position() {
  this->translate_to_point(this->position);
}

bool GameObject::Object::check_point_intersection(glm::vec2 point) {
  return ((this->bounding_box.left <= point.x) && (this->bounding_box.right >= point.x) && (this->bounding_box.top <= point.y) && (this->bounding_box.bottom >= point.y));
}

GameObject::Object *GameObject::create(std::string handle, Camera::OrthoCamera *camera, Texture texture, glm::vec2 position, glm::vec2 scale, bool absolute_scaling, glm::vec2 snap) {
  GameObject::Object object = GameObject::Object();
  object.handle = handle;
  object.camera = camera;
  object.texture = texture;
  object.position = position;
  object.scale = scale;
  object.snap = snap;
  object.absolute_scaling = absolute_scaling;
  object.scale_factor = (float)camera->width / (float)camera->height;

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
  float scale_factor = this->absolute_scaling ? 1.0f : this->scale_factor;
  this->bounding_box.right = this->position.x + (((this->texture.width / 2) / (this->texture.width / this->scale.x))) * scale_factor;
  this->bounding_box.left = this->position.x - (((this->texture.width / 2) / (this->texture.width / this->scale.x))) * scale_factor;
  this->bounding_box.bottom = this->position.y + (((this->texture.height / 2) / (this->texture.height / this->scale.y))) * scale_factor;
  this->bounding_box.top = this->position.y - (((this->texture.height / 2) / (this->texture.height / this->scale.y))) * scale_factor;
}

void GameObject::Object::update_snap_position() {
  glm::vec2 snap;
  snap.x = this->absolute_scaling ? this->snap.x : this->snap.x * this->scale_factor;
  snap.y = this->absolute_scaling ? this->snap.y : this->snap.y * this->scale_factor;
  this->position = glm::vec2(round(this->position.x / snap.x) * snap.x, round(this->position.y / snap.y) * snap.y);
}
