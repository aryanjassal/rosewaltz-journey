#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdio.h>
#include <algorithm>

#include "camera.h"
#include "renderer.h"
#include "object.h"
#include "resource_manager.h"
#include "texture.h"

// Create a Player class to handle any and all player-related code 
class Player : public Entity {
  public:
    // The velocity acting on the player at every frame
    glm::vec2 velocity = glm::vec2(0.0f);

    // The acceleration will be added to the velocity each frame
    glm::vec2 acceleration = glm::vec2(0.0f, -10.0f);

    // The impulse force will apply for one frame only
    glm::vec2 impulse = glm::vec2(0.0f, 5.0f);

    // Movable controlls whether the player can move or not
    bool movable = true;

    // The walk speed the player walks with
    float walk_speed = 100.0f;

    // Is the object grounded?
    bool grounded = false;

    // Did the player win?
    bool won = false;

    // Did the player die?
    bool dead = false;

    // Constructor
    // TODO: Remove all instances of singular tag and singular texture, instead migrate to texture maps and tag vectors
    Player() { }
    Player(std::string handle, TextureMap texture_map, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform()):
      Entity(handle, texture_map, tags, transform) { }

    // Update the player every frame
    void update();

    // Resolve forces and vectors
    void resolve_vectors();

    // Resolve all collisions with other objects
    void resolve_collisions();
};

// When handling GameObjects gets too annoying and more control over
// the object or more interactive features are needed, then this class
// will come in clutch.
namespace Characters {
  namespace Players {
    // Create a Player by providing all the required parameters
    Player *create(std::string handle, std::vector<Texture> texture, Transform transform = Transform(), std::vector<std::string> tags = std::vector<std::string>());
    Player *create(std::string handle, Texture texture, Transform transform = Transform(), std::vector<std::string> tags = std::vector<std::string>());

    std::vector<Player*> all();
  }
};

#endif
