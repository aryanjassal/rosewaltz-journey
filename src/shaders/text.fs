#version 330 core

in vec2 texture_coordinates;
out vec4 colour;

uniform sampler2D text;
uniform vec4 text_colour;

void main() {
  vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(text, texture_coordinates).r);
  colour = text_colour * sampled;
}
