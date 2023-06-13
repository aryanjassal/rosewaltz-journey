#version 330 core

// Output the fragment colour we calculate in this shader
out vec4 pixel;

// Get the colours and the texture coordinates that were output from the vertex shader
in vec2 texture_coordinate;

// The texture and the colour uniform
uniform sampler2D sprite;
uniform vec4 colour;
uniform bool highlight;

void main() {
  // Set the default pixel colour
  pixel = vec4(0);

  // Display the texture
  if (highlight && (texture_coordinate.x <= 0.01f || texture_coordinate.x >= 0.99f || texture_coordinate.y <= 0.01f || texture_coordinate.y >= 0.99f)) {
    pixel += vec4(1.0f, 1.0f, 1.0f, 0.75f);
  }
  
  // Finally set the pixel colour
  pixel += colour * texture(sprite, texture_coordinate);
}
