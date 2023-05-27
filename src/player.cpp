#include "player.h"
#include <limits>

// Player *Characters::Players::create(
//   std::string handle, 
//   Camera::OrthoCamera *camera, 
//   Texture texture, 
//   glm::vec2 window_dimensions, 
//   Transform transform,
//   std::vector<std::string> tags,
//   glm::vec2 origin
// ) {
//   Player player = Player();
//   player.handle = handle;
//   player.camera = camera;
//   player.texture = texture;
//   player.window_dimensions = window_dimensions;
//   player.tags = tags;
//   player.transform = transform;
//   player.origin = origin;
//   player.originate = false;
//   player.interactive = false;
//   player.update_position();
//
//   Characters::Players::Players[handle] = player;
//   return &Characters::Players::Players[handle];
// }

Player *Characters::Players::create(
  std::string handle, 
  Camera::OrthoCamera *camera, 
  Texture texture, 
  glm::vec2 window_dimensions, 
  glm::vec3 position,
  glm::vec2 scale, 
  float rotation,
  std::vector<std::string> tags,
  glm::vec2 origin
) {
  Transform transform;
  transform.position = position;
  transform.scale = scale;
  transform.rotation = rotation;
  // Characters::Players::create(handle, camera, texture, window_dimensions, transform, tags, origin);
  Player player = Player();
  player.handle = handle;
  player.camera = camera;
  player.texture = texture;
  player.tags = tags;
  player.transform = transform;
  player.origin = origin;
  player.rigidbody = true;
  player.active = true;
  player.update_bounding_box();

  Characters::Players::Players[handle] = player;
  return &Characters::Players::Players[handle];
}

void Player::resolve_vectors() {
  this->velocity += this->acceleration;
  this->velocity += this->impulse;

  this->transform.position += glm::vec3(this->velocity.x, -this->velocity.y, this->transform.position.z);
  this->impulse = glm::vec2(0.0f);
}

void Player::resolve_collisions() {
  // If this object is not a rigidbody, then do not resolve collisions
  if (!this->rigidbody) return;

  // Otherwise, loop over each object and resolve each collision
  for (GameObject *&object : GameObjects::all()) {
    if (object->rigidbody) {
      if ((this->bounding_box.left <= object->bounding_box.right) 
        || (this->bounding_box.right >= object->bounding_box.left)
        && (this->bounding_box.top <= object->bounding_box.bottom) 
        || (this->bounding_box.bottom >= object->bounding_box.top)) {
      // if(object->check_point_intersection(glm::vec2(this->transform.position.x, this->transform.position.y)), false) {
        printf("[%s] collision with [%s] detected\n", this->handle.c_str(), object->handle.c_str());
      } 
      // else printf("[%s] no collision\n", this->handle.c_str());
    } 
  }
}

std::vector<Player *> Characters::Players::all() {
  std::vector<Player *> all_players;
  for (auto &pair : Characters::Players::Players)
    if (pair.second.active)
      all_players.push_back(&pair.second);
  return all_players;
}
