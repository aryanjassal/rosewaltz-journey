#include "player.h"
#include <limits>

Player *Characters::Players::create(
  std::string handle, 
  OrthoCamera *camera, 
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
  
  Player player = Player();
  player.handle = handle;
  player.camera = camera;
  player.texture = texture;
  player.tags = tags;
  player.transform = transform;
  player.origin = origin;
  player.rigidbody = true;
  player.active = true;
  player.bounding_box = { 0.0f, 0.0f, 0.0f, 0.0f };
  player.swap = false;
  player.interactive = false;
  player.update_bounding_box();

  Characters::Players::Players[handle] = player;
  return &Characters::Players::Players[handle];
}

void Player::resolve_vectors() {
  // Apply impulse acceleration and regular acceleration to the object
  this->velocity += this->acceleration;
  this->velocity += this->impulse;

  // Flip the y-component of the velocity as it points upwards, which is incorrect in this context
  this->transform.position += glm::vec3(this->velocity.x, -this->velocity.y, this->transform.position.z);
  this->impulse = glm::vec2(0.0f);

  // if (this->transform.position.x > this->window_dimensions.x || this->transform.position.y > this->window_dimensions.y) exit(0);

  // Update the bounding box of the player
  this->update_bounding_box();
}

void Player::resolve_collisions() {
  // If the player is not a rigidbody, then no collisions can happen
  if (!this->rigidbody) return;

  // Otherwise, loop over each object and resolve each collision
  for (GameObject *&object : GameObjects::all()) {
    if (object->rigidbody) {
      if (object->check_collision(this->bounding_box)) {
        this->velocity = glm::vec2(1.0f, -this->velocity.y);
        this->impulse = glm::vec2(0.0f, 1.0f);
      }
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
