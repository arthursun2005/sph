layout (location = 0) out vec2 A;

uniform vec2 xy;
uniform float sp;
uniform int hx;
uniform int hy;
uniform int root;
uniform int count;

void main() {
    ivec2 ul = ivec2(gl_FragCoord.xy - 0.5);
    int on = ul.x + root * ul.y - count;
    int h = int(floor(on/hx));
    int dx = on%hx - (hx >> 1);
    int dy = h - (hy >> 1);
    A = xy + vec2(dx * sp, dy * sp);
}
