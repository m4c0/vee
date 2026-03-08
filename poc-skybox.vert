#version 450

layout(push_constant) uniform upc {
  vec2 mouse;
} pc;

layout(location = 0) out vec3 f_pos;

void main() {
  vec2 pos = vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1) * 3;
  vec3 p3 = vec3(pos * 2.0 - 1.0, 1.0);

  float ax = (10 * pc.mouse.x - 300.0) / 300.0;
  float ay = (pc.mouse.y - 300.0) / 300.0;

  const mat3 rot_y = mat3(
    cos(ax), 0, -sin(ax),
    0, 1, 0,
    sin(ax), 0, cos(ax) 
  );
  const mat3 rot_x = mat3(
    1, 0, 0,
    0, cos(ay), -sin(ay),
    0, sin(ay), cos(ay) 
  );

  f_pos = rot_y * rot_x * p3;
  gl_Position = vec4(p3, 1);
}
