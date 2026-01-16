#version 450

void main() {
  vec2 pos = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1);
  gl_Position = vec4(pos * 2 - 1, 0, 1);
}
