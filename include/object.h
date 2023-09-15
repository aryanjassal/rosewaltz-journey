#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "glm/glm.hpp"

#include "texture.h"
#include "renderer.h"
#include "utils.h"
#include "physics.h"
#include "resource_manager.h"
#include "tutils.h"
#include "camera.h"

// This class handles all game objects, containing boilerplate code for
// collision detection or motion or anything else an object might need.
// For complex object interactions, usage of this namespace is recommended
// over using the default SpriteRenderer, as it will only take you so far.
class Entity {
  public:
    // Defines an name or handle for each object
    std::string handle;

    // Defines a unique identifier for each object
    unsigned long id;

    // Defines the transform of the object
    Transform transform;
    Transform old_transform;

    // Define the snap grid of the object
    glm::vec2 grid = glm::vec2(0.0f);

    // Defines a vector of tags that the object has.
    // The tags can help filter object or pair them up together.
    std::vector<std::string> tags;

    // The texture map information for the object
    TextureMap texture_map;

    // Inactive object won't be rendered or have any calculations run on them.
    bool active = true;

    // Interactivity controls whether the mouse should be able to interact with the object or not. 
    bool interactive = false;

    // Ridigbody controls if the object will be involved in physics collisions or not.
    bool rigidbody = false;

    // Snap controls whether the object should snap to a predefined grid or not.
    bool can_snap = false;

    // Reveal the bound colliders and the origin in a visual format.
    bool debug_colliders = false;

    // Flip the sprite on an axis.
    void flip_x();
    void flip_y();

    // The parent of this object.
    Entity *parent = nullptr;

    // The object's constructor
    Entity();
    Entity(std::string handle, TextureMap texture_map, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());

    // Render the object
    void render(glm::vec4 colour = glm::vec4(1.0f));

    // Translate the object to a given point
    void translate(glm::vec2 point);
  
    // Stores a pointer to all the object's children
    std::vector<Entity*> children = std::vector<Entity*>();

    // Set the object's parent
    void set_parent(Entity *parent);
    void unset_parent();

    // Set the object's child
    void set_child(Entity *child);
    void unset_child(Entity *child);

    // Update the position of the object based on the snap set
    void snap();

    // Check collision between this object and another entity
    Collision check_collision(Entity *object);
    Collision check_collision(glm::vec2 point);

    // Define a bounding box for the object. This will be used in the
    // collision detection and the collider used for mouse interaction.
    std::vector<Collider> colliders = std::vector<Collider>();

    // Create a number of colliders with attributes
    void create_colliders(int n, float x, float y, float w, float h, float angle);
    void create_colliders(int n, float x, float y, float w, float h);
    void create_colliders(int n);

    // Update the bounding box according to the new position of the object
    void update_colliders(int index, float x, float y, float w, float h);
    void update_colliders(int index, float x, float y);
    void update_colliders();
};

// class Tile : Entity {
//   public:
//     // Define the snap grid of the object
//     glm::vec2 grid = glm::vec2(0.0f);
//
//     // Update the position of the object based on the snap set
//     void snap();
// };

// This namespace handles generic functions related to dealing with Entities
namespace Entities {
  // Externally declare the main Renderer and the Camera
  extern Renderer *Renderer;
  extern Camera *Camera;

  // This namespace handles creating and dealing with Prefabs
  namespace Prefabs {
    Entity *create(std::string handle, Texture texture);
    Entity *create(std::string handle, TextureMap texture_map);
    Entity *create(std::string handle, Texture texture, std::string tag = "", Transform transform = Transform());
    Entity *create(std::string handle, TextureMap texture_map, std::string tag = "", Transform transform = Transform());
    Entity *create(std::string handle, Texture texture, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());
    Entity *create(std::string handle, TextureMap texture_map, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());
    Entity *create(std::string handle, Entity prefab);
    Entity *get(std::string handle);

    // Load Entity Prefabs from the given file (should be `required.prefabs` with R* syntax)
    void load_from_file(const char *file_path);
  }

  // Instantiate an existing prefab with all the required settings already set
  Entity *instantiate(std::string prefab_handle);
  Entity *instantiate(Entity prefab);
  Entity *instantiate(std::string prefab_handle, Transform transform);
  Entity *instantiate(Entity prefab, Transform transform);

  // Delete an instantiated object (only removes the object and not the prefab)
  void uninstantiate(std::string handle);
  void uninstantiate(unsigned long id);
  void uninstantiate_all();

  // Fetch the pointer to an enitiy from the list of entities
  Entity *get(std::string handle);
  Entity *get(unsigned int id);

  // Fetch a vector with a pointer to all active entities
  std::vector<Entity*> all();

  // Filter all the active entities and return a vector with a pointer to the filtered entities
  // Note: Any operation involving filtering is performance-hungry [O(n^2) complexity] and its use should be minimised
  std::vector<Entity*> filter(std::string tag);
  std::vector<Entity*> filter(std::vector<std::string> tags);
  std::vector<Entity*> except(std::string tag);
}

#endif
