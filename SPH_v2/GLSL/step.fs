layout (location = 0) out vec2 pos;

uniform sampler2D uV;
uniform sampler2D uP;

uniform ivec2 size;
uniform float dt;

uniform int count;

void main() {
    uint id = flatten_texel_center(gl_FragCoord.xy, size);
    
    if(id >= count) return;
    
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    vec2 vel0 = texture(uV, coord).xy;
    vec2 pos0 = texture(uP, coord).xy;
    
    pos0 += vel0 * dt;
    
    pos = pos0;
}
