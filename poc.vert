#version 450

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 frag_uv;

void main() {
  gl_Position = vec4(position, 0, 1);
  frag_uv = position * 0.5 + 0.5;
}
