layout (location = 0) out vec2 A;

uniform vec2 xy;
uniform float sp;
uniform int hx;
uniform int hy;
uniform ivec2 size;
uniform int count;

void main() {
    int on = flatten(gl_FragCoord.xy, size) - count;
    int h = int(floor(on/hx));
    int dx = on%hx - (hx >> 1);
    int dy = h - (hy >> 1);
    A = xy + vec2(dx * sp, dy * sp);
}
