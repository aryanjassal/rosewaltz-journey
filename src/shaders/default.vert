#version 330 core
layout (location = 0) in vec3 pos_a;

void main() {
  gl_Position = vec4(pos_a.x, pos_a.y, pos_a.z, 1.0);
}