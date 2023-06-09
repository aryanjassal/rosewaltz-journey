#include "player.h"

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

void Player::update() {
  if (this->bounding_box.left <= 0.0f || this->bounding_box.right >= this->camera->width) {
    this->walk_speed *= -1;
    this->transform.position.x = std::clamp(this->transform.position.x, 0.0f, (float)this->camera->width - this->transform.scale.x);
  }
  this->transform.position.z = 1.0f;
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
  this->transform.position += glm::vec3(this->velocity.x + this->walk_speed, this->grounded ? 0.0f : -this->velocity.y, 0.0f);
  this->impulse = glm::vec2(0.0f);

  // Update the bounding box of the player
  this->update_bounding_box();
}

void Player::resolve_collisions() {
  // If the player is not a rigidbody, then no collisions can happen
  if (!this->rigidbody) return;

  // Set the grounded to false, so if no collisions is detected, then the grounded will be false by default
  this->grounded = false;

  // Number of tiles the player is in contact with
  int t_touching = 0;

  // Loop over each object and resolve each collision
  if (this->parent_tile != nullptr) {
    for (GameObject *&object : GameObjects::all()) {
      Collision collision = object->check_collision(this);
      if (object->tags[0] == this->parent_tile->tags[0]) {
        Collision collision = object->check_collision(this);

        if (collision.collision) {
          if (object->rigidbody) {
            this->grounded = true;
            Direction v_dir = collision.vertical.direction;
            
            // Vertical collision handling
            if (v_dir == DOWN) {
              this->transform.position.y -= collision.vertical.mtv;
            } else if (v_dir == UP) {
              this->transform.position.y -= collision.vertical.mtv - this->transform.scale.y - this->transform.scale.y;
            } 
          }
        } 
      }

      // if the player is touching a tile, then increment the t_touching variable 
      if (collision.collision && object->tags[1] == "tile") t_touching++;
    }
  }

  // If the player is touching more than two tiles, then set a flag on the player
  if (t_touching >= 2) {
    // printf("[%s] touching more than one tiles\n", this->handle.c_str());
    this->movable = false;
  } else {
    this->movable = true;
    // printf("[%s] touching one or less tiles\n", this->handle.c_str());
  }
}

std::vector<Player *> Characters::Players::all() {
  std::vector<Player *> all_players;
  for (auto &pair : Characters::Players::Players)
    if (pair.second.active)
      all_players.push_back(&pair.second);
  return all_players;
}
