#version 330 core

// Define a layout with two points for a vertex and two for the texture coordinates
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texture;

// Output the color and texture coordinate for each pixel
out vec2 texture_coordinate;

// Uniform matrices for conversion from local space to world space to screen space
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool highlight;

void main() {
  // Calculate the OpenGL vertex position
  gl_Position = projection * view * model * vec4(vertex, 0.0, 1.0);

  // Output the correct texture coordinate for each vertex
  texture_coordinate = texture;
}
