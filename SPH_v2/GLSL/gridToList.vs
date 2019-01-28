layout (location = 0) in vec2 uv;

uniform usampler2D grid;
uniform int root;
uniform vec2 invSize;

out vec2 A;

void main() {
    uint k = texture(grid, uv).x;
    vec2 c = vec2(float(k%root) + 0.5, floor(float(k)/float(root)) + 0.5) * invSize;
    A = uv * float(root);
    gl_Position = vec4(c * 2.0 - 1.0, 0.0, 1.0);
}
