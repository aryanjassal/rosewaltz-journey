#version 330 core

// Output the fragment colour we calculate in this shader
out vec4 pixel;

// Get the colours and the texture coordinates that were output from the vertex shader
in vec2 texture_coordinate;

// The texture and the colour uniform
uniform sampler2D sprite;
uniform vec4 colour;
uniform int focus;

void main() {
  // Set the default pixel colour
  pixel = colour * texture(sprite, texture_coordinate);

  // Display the texture
  if (texture_coordinate.x <= 0.005f || texture_coordinate.x >= 0.995f || texture_coordinate.y <= 0.005f || texture_coordinate.y >= 0.995f) {
    if (focus == 1) { pixel += vec4(0.796f, 0.482f, 0.494f, 0.6f); }
    else if (focus == -1) { pixel += vec4(0.4f, 0.4f, 0.4f, 0.6f); }
    else if (focus == -2) { pixel += vec4(1.0f, 0.843f, 0.0f, 0.6f); }
  }
}
