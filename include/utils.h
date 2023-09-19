#ifndef __UTILS_H__
#define __UTILS_H__

#include <chrono>
#include <vector>

#include "glm/glm.hpp"
#include "camera.h"

//INFO TEMP:
extern glm::vec2 WindowSize;

// Struct holding the transformations to be applied to the object
typedef struct Transform {
  // Declare the constructors for a transform
  Transform(): position(glm::vec2(0.0f)), scale(glm::vec2(100.0f)), rotation(0.0f), z(1.0f) { }
  Transform(glm::vec2 _position): position{_position}, scale(glm::vec2(100.0f)), rotation(0.0f), z(1.0f) { }
  Transform(glm::vec2 _position, glm::vec2 _scale): position{_position}, scale{_scale}, rotation(0.0f), z(1.0f) { }
  Transform(glm::vec2 _position, glm::vec2 _scale, float _rotation): position{_position}, scale{_scale}, rotation{_rotation}, z(1.0f) { }
  Transform(glm::vec2 _position, glm::vec2 _scale, float _rotation, float _z): position{_position}, scale{_scale}, rotation{_rotation}, z{_z} { }

  // Fields controlled by transform
  glm::vec2 position;
  glm::vec2 origin;
  glm::vec2 scale;
  float rotation;
  float z;
};

// A structure holding time-related information
namespace Time {
  extern double delta;
}

// Timers based on delta time
class Timer {
  public: 
    // Tick the timer based on the delta time (in seconds)
    void tick(double delta_seconds);

    // Has the timer ticked down completely?
    bool test();

    // Reset the timer
    void reset();

    // Construct a new timer based on a time threshold
    Timer(double time_milliseconds) : time{time_milliseconds}, max_time{time_milliseconds} { }

    // Simplify evaluating the timer 
    explicit operator bool() { return test(); }

  private:
    // The maximum internal time threshold (in milliseconds)
    double max_time;
    
    // The internal timer (in milliseconds)
    double time;
};

// Struct holding a vector of vertices for an object
typedef struct Vertex {
  std::vector<glm::vec2> vertex_coordinates;
  std::vector<glm::vec2> texture_coordinates;

  void add(float px, float py, float tx, float ty) {
    this->vertex_coordinates.emplace_back(px, py);
    this->vertex_coordinates.emplace_back(tx, ty);
  }
  void add(glm::vec2 point_coord, glm::vec2 texture_coord) {
    this->vertex_coordinates.push_back(point_coord);
    this->vertex_coordinates.push_back(texture_coord);
  }
};

// Convert the screen coordinates to the world coordinates
glm::vec2 screen_to_world(glm::vec2 screen_point);

#endif
