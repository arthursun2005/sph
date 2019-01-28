layout (location = 0) in vec2 uv;

layout (location = 1) in vec2 pos;

uniform sampler2D T;
uniform vec2 scl1;
uniform vec2 scl2;

out vec2 coord;

void main() {
    coord = uv;
    gl_Position = vec4(texture(T, uv).xy * scl1 + pos * scl2, 0.0, 1.0);
}
