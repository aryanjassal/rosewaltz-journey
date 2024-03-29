#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "camera.h"
#include "glm/glm.hpp"

#include "texture.h"
#include "sprite.h"
#include "utils.h"
#include "physics.h"
#include "resource_manager.h"

// This class handles all game objects, containing boilerplate code for
// collision detection or motion or anything else an object might need.
// For complex object interactions, usage of this namespace is recommended
// over using the default SpriteRenderer, as it will only take you so far.
class GameObject {
  public:
    // Defines an name or handle for each GameObject
    std::string handle;

    // Defines a unique identifier for each GameObject
    unsigned long id;

    // Defines the transformations of the object
    Transform transform;

    // The offset to be added to the transform
    // Tip: This is typically used when paired up with another parent transform object
    glm::vec3 position_offset = glm::vec3(0.0f);

    // Defines the old transform before the object was clicked
    Transform old_transform;

    // Defines a vector of tags that the GameObject has
    // The tags can help filter GameObjects or pair them up together
    std::vector<std::string> tags;

    // Defines the texture the GameObject will render
    std::vector<Texture> texture;
    unsigned int texture_index = 0;

    // Defines the origin of the object
    glm::vec2 origin = glm::vec2(0.0f);

    // Define the grid-snap of the object
    glm::vec2 grid = glm::vec2(0.0f);

    // Snap controls whether the object should snap to a predefined grid or not.
    bool snap = false;

    // Swap controls whethe the object should swap with another object at the same position or not.
    bool swap = false;

    // Originate controls whether the origin setting will be respected or not.
    bool originate = false;

    // Inactive object won't be rendered or have any calculations run on them.
    bool active = true;

    // Interactivity controls whether the mouse should be able to interact with the object or not. 
    bool interactive = false;

    // Ridigbody controls if the object will be involved in physics collisions or not.
    bool rigidbody = false;

    // Locked controls whether the tile can move at all or not. This includes swapping and everything.
    bool locked = false;

    // Should the collider be revealed?
    // Tip: This is a debug function
    bool collider_revealed = false;

    // Flip the sprite on an axis without tampering with the scale or anything
    bool flip_x = false, flip_y = false;

    // Declare a parent object
    GameObject *parent = nullptr;

    // Declare the child objects
    std::vector<GameObject *> children = std::vector<GameObject *>();
  
    // Define a bounding box for the object.
    // This will be used in the collision detection and the collider used for mouse interaction
    BoundingBox bounding_box = BoundingBox();

    // Create an empty constructor for an object, as otherwise it won't play nice with std::map
    GameObject() { }

    // Actually render the GameObject using a SpriteRenderer
    void render(glm::vec4 colour = glm::vec4(1.0f), int focus = 0);

    // Translate the object to a given point
    void translate(glm::vec2 point);

    // Set or unset this object's parent
    void set_parent(GameObject *parent);
    void unset_parent();

    // Set or unset this object's child
    void set_child(GameObject *child);
    void unset_child(GameObject *child);

    // Check if the object is intersecting with a point
    bool check_point_intersection(glm::vec2 point);

    // Check collision between two bounding boxes
    Collision check_collision(GameObject *object);

    // Update the bounding box according to the new position of the object
    void update_bounding_box();

    // Update the position of the object based on the snap set
    void update_snap_position();
};

// This namespace handles generic functions related to dealing with GameObjects
namespace GameObjects {
  // Externally declare the main Renderer and the Camera
  extern SpriteRenderer *Renderer;
  extern OrthoCamera *Camera;

  // This namespace handles creating and dealing with Prefabs
  namespace ObjectPrefabs {
    GameObject *create(std::string handle, Texture texture, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());
    GameObject *create(std::string handle, std::vector<Texture> texture, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());
    GameObject *create(std::string handle, GameObject prefab);
    GameObject *get(std::string handle);

    // Load GameObject Prefabs from the given file (should be `required.prefabs` with R* syntax)
    void load_from_file(const char *file_path);
  }

  // Create a GameObject by providing all the required parameters
  GameObject *create(std::string handle, Texture texture, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());
  GameObject *create(std::string handle, std::vector<Texture> texture, std::vector<std::string> tags = std::vector<std::string>(), Transform transform = Transform());

  // Instantiate an existing prefab with all the required settings already set
  GameObject *instantiate(std::string prefab_handle);
  GameObject *instantiate(GameObject prefab);
  GameObject *instantiate(std::string prefab_handle, Transform transform);
  GameObject *instantiate(GameObject prefab, Transform transform);

  // Delete an instantiated object (only removes the object and not the prefab)
  void uninstantiate(std::string handle);
  void uninstantiate(unsigned long id);

  // Fetch the pointer to a GameObject from the list of GameObjects
  GameObject *get(std::string handle);

  // Fetch a vector with a pointer to all active GameObjects
  std::vector<GameObject *> all();

  // Filter all the GameObjects and return a vector with a pointer to active filtered GameObjects
  // Note: Any operation involving filtering is very performance-hungry and its use should be minimised
  std::vector<GameObject *> filter(std::string tag);
  std::vector<GameObject *> filter(std::vector<std::string> tags);

  // Filter all the GameObjects and return a vector with a pointer to active filtered GameObjects
  // Note: Any operation involving filtering is very performance-hungry and its use should be minimised
  std::vector<GameObject *> except(std::string tag);
}

#endif
