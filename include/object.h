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

// A struct to define the structure of information regarding the GameObject's bounding box
typedef struct BoundingBox {
  float top;
  float bottom;
  float left;
  float right;
};

// Directional enum to handle collision direction
typedef enum Direction {
  UP,
  RIGHT,
  DOWN,
  LEFT
};

// This class handles all game objects, containing boilerplate code for
// collision detection or motion or anything else an object might need.
// For complex object interactions, usage of this namespace is recommended
// over using the default SpriteRenderer, as it will only take you so far.
class GameObject {
  public:
    // Defines a unique identifier for each GameObject
    std::string handle;

    // Defines the transformations of the object
    Transform transform;

    // Defines the old transform before the object was clicked
    Transform old_transform;

    // Stores the change in transformation over the previous frame
    Transform delta_transform;

    // Defines a vector of tags that the GameObject has
    // The tags can help filter GameObjects or pair them up together
    std::vector<std::string> tags;

    // Defines the texture the GameObject will render
    Texture texture;

    // Stores the GameCamera that will be rendering the scene
    OrthoCamera *camera;

    // Defines the position, scale, origin, and the grid-snap of the object
    // The origin of the object is a transform operation that will be executed before any other transformation
    glm::vec2 origin, grid;

    // Snap controls wheather the object should snap to a predefined grid or not.
    // Swap controls wheathe the object should swap with another object at the same position or not.
    // Originate controls wheather the origin setting will be respected or not.
    // Inactive object won't be rendered or have any calculations run on them.
    // Interactivity controls whether the mouse should be able to interact with the object or not. 
    // Ridigbody controls if the object will be involved in physics collisions or not.
    bool snap, swap, originate, active, interactive, rigidbody;
  
    // The window dimensions are needed to ensure correct bounding box calculation when resizing the viewport
    // and not the camera's matrices in order to keep the Objects' size consistent across screen sizes.
    glm::vec2 window_dimensions;

    // Define a bounding box for the object.
    // This will be used in the collision detection and the collider used for mouse interaction
    BoundingBox bounding_box;

    // Create an empty constructor for an object, as otherwise it won't play nice with std::map
    GameObject() { }

    // Actually render the GameObject using a SpriteRenderer
    void render(SpriteRenderer *renderer, glm::vec4 colour = glm::vec4(1.0f));

    // Translate the object to a given point
    void translate_to_point(glm::vec2 point, bool convert = true);

    // Check if the object is intersecting with a point
    bool check_point_intersection(glm::vec2 point, bool convert = true);

    // Check collision between two bounding boxes
    bool check_collision(BoundingBox bounding_box);

    // Update the position by rerunning all calculations that would be run while updating position
    void update_position();
  
    // Update the bounding box according to the new position of the object
    void update_bounding_box();

    // Update the position of the object based on the snap set
    void update_snap_position();

    // Add a new point in delta transform
    void update_delta_transform(glm::vec3 old_position);
};

// This namespace handles generic functions related to dealing with GameObjects
namespace GameObjects {
  // Store a list of all the GameObjects ever created
  static std::map<std::string, GameObject> Objects;

  // Create a GameObject by providing all the required parameters
  GameObject *create(
    std::string handle, 
    OrthoCamera *camera, 
    Texture texture, 
    glm::vec2 window_dimensions, 
    std::vector<std::string> tags = std::vector<std::string>(),
    Transform transform = { glm::vec3(0.0f), glm::vec2(100.0f), 0.0f },
    glm::vec2 origin = glm::vec2(0.0f),
    glm::vec2 grid = glm::vec2(0.0f)
  );
  GameObject *create(
    std::string handle, 
    OrthoCamera *camera, 
    Texture texture, 
    glm::vec2 window_dimensions, 
    std::vector<std::string> tags = std::vector<std::string>(),
    glm::vec3 position = glm::vec3(0.0f),
    glm::vec2 scale = glm::vec2(100.0f), 
    float rotation = 0.0f,
    glm::vec2 origin = glm::vec2(0.0f),
    glm::vec2 grid = glm::vec2(0.0f)
  );

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
