layout (location = 0) out ivec2 f;

uniform sampler2D uP;
uniform ivec2 size;
uniform float h;
uniform int root;

int _hash(in ivec2 tx) {
    ivec2 ph = ivec2(floor(texelFetch(uP, tx, 0).xy/h));
    return hash(ph, size.x * size.y);
}

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    f = ivec2(_hash(coord), flatten(gl_FragCoord.xy, size));
}
