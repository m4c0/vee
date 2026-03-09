#version 450

layout(binding = 0) uniform samplerCube txt;

layout(location = 0) in vec3 f_pos;
layout(location = 0) out vec4 colour;

void main() {
  vec3 p = normalize(f_pos);
  p.y *= -1; // Classical Vulkan being weird on Y

  colour = texture(txt, p);
}
