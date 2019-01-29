layout (location = 0) in vec2 uv;

uniform usampler2D grid;
uniform int root;
uniform ivec2 size;

out vec2 A;

void main() {
    uint k = texture(grid, uv).x;
    vec2 c = unflatten(k, size);
    A = uv * float(root);
    gl_Position = vec4(c * 2.0 - 1.0, 0.0, 1.0);
}
