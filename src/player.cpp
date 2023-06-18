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

void Player::animate() {
  this->animation_timer -= Time::delta * 1000;

  if (this->animation_timer <= 0.0f) {
    this->current_frame = (this->current_frame + 1) % this->animation_sprite_sheet.size();
    this->texture = this->animation_sprite_sheet.at(current_frame);
    this->animation_timer = this->fps;
  }
}

void Player::update() {
  if (this->rigidbody) {
    if (this->bounding_box.left <= 0.0f || this->bounding_box.right >= GameObjects::Camera->width) {
      this->walk_speed *= -1;
      
      if (this->walk_speed < 0) this->flip_x = true;
      else this->flip_x = false;

      // if (this->walk_speed > 0) this->position_offset.x -= 100.0f;
      // else this->position_offset.x += 100.0f;

      // this->transform.scale.x *= -1;
      // printf("transfor: %.2f, %.2f, %.2f\n", this->transform.position.x, this->transform.position.y, this->transform.position.z);

      this->transform.position.x = std::clamp(this->transform.position.x - this->position_offset.x, 0.0f, (float)GameObjects::Camera->width - this->transform.scale.x);
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

  // Set variables to false, so if they are not updated, they will be false by default
  this->grounded = false;
  this->won = false;

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
          } else if (c.vertical.collision && c.vertical.direction == UP && !c.horizontal.collision) {
            printf("[player] UP collision detected  \n");
          } 

          if (object->tags[0] == "obstacle") {
            if (c.vertical.collision && c.vertical.direction == DOWN) this->transform.position.y -= c.vertical.mtv;
            else {
              if (c.horizontal.collision && c.horizontal.direction == LEFT) this->transform.position.x -= c.horizontal.mtv;
              else if (c.horizontal.collision && c.horizontal.direction == RIGHT) this->transform.position.x -= c.horizontal.mtv - object->transform.scale.x - this->transform.scale.x;
              this->walk_speed *= -1.0;

              if (this->walk_speed < 0) this->flip_x = true;
              else this->flip_x = false;
            }
          }
        }
      } 

      if (c.collision && !object->rigidbody) {
        if (object->handle == "goal") {
          object->texture = ResourceManager::Texture::get("treasure-open");
          this->won = true;
          this->locked = true;
        } else if (object->tags[0] == "tile") {
          t_touching++;
        }
      }
    }

    if (t_touching >= 2) {
      this->locked = true;
    } else {
      this->locked = false;
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
