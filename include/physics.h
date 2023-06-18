#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "glm/glm.hpp"

// Directional enum to handle collision direction
typedef enum Direction {
  UP,
  RIGHT,
  DOWN,
  LEFT,
  NONE
};

// A struct to define the structure of information regarding the GameObject's bounding box
typedef struct BoundingBox {
  // Default constructor to initialise with default empty values
  BoundingBox() : top(0.0f), bottom(0.0f), left(0.0f), right(0.0f) { }
  BoundingBox(float _top, float _bottom, float _left, float _right) : top{_top}, bottom{_bottom}, left{_left}, right{_right} { }

  // Fields of the struct
  float top;
  float bottom;
  float left;
  float right;
};

// Return the best direction the target vector is facing
Direction vector_direction(glm::vec2 target);

// Define a tuple storing all the relevant information about a collision
typedef struct CollisionInfo {
  // Default constructor to set default values
  CollisionInfo(): collision(false), direction(NONE), mtv(0.0f) { }
  CollisionInfo(bool _collision): collision{_collision}, direction(NONE), mtv(0.0f) { }
  CollisionInfo(bool _collision, Direction _direction): collision{_collision}, direction{_direction}, mtv(0.0f) { }
  CollisionInfo(bool _collision, Direction _direction, float _mtv): collision{_collision}, direction{_direction}, mtv{_mtv} { }

  // Override the boolean conversion
  operator bool() { return collision; }

  // Fields of the struct
  Direction direction;
  float mtv;

  private: 
    bool collision;
};

// A struct which combines the vertical and horizontal collision information
typedef struct Collision {
  // Default constructor to set default values
  Collision() : collision(false), horizontal(CollisionInfo()), vertical(CollisionInfo()) { }
  Collision(bool _collision) : collision{_collision}, horizontal(CollisionInfo()), vertical(CollisionInfo()) { }
  Collision(bool _collision, CollisionInfo _horizontal, CollisionInfo _vertical) : collision{_collision}, horizontal{_horizontal}, vertical{_vertical} { }

  // Override the boolean conversion
  operator bool() { return collision; }

  // Fields of the struct
  CollisionInfo horizontal;
  CollisionInfo vertical;

  private: 
    bool collision;
};

#endif
