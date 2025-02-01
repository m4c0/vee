#version 450

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput in_c;
layout(location = 0) out vec4 out_c;

void main() {
  out_c = vec4(subpassLoad(in_c).rgb, 1.0);
}
