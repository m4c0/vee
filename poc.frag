#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;
layout(push_constant) uniform upc {
  vec2 mouse_pos;
} pc;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 frag_color;

void main() {
  float d = pow(clamp(1.0 - length(pc.mouse_pos - gl_FragCoord.xy) / 256.0, 0.0, 1.0), 2.0);
  vec4 tex_color = texture(tex, frag_uv);
  frag_color = mix(vec4(d, 1, 0, 1), tex_color, tex_color.a);
}
