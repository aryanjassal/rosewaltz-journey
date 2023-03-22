#version 330 core

// Create two layouts with 3 inputs each for coordinates and color, and one layout with 2 inputs for the texture coordinate
layout (location = 0) in vec3 pos_a;
layout (location = 1) in vec3 color_a;
layout (location = 2) in vec2 texture_a;

// Output the color and texture coordinate for each fragment
// out vec3 color;
out vec2 texture_coordinate;

// A uniform for the translation matrix
uniform mat4 transform;

void main() {
  // The OpenGL position will be the current vertex coordinates and an arbirary fourth dimension
  gl_Position = transform * vec4(pos_a, 1.0);

  // Also output the correct color and texture coordinate for each vertex
  // color = color_a;
  texture_coordinate = texture_a;
}