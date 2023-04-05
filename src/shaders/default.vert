#version 330 core

// Create two layouts with 3 inputs each for coordinates and color, and one layout with 2 inputs for the texture coordinate
layout (location = 0) in vec3 pos_a;
layout (location = 1) in vec2 texture_a;

// Output the color and texture coordinate for each fragment
out vec2 texture_coordinate;

// // A uniform for the translation matrix
// uniform mat4 transform_matrix;

// Uniforms for the perspective camera matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
  // Calculate the OpenGL vertex position
  // gl_Position = projection_matrix * view_matrix * model_matrix * transform_matrix * vec4(pos_a, 1.0);
  gl_Position = projection_matrix * view_matrix * model_matrix * vec4(pos_a, 1.0);

  // Also output the correct texture coordinate for each vertex
  texture_coordinate = texture_a;
}