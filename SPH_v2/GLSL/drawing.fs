layout (location = 0) out vec4 A;

uniform sampler2D uV;

in vec2 coord;

void main() {
    vec2 v = texture(uV, coord).xy;
    float m = dot(v, v);
    A = vec4(0.5, 0.5, 1.0, 1.0) + 0.1 * vec4(m, m, 0.0, 0.0);
}
