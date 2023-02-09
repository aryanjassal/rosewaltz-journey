#version 330 core

// Create two layouts with 3 inputs each for coordinates and color
layout (location = 0) in vec3 pos_a;
layout (location = 1) in vec3 color_a;

// Output the color for each fragment
out vec3 color;

// A uniform triangle scale
uniform float scale;

void main() {
  // The OpenGL position will be the current vertex coordinates and an arbirary fourth dimensio
  gl_Position = vec4(pos_a.x * scale, pos_a.y * scale, pos_a.z * scale, 1.0);

  // Also output the correct color for each vertex
  color = color_a;
}