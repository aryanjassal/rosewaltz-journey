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

void Player::resolve_vectors() {
  // Apply impulse acceleration and regular acceleration to the object
  this->velocity.x += this->acceleration.x;
  this->velocity.x += this->impulse.x;

  if (!this->grounded) {
    this->velocity.y += this->acceleration.y;
    this->velocity.y += this->impulse.y;
  } else this->velocity.y = 0.0f;

  // Flip the y-component of the velocity as it points upwards, which is incorrect in this context
  this->transform.position += glm::vec3(this->velocity.x + this->walk_speed, this->grounded ? 0.0f : -this->velocity.y, this->transform.position.z);
  this->impulse = glm::vec2(0.0f);

  // Update the bounding box of the player
  this->update_bounding_box();
}

void Player::resolve_collisions() {
  // If the player is not a rigidbody, then no collisions can happen
  if (!this->rigidbody) return;

  this->grounded = false;

  for (GameObject *&object : GameObjects::filter("border")) {
    Collision collision = object->check_collision(this);
    if (collision.collision && object->tags[0] == "border") {
      this->walk_speed *= -1;
      this->transform.position.x = std::clamp(this->transform.position.x, 0.0f, (float)this->camera->width - this->transform.scale.x);
      // this->transform.position.x += collision.horizontal.mtv / 4.0f;
      // printf("newpos: %.2f, %.2f\n", this->transform.position.x, this->transform.position.y);
    }
  }

  // Number of tiles the player is in contact with
  int t_touching = 0;

  // Otherwise, loop over each object and resolve each collision
  if (this->parent_tile != nullptr) 
  {
    for (GameObject *&object : GameObjects::filter(this->parent_tile->tags[0])) {
      Collision collision = object->check_collision(this);

      if (collision.collision) {
        if (object->rigidbody) {
          this->grounded = true;
          Direction v_dir = collision.vertical.direction;
          Direction h_dir = collision.horizontal.direction;
          // printf("v_%s h_%s ", v_dir == UP ? "UP" : v_dir == DOWN ? "DOWN" : "NONE", h_dir == LEFT ? "LEFT" : h_dir == RIGHT ? "RIGHT" : "NONE");
          // printf("y-vel: %.2f (y-off: %.2f) pos: [%.2f, %.2f]\n", this->velocity.y, collision.vertical.mtv, this->transform.position.x, this->transform.position.y);

          // Vertical collision handling
          if (v_dir == DOWN) {
            this->transform.position.y -= collision.vertical.mtv;
          }

          // if (abs(collision.vertical.mtv) > 300.0f) {
          //   printf("[%s] mtv too large\n", object->handle.c_str());
          //   continue;
          // }

          else if (v_dir == UP) {
            this->transform.position.y -= collision.vertical.mtv;
          } 

          // // Horizontal collision handling
          // if (h_dir == LEFT || h_dir == RIGHT) {
          //   this->velocity.x *= -1.0f;
          // }
        } else {
          if (object->tags[1] == "tile") t_touching++;
        } 
      } 
    }
  }

  if (t_touching >= 2) {
    // printf("[%s] touching more than one tiles\n", this->handle.c_str());
  }
}

std::vector<Player *> Characters::Players::all() {
  std::vector<Player *> all_players;
  for (auto &pair : Characters::Players::Players)
    if (pair.second.active)
      all_players.push_back(&pair.second);
  return all_players;
}
