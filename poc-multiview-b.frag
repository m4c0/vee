layout(binding = 0) uniform sampler2DArray t0;
layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 colour;

void main() {
  colour = texture(t0, vec3(uv, step(0.5, uv.x)));
}
