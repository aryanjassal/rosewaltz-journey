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
  this->velocity.x += this->acceleration.x;
  this->velocity.x += this->impulse.x;

  if (!this->grounded) {
    this->velocity.y += this->acceleration.y;
    this->velocity.y += this->impulse.y;
  } else this->velocity.y = 0.0f;

  // Flip the y-component of the velocity as it points upwards, which is incorrect in this context
  this->transform.position += glm::vec3(this->velocity.x, this->grounded ? 0.0f : -this->velocity.y, this->transform.position.z);
  this->impulse = glm::vec2(0.0f);

  // Update the bounding box of the player
  this->update_bounding_box();
}

void Player::resolve_collisions() {
  // If the player is not a rigidbody, then no collisions can happen
  if (!this->rigidbody) return;

  this->grounded = false;

  // Otherwise, loop over each object and resolve each collision
  for (GameObject *&object : GameObjects::all()) {
    if (object->rigidbody) {
      Collision collision = object->check_collision(this);
      if (std::get<0>(collision)) {
        this->grounded = true;
        Direction dir = std::get<1>(collision);
        // printf("%s ", dir == UP ? "UP" : dir == DOWN ? "DOWN" : dir == LEFT ? "LEFT" : dir == RIGHT ? "RIGHT" : "NONE");
        // printf("y-vel: %.2f (y-off: %.2f) pos: [%.2f, %.2f]\n", this->velocity.y, std::get<2>(collision).y, this->transform.position.x, this->transform.position.y);

        if (dir == DOWN) this->velocity.y = 0.0f;

        if (dir == DOWN) this->transform.position.y -= std::get<2>(collision).y;
        else if (dir == UP) this->transform.position.y += std::get<2>(collision).y;
        else if (dir == LEFT) this->transform.position.x -= std::get<2>(collision).x;
        else if (dir == RIGHT) this->transform.position.x += std::get<2>(collision).x;
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
