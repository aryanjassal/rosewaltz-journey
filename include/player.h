#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdio.h>

#include "camera.h"
#include "sprite.h"
#include "object.h"

// Create a Player class to handle any and all player-related code 
class Player : public GameObject {
  public:
    // The velocity acting on the player at every frame
    glm::vec2 velocity = glm::vec2(0.0f);

    // The acceleration will be added to the velocity each frame
    glm::vec2 acceleration = glm::vec2(0.0f, -0.1f);

    // The impulse force will apply for one frame only
    glm::vec2 impulse = glm::vec2(0.0f, 5.0f);

    // // The drag coefficient that will reduce the velocity of the player by a set amount
    // glm::vec2 drag_coefficient = glm::vec2(0.03f);

    // // Should the object have drag or not?
    // bool drag;

    // Empty construtor
    Player() { }

    // Resolve forces and vectors
    void resolve_vectors();

    // Resolve all collisions with other objects
    void resolve_collisions();
};

// When handling GameObjects gets too annoying and more control over
// the object or more interactive features are needed, then this class
// will come in clutch. Examples include but are not limited to a Player
// or a mob/NPC controller.
namespace Characters {
  namespace Players {
    // Keep track of all the players created
    static std::map<std::string, Player> Players;

    // Create a Player by providing all the required parameters
    Player *create(
      std::string handle, 
      OrthoCamera *camera, 
      Texture texture, 
      glm::vec2 window_dimensions, 
      glm::vec3 position = glm::vec3(0.0f),
      glm::vec2 scale = glm::vec2(100.0f), 
      float rotation = 0.0f,
      std::vector<std::string> tags = std::vector<std::string>(),
      glm::vec2 origin = glm::vec2(0.0f)
    );

    std::vector<Player *> all();
  }
};

#endif
