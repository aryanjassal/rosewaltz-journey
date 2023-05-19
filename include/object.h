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
      std::string handle;
      glm::vec2 position;
      glm::vec2 scale;
      glm::vec2 window_dimensions;
      Texture texture;
      Camera::OrthoCamera *camera;
      bool active;
      bool interactive;
      glm::vec2 origin = glm::vec2(0.0f);

      //TODO: get current and previous grid/snap cell of the object and use it to swap or snap the objects

      // Define the bounding box of the object
      typedef struct BoundingBox {
        float top;
        float bottom;
        float left;
        float right; 
      };
      BoundingBox bounding_box;

      // If snap is set, then the object will be grid-snapped in the given values
      bool snap = false;
      glm::vec2 snap_grid = glm::vec2(0.0f);

      // A game object needs its texture, position, and its scale
      Object() { }

      // Actually render the object using a sprite renderer
      void render(SpriteRenderer *renderer);

      // Translate the object to a particular point
      void translate_to_point(glm::vec2 point, glm::vec2 origin = glm::vec2(0.0f));

      // Check if the object is intersecting with a point
      bool check_point_intersection(glm::vec2 point, glm::vec2 origin = glm::vec2(0.0f));

      // Update the position by rerunning all calculations that would be run while updating position
      void update_position();
    
      // Update the bounding box according to the new position of the object
      void update_bounding_box();

      // Update the position of the object based on the snap set
      void update_snap_position();
  };

  // If you need to group multiple GameObjects and need them to behave as one singular
  // GameObject, then use this class.
  class ObjectGroup : public GameObject::Object {
    public:
      // Construct the object
      ObjectGroup() { }

      // Keep track of all the GameObjects in this group
      std::map<std::string, GameObject::Object> Objects;

      // Returns a vector of all the GameObjects in this group
      std::vector<GameObject::Object *> all(); 

      // Add a GameObject to the group
      void add(std::string handle, GameObject::Object *object);

      // Get an object via its handle
      GameObject::Object *get(std::string handle);

      // Update the object information of an object
      void update(std::string handle, GameObject::Object);

      // Render the objects in the group
      void render(SpriteRenderer *renderer);
      void render(SpriteRenderer *renderer, std::string handle);

      // Remove a GameObject from the group
      void remove(std::string handle);

      // Update the cumulative bounding box of the group
      void update_bounding_box();
  };

  // Keep track of all the game objects created
  static std::map<std::string, GameObject::Object> Objects;
  static std::map<std::string, GameObject::ObjectGroup> ObjectGroups;

  // Create a new game object instance
  Object *create(std::string handle, Camera::OrthoCamera *camera, Texture texture, glm::vec2 window_dimensions, glm::vec2 position = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(100.0f), glm::vec2 snap = glm::vec2(0.0f));
  
  // Create a ObjectGroup of GameObjects
  ObjectGroup *create_group(std::string handle);
  
  // Update an object group
  // Note that the name is same as that of the update game object. It is fine as there are different types of parameters
  void update(std::string handle, ObjectGroup *object_group);

  // Fetch the game object
  Object *get(std::string handle);
  ObjectGroup *get_group(std::string handle);

  // Fetch all game objects being handled by the game
  std::vector<Object *> all();

  // Update an already defined game object within the GameObject manager using its handle
  void update(std::string handle, Object object);
}

#endif
