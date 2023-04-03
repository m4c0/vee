#version 450

layout(push_constant) uniform U {
  vec2 mouse_pos;
  float scale_factor;
  float vert_scale;
} u;

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_uv;

void main() {
  gl_Position = vec4(u.vert_scale * position, 0, 1);
  frag_uv = position * 0.5 + 0.5;
}
