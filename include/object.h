#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "camera.h"
#include "physics.h"
#include "renderer.h"
#include "resource_manager.h"
#include "texture.h"
#include "tutils.h"
#include "utils.h"

using ID = uint64_t;

// This class handles all entities, containing boilerplate code for collision detection or motion or anything else an entity might need.
class Entity {
public:
  // Defines an name or handle for each object
  std::string handle;

  // Defines a unique identifier for each object
  ID id;

  // Defines the transform of the object
  Transform transform;
  Transform old_transform;

  // Defines a vector of tags that the object has.
  // The tags can help filter object or pair them up together.
  std::vector<std::string> tags;

  // The texture map information for the object
  TextureMap texture_map;

  // Inactive object won't be rendered or have any calculations run on them.
  bool active = true;

  // Interactivity controls whether the mouse should be able to interact with
  // the object or not.
  bool interactive = false;

  // Ridigbody controls if the object will be involved in physics collisions or
  // not.
  bool rigidbody = false;

  // Reveal the bound colliders and the origin in a visual format.
  bool debug_colliders = false;

  // Flip the sprite on an axis.
  void flip_x();
  void flip_y();

  // The parent of this object.
  Entity *parent = nullptr;

  // The object's constructor
  Entity();
  Entity(std::string handle, TextureMap texture_map,
         std::vector<std::string> tags = std::vector<std::string>(),
         Transform transform = Transform());

  // Render the object
  void render();
  void render(glm::vec4 colour);

  // Translate the object to a given point
  void translate(glm::vec2 point);

  // Stores a pointer to all the object's children
  std::vector<Entity *> children = std::vector<Entity *>();

  // Set the object's parent
  void set_parent(Entity *parent);
  void unset_parent();

  // Set the object's child
  void set_child(Entity *child);
  void unset_child(Entity *child);

  // Check collision between this object and another entity
  Collision check_collision(Entity *object);
  Collision check_collision(glm::vec2 point);

  // Define a bounding box for the object. This will be used in the
  // collision detection and the collider used for mouse interaction.
  std::vector<Collider> colliders = std::vector<Collider>();

  // Create colliders with attributes
  void create_colliders(float x, float y, float w, float h, float angle);
  void create_colliders(float x, float y, float w, float h);
  void create_colliders(int n);

  // Update the bounding box according to the new position of the object
  void update_colliders(int index, float x, float y, float w, float h);
  void update_colliders(int index, float x, float y);
  void update_colliders();
};

class Tile : Entity {
public:
  // Define the snap grid of the object
  glm::vec2 grid = glm::vec2(0.0f);

  // Update the position of the object based on the snap set
  void snap();
};

// This namespace handles generic functions related to dealing with Entities
namespace Entities {
// Externally declare the main Renderer and the Camera
extern Renderer *Renderer;
extern Camera *Camera;

// This namespace handles creating and dealing with Prefabs
namespace Prefabs {
Entity *create(std::string handle, TextureMap texture_map,
               std::vector<std::string> tags = std::vector<std::string>(),
               Transform transform = Transform());
Entity *create(std::string handle, Entity prefab);
Entity *get(std::string handle);
} // namespace Prefabs

// Instantiate an existing prefab with all the required settings already set
Entity *instantiate(std::string prefab_handle);
Entity *instantiate(Entity prefab);
Entity *instantiate(std::string prefab_handle, Transform transform);
Entity *instantiate(Entity prefab, Transform transform);

// Delete an instantiated object (only removes the object and not the prefab)
void uninstantiate(std::string handle);
void uninstantiate(ID id);
void uninstantiate_all();

// Fetch the pointer to an entity from the list of entities
Entity *get(std::string handle);
Entity *get(ID id);

// Fetch a vector with a pointer to all active entities
std::vector<Entity *> all();
} // namespace Entities

#endif
