layout (location = 0) out vec2 pos;

uniform sampler2D uV;
uniform sampler2D uP;

uniform ivec2 size;
uniform float dt;

void main() {
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    vec2 vel0 = texture(uV, coord).xy;
    vec2 pos0 = texture(uP, coord).xy;
    
    pos0 += vel0 * dt;
    
    pos = pos0;
}
