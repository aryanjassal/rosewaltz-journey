#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <map>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "texture.h"
#include "sprite.h"

// This namespace handles all game objects, containing boilerplate code for
// collision detection or motion or anything else an object might need.
// For complex object interactions, usage of this namespace is recommended
// over using the default SpriteRenderer as it can only take you so far.
namespace GameObject {
  class Object {
    public:
      // Define variables for game objects
      glm::vec2 position;
      glm::vec2 scale;
      Texture texture;
      Camera::OrthoCamera *camera;
      bool active;
      bool interactive;

      typedef struct BoundingBox {
        int top;
        int bottom;
        int left;
        int right; 
      };
      BoundingBox bounding_box;

      // If snap is set, then the object will be grid-snapped in the given values
      glm::vec2 snap = glm::vec2(0, 0);

      // A game object needs its texture, position, and its scale
      Object();

      // Actually render the object using a sprite renderer
      void render(SpriteRenderer *renderer);

      // Translate the object to a particular point
      void translate_to_point(glm::vec2 point);

      // Check if the object is intersecting with a point
      bool check_point_intersection(glm::vec2 point);

      // Update the position by rerunning all calculations that would be run while updating position
      void update_position();
    
    private:
      // Update the bounding box according to the new position of the object
      void update_bounding_box();

      // Update the position of the object based on the snap set
      void update_snap_position();
  };

  // Keep track of all the game objects created
  static std::map<std::string, GameObject::Object> Objects;

  // Create a new game object instance
  Object *create(std::string handle, Camera::OrthoCamera *camera, Texture texture, glm::vec2 position = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(100.0f), glm::vec2 snap = glm::vec2(0.0f));

  // Fetch the game object
  Object *get(std::string handle);

  // Fetch all game objects being handled by the game
  std::vector<Object *> all();

  // Update an already defined game object within the GameObject manager using its handle
  void update(std::string handle, Object object);
}

#endif