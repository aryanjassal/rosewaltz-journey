#version 330 core

// Output the fragment colour we calculate in this shader
out vec4 frag_colour;

// Get the colours and the texture coordinates that were output from the vertex shader
in vec2 texture_coordinate;

// The texture and the colour uniform
uniform sampler2D frag_texture;
uniform vec3 colour;

void main() {
  // Display the texture  
  frag_colour = texture(frag_texture, texture_coordinate) * vec4(colour.xyz, 1.0f);
}