#version 330 core

// Define a layout with two points for a vertex and two for the texture coordinates
layout (location = 0) in vec4 vertex;

// Output the color and texture coordinate for each pixel
out vec2 texture_coordinate;

// Uniform matrices for conversion from local space to world space to screen space
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  // Calculate the OpenGL vertex position
  //! Unsure about how the different matrices play together here
  gl_Position = projection * view * model * vec4(vertex.xy, 0.0f, 1.0f);

  // Also output the correct texture coordinate for each vertex
  texture_coordinate = vertex.zw;
}