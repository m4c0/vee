#version 450

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 frag_color;

void main() {
  vec4 tex_color = texture(tex, frag_uv);
  frag_color = mix(vec4(0, 1, 0, 1), tex_color, tex_color.a);
}
