#version 330 core

// Output the fragment colour we calculate in this shader
out vec4 pixel;

// Get the colours and the texture coordinates that were output from the vertex shader
in vec2 texture_coordinate;

// The texture and the colour uniform
uniform sampler2D sprite;
uniform vec4 colour;

void main() {
  // Display the texture
  pixel = colour * texture(sprite, texture_coordinate);
}
