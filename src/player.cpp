#include "player.h"
#include "physics.h"

std::map<std::string, Player> AllPlayers;

Player *Characters::Players::create(std::string handle, std::vector<Texture> texture, Transform transform, std::vector<std::string> tags) {
  Player player = Player(handle, TextureMap(texture));
  player.rigidbody = true;

  AllPlayers[handle] = player;
  return &AllPlayers[handle];
}

Player *Characters::Players::create(std::string handle, Texture texture, Transform transform, std::vector<std::string> tags) {
  return Characters::Players::create(handle, (std::vector<Texture>){ texture }, transform, tags);
}

void Player::update() {
  if (this->rigidbody) {
    if (this->colliders.at(0).x <= 0.0f || this->colliders.at(0).x + this->colliders.at(0).w >= Entities::Camera->dimensions.x) {
      this->walk_speed *= -1;
      
      // if (this->walk_speed < 0) this->flip_x = true;
      // else this->flip_x = false;

      this->transform.position.x = std::clamp(this->transform.position.x, 0.0f, Entities::Camera->dimensions.x - this->transform.scale.x);
    }
  }
  this->transform.z = 1.0f;
  this->texture_map.tick(Time::delta * 1000.0f);
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
  this->transform.position += glm::vec2((this->velocity.x + this->walk_speed) * Time::delta, -this->velocity.y * Time::delta);
  this->impulse = glm::vec2(0.0f);

  // Update the bounding box of the player
  this->update_colliders(0, this->transform.position.x, this->transform.position.y + (this->transform.scale.y / 2.0f), this->transform.scale.x, this->transform.scale.y / 2.0f);
  // this->update_colliders(1, this->transform.position.x - 1.5 * this->transform.scale.x + this->transform.scale.x, this->transform.position.y - (this->transform.scale.y / 2.0f), this->transform.scale.x * 2.0f, this->transform.scale.y / 2.0f);
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
    for (Entity *&object : Entities::all()) {
      
      // The collision is being checked here as it is needed for the lock-unlock calculation
      Collision collision = this->check_collision(object);

      // If the object is a rigidbody, then execute the collision checking
      if (object->rigidbody && collision) {
        if (object->tags[0] == "obstacle") {
          // this->flip_x = !this->flip_x;
          this->walk_speed *= -1;
        }

        printf("COLLSISION (mtv: %.2f, %.2f)\n", collision.mtv.x, collision.mtv.y);
        this->grounded = true;
        if (collision.mtv.x > collision.mtv.y) this->transform.position.y += collision.mtv.y;
        else this->transform.position.x += collision.mtv.x;
        // this->transform.position -= glm::vec3(collision.mtv, 0.0f);
        this->update_colliders();
      } 

      if (collision && !object->rigidbody) {
        if (object->tags[0] == "tile") {
          t_touching++;
        }
      }

      if (collision && object->tags[0] == "goal") {
        object->texture_map.index = 1;
        this->won = true;
        this->movable = false;
      }
    }

    if (t_touching >= 2) {
      this->movable = true;
    } else {
      this->movable = false;
    }
  }
}

std::vector<Player *> Characters::Players::all() {
  std::vector<Player *> all_players;
  for (auto &pair : AllPlayers)
    if (pair.second.active)
      all_players.push_back(&pair.second);
  return all_players;
}
