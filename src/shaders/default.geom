#version 330 core
#define MAX_VERT 3

layout (triangles) in;
layout (triangle_strip, max_vertices = MAX_VERT) out;

in vec2 texture_coordinate[];
out vec2 modified_texture_coordinate;

void main() {
  for (int i = 0; i < MAX_VERT; i++) {
    modified_texture_coordinate = texture_coordinate[i];
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}
