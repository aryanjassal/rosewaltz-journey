#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <limits>
#include <vector>
#include <math.h>
#include <cstdio>

#include "glm/glm.hpp"

#include "tutils.h"

// A struct to define the structure of information regarding an object's oriented collider.
typedef struct Collider {
  // Default constructor to initialise with default empty values
  Collider() : x(0.0f), y(0.0f), w(0.0f), h(0.0f), angle(0.0f) { }
  Collider(float _x, float _y, float _w, float _h) : x{_x}, y{_y}, w{_w}, h{_h}, angle(0.0f) { }
  Collider(float _x, float _y, float _w, float _h, float _angle) : x{_x}, y{_y}, w{_w}, h{_h}, angle{_angle} { }

  // Dimensions of the collider
  float x, y, w, h;

  // The angle of the collider (in degrees)
  float angle;

  // Returns all the vertices of the collider as a 2D vector
  std::vector<glm::vec2> vertices() {
    std::vector<glm::vec2> out;
    out.push_back(glm::vec2(x + w, y + h));
    out.push_back(glm::vec2(x + w, y));
    out.push_back(glm::vec2(x, y));
    out.push_back(glm::vec2(x, y + h));
    return out;
  }
};

// A struct which combines the vertical and horizontal collision information
typedef struct Collision {
  Collision() : collision(false), mtv(glm::vec2()) { }
  Collision(bool _collision) : collision{_collision}, mtv(glm::vec2()) { }
  Collision(bool _collision, glm::vec2 _mtv) : collision{_collision}, mtv{_mtv} { }

  // Collision information
  bool collision;
  glm::vec2 mtv;

  // Simplify checking for collision
  operator bool() { return collision; }
};

namespace Physics {
  // Note: The collider-point collision doesn't return a minimum translation vector
  Collision collider_point_collision(Collider collider, glm::vec2 point);
  Collision collider_collider_collision(Collider a, Collider b);
}

#endif
