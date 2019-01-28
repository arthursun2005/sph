layout (location = 0) out vec2 pos;

uniform sampler2D uV;
uniform sampler2D uP;
uniform vec2 bound;
uniform vec2 invSize;
uniform float dt;

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 vel0 = texture(uV, coord).xy;
    vec2 pos0 = texture(uP, coord).xy;
    
    pos0 += vel0 * dt;
    
    if(pos0.x < 0.0) pos0.x = 0.0;
    if(pos0.x > bound.x) pos0.x = bound.x;
    
    if(pos0.y < 0.0) pos0.y = 0.0;
    if(pos0.y > bound.y) pos0.y = bound.y;
    
    pos = pos0;
}
