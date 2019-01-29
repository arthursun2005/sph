layout (location = 0) out uvec2 f;

uniform sampler2D uP;
uniform ivec2 size;
uniform float h;
uniform int root;

uint _hash(in vec2 uv) {
    ivec2 ph = ivec2(floor(texture(uP, uv).xy/h));
    return hash(ph)%(size.x * size.y);
}

void main() {
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    f = uvec2(_hash(coord), flatten_texel_center(gl_FragCoord.xy, size));
}
