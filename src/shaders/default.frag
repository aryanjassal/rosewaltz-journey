#version 330 core

// Output the fragment color we calculate in this shader
out vec4 frag_color;

// Get the colors and the texture coordinates that were output from the vertex shader
// in vec3 color;
in vec2 texture_coordinate;

// The uniform texture
uniform sampler2D frag_texture;

void main() {
  // Display the texture  
  frag_color = texture(frag_texture, texture_coordinate);
}