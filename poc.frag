#version 450

layout(constant_id = 0) const float k = 0;
layout(constant_id = 9) const float j = 1;

layout(set = 0, binding = 0) uniform sampler2D tex;
layout(push_constant) uniform upc {
  vec2 mouse_pos;
  float scale_factor;
  float vert_scale;
} pc;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 frag_color;

void main() {
  float len = length(pc.mouse_pos * pc.scale_factor - gl_FragCoord.xy);
  float d = pow(clamp(1.0 - len / 256.0, 0.0, 1.0), 2.0);
  vec4 tex_color = texture(tex, frag_uv);
  frag_color = mix(vec4(d, k, j, 1), tex_color, tex_color.a);
}
