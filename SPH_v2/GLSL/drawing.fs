layout (location = 0) out vec4 A;

uniform sampler2D uV;
uniform sampler2D uW;

in vec2 coord;

void main() {
    vec2 v = texture(uV, coord).xy;
    float m = length(v);
    A = 1.0 * (vec4(0.5, 0.5, 1.0, 0.5) + m * vec4(0.05, 0.05, -0.01, 0.0));
}
