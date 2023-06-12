#include "player.h"

Player *Characters::Players::create(const char *handle, Texture texture, Transform transform, std::vector<std::string> tags) {
  Player player = Player();
  player.handle = handle;
  player.texture = texture;
  player.tags = tags;
  player.transform = transform;
  player.rigidbody = true;
  player.update_bounding_box();

  Characters::Players::Players[handle] = player;
  return &Characters::Players::Players[handle];
}

void Player::update() {
  if (this->rigidbody) {
    if (this->bounding_box.left <= 0.0f || this->bounding_box.right >= GameObjects::Camera->width) {
      this->walk_speed *= -1;
      this->transform.position.x = std::clamp(this->transform.position.x, 0.0f, (float)GameObjects::Camera->width - this->transform.scale.x);
    }
  }
  this->transform.position.z = 1.0f;
}

void Player::resolve_vectors() {
  // If the object is grounded, nullify the y-velocity.
  // This is done to allow inversing gravity even when the player is grounded.
  if (this->grounded) this->velocity.y = 0.0f;

  // Apply impulse acceleration and regular acceleration to the object
  this->velocity.x += this->acceleration.x;
  this->velocity.x += this->impulse.x;

  this->velocity.y += this->acceleration.y;
  this->velocity.y += this->impulse.y;

  // Flip the y-component of the velocity as it points upwards, which is incorrect in this context
  this->transform.position += glm::vec3((this->velocity.x + this->walk_speed) * Time::delta, -this->velocity.y * Time::delta, 0.0f);
  this->impulse = glm::vec2(0.0f);

  // Update the bounding box of the player
  this->update_bounding_box();
}

void Player::resolve_collisions() {
  if (!this->rigidbody) return;

  // Set the grounded to false, so if no collisions is detected, then the grounded will be false by default
  this->grounded = false;

  // Only do collisions if a parent tile is set. If no parent tile exist, then the player is not colliding
  // with any tiles, and running collisions is redundant
  if (this->parent != nullptr) {
    int t_touching = 0;
    for (GameObject *&object : GameObjects::all()) {
      // The collision is being checked here as it is needed for the lock-unlock calculation
      Collision c = object->check_collision(this);

      // If the object is a rigidbody, then execute the collision checking
      if (object->rigidbody) {
        if (c.collision) {
          if (c.vertical.collision && c.vertical.direction == DOWN) {
            this->grounded = true;
            this->transform.position.y -= c.vertical.mtv;
          } else if (c.vertical.collision && c.vertical.direction == UP) {
            // printf("[player] UP collision detected  \n");
          }
        }
      } 

      if (c.collision && !object->rigidbody && object->handle == "tile") {
        t_touching++;
      }
    }

    if (t_touching >= 2) {
      this->locked = true;
      // printf("[player] touching multiple tiles      \r");
    } else {
      // printf("[player] parent: %s [%i]              \r", this->parent->handle.c_str(), this->parent->id);
      this->locked = false;
    }
    // fflush(stdout);
  }
}

std::vector<Player *> Characters::Players::all() {
  std::vector<Player *> all_players;
  for (auto &pair : Characters::Players::Players)
    if (pair.second.active)
      all_players.push_back(&pair.second);
  return all_players;
}
