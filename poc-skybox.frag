#version 450

layout(binding = 0) uniform sampler2D txt;

layout(location = 0) in vec3 f_pos;
layout(location = 0) out vec4 colour;

void main() {
  vec3 p = normalize(f_pos);
  // we are looking from inside the sphere, so coordinates are inverted. Also,
  // VUlkan inverts Y.
  p.x *= -1;

  const float r = 2.0; // Not sure how to make this non-int
  const float pi = 3.1415926535;
  float lng = r * atan(p.z, p.x);
  float lat = r * atan(p.y);

  vec2 uv = vec2(lng / pi, lat / (pi / 2));
  uv = uv * 0.5 + 0.5;
  colour = texture(txt, uv);
}
