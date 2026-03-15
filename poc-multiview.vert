// Core in 1.1
#extension GL_EXT_multiview : require

void main() {
  vec2 pos = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1) * 3;
  gl_Position = vec4(pos - gl_ViewIndex, 0, 1);
}
