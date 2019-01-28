layout (location = 0) in vec2 position;

uniform vec2 S;
uniform vec2 P;

out vec2 at;

void main() {
    at = position;
    gl_Position = vec4(position * S + P, 0.0, 1.0);
}
