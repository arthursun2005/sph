layout (location = 0) out vec2 pos;

uniform sampler2D uV;
uniform sampler2D uP;

uniform vec2 bound;

uniform ivec2 size;
uniform float dt;

uniform int count;

void main() {
    int id = flatten(gl_FragCoord.xy, size);
    
    if(id >= count) return;
    
    ivec2 coord = ivec2(gl_FragCoord.xy);
    
    vec2 vel0 = texelFetch(uV, coord, 0).xy;
    vec2 pos0 = texelFetch(uP, coord, 0).xy;
    
    pos0 += vel0 * dt;
    
    if(pos0.x < 0.0) pos0.x = 0.0;
    if(pos0.x > bound.x) pos0.x = bound.x;
    
    if(pos0.y < 0.0) pos0.y = 0.0;
    if(pos0.y > bound.y) pos0.y = bound.y;
    
    pos = pos0;
}
