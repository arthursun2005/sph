layout (location = 0) out uvec2 f;

uniform sampler2D uP;
uniform vec2 invSize;
uniform float h;
uniform int root;

uint hash(in vec2 uv) {
    ivec2 ph = ivec2(floor(texture(uP, uv).xy/h));
    return uint((ph.x * PX) ^ (ph.y * PY))%(root * root);
}

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    uvec2 u = uvec2(gl_FragCoord.xy - 0.5);
    f = uvec2(hash(coord), u.x + u.y * root);
}
