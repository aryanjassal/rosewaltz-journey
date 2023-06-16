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
#include "sprite.h"
#include "object.h"
#include "resource_manager.h"
#include "texture.h"

// Create a Player class to handle any and all player-related code 
class Player : public GameObject {
  public:
    // The velocity acting on the player at every frame
    glm::vec2 velocity = glm::vec2(0.0f);

    // The acceleration will be added to the velocity each frame
    glm::vec2 acceleration = glm::vec2(0.0f, -10.0f);

    // The impulse force will apply for one frame only
    glm::vec2 impulse = glm::vec2(0.0f, 5.0f);

    // The walk speed the player walks with
    float walk_speed = 100.0f;

    // Is the object grounded?
    bool grounded = false;

    // Did the player win?
    bool won = false;

    // Animation related variables 
    float fps = 100.0f;
    float animation_timer = this->fps;
    int current_frame = 0;

    // The sprite sheet for the player's animation
    std::vector<Texture> animation_sprite_sheet = std::vector<Texture>();

    // Empty construtor
    Player() { }

    // Update the player every frame
    void update();

    // Resolve forces and vectors
    void resolve_vectors();

    // Resolve all collisions with other objects
    void resolve_collisions();

    // Update the animation state
    void animate();
};

// When handling GameObjects gets too annoying and more control over
// the object or more interactive features are needed, then this class
// will come in clutch. Examples include but are not limited to a Player
// or a mob/NPC controller.
namespace Characters {
  namespace Players {
    // Keep track of all the players created
    static std::map<std::string, Player> Players;

    // Assign a static, active player
    static Player *ActivePlayer;

    // Create a Player by providing all the required parameters
    Player *create(const char *handle, Texture texture, Transform transform = Transform(), std::vector<std::string> tags = std::vector<std::string>());

    std::vector<Player *> all();
  }
};

#endif
