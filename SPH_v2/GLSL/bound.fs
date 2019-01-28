layout (location = 0) out vec2 vel;

uniform sampler2D uP;
uniform sampler2D uV;

uniform vec2 bound;
uniform vec2 invSize;

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 pos0 = texture(uP, coord).xy;
    vec2 vel0 = texture(uV, coord).xy;
    
    float dmp = -0.5;
    
    if(pos0.x < 0.0) {
        if(vel0.x < 0.0) vel0.x *= dmp;
    }
    if(pos0.x > bound.x) {
        if(vel0.x > 0.0) vel0.x *= dmp;
    }
    
    if(pos0.y < 0.0) {
        if(vel0.y < 0.0) vel0.y *= dmp;
    }
    if(pos0.y > bound.y) {
        if(vel0.y > 0.0) vel0.y *= dmp;
    }
    
    vel = vel0;
}
