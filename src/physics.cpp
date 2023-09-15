#include "physics.h"

Collision Physics::collider_point_collision(Collider collider, glm::vec2 point) {
  if ((collider.x <= point.x) 
    && (collider.x + collider.w >= point.x) 
    && (collider.y <= point.y) 
    && (collider.y + collider.h >= point.y)
  ) {
    // Note: No minimum translation vector is calculated or returned as the
    // typical use case of this is to check if the mouse is colliding with
    // a collider or not.
    return Collision(true);
  }
  return Collision();
}

Collision Physics::collider_collider_collision(Collider a, Collider b) {
  // Prepare to calculate the minimum overlap
  float overlap = std::numeric_limits<float>::max();

  // Calculate collisions using the Separation Axis Theorem
  for (int shape = 0; shape < 2; shape++) {
    // If we are testing the second shape, then swap a and b to check for all axes
    if (shape == 1) {
      Collider c = a;
      a = b;
      b = c;
    }

    // TODO TEMPORARY: Save memory bandwidth and processing time by preallocating all the vertices
    // instead of recalculating them every time a vertex is accessed.
    std::vector<glm::vec2> a_vertices = a.vertices();
    std::vector<glm::vec2> b_vertices = b.vertices();

    // Check for collisions for both objects
    for (int i = 0; i < a_vertices.size(); i++) {
      glm::vec2 normal;

      // Calculate the (normalised) vertex normals
      int m = (i + 1) % a_vertices.size();
      normal = glm::vec2(
        -(a_vertices[m].y - a_vertices[i].y),
        a_vertices[m].x - a_vertices[i].x
      );
      if (glm::length(normal)) normal /= glm::length(normal);

      // Calculate extents of projection of collider a onto the normal axis
      float a_min_extent = std::numeric_limits<float>::max();
      float a_max_extent = std::numeric_limits<float>::lowest();

      for (int j = 0; j < a_vertices.size(); j++) {
        float dot = glm::dot(a_vertices[j], normal);
        a_min_extent = std::min(a_min_extent, dot);
        a_max_extent = std::max(a_max_extent, dot);
      }

      // Calculate extents of projection of collider b onto the normal axis
      float b_min_extent = std::numeric_limits<float>::max();
      float b_max_extent = std::numeric_limits<float>::lowest();

      for (int j = 0; j < b_vertices.size(); j++) {
        float dot = glm::dot(b_vertices[j], normal);
        b_min_extent = std::min(b_min_extent, dot);
        b_max_extent = std::max(b_max_extent, dot);
      }

      // If a separation axis has been found, then there is no collision
      if (!(b_max_extent >= a_min_extent && a_max_extent >= b_min_extent)) return Collision();

      // Calculate the minimum translation distance
      overlap = std::min(std::max(a_max_extent, b_max_extent) - std::max(a_min_extent, b_min_extent), overlap);
    }
  }

  // Vector distance between the colliders
  glm::vec2 vd = glm::vec2(
    b.x - a.x,
    b.y - a.y
  );

  // Actual distance between the colliders
  float cd = sqrtf((vd.x * vd.x) + (vd.y * vd.y));

  // Calculate the minimum translation vector
  glm::vec2 mtv = glm::vec2(
    overlap * (vd.x / cd),
    overlap * (vd.y / cd)
  );

  // If all axes have been tested, then the shapes are colliding
  return Collision(true, mtv); 
}
