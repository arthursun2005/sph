layout (location = 0) out vec2 vel_pos;

uniform sampler2D uP;
uniform sampler2D uV;

uniform vec2 bound;
uniform ivec2 size;

uniform int order;

void main() {
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    vec2 pos0 = texture(uP, coord).xy;
    vec2 vel0 = texture(uV, coord).xy;
    
    if(order == 0) {
        if(pos0.x < 0.0) pos0.x = 0.0;
        if(pos0.x > bound.x) pos0.x = bound.x;
        
        if(pos0.y < 0.0) pos0.y = 0.0;
        if(pos0.y > bound.y) pos0.y = bound.y;
        
        vel_pos = pos0;
    }else if(order == 1) {
        float dmp = -0.5;
        
        if(pos0.x < 0.0 && vel0.x < 0.0) vel0.x *= dmp;
        if(pos0.x > bound.x && vel0.x > 0.0) vel0.x *= dmp;
        
        if(pos0.y < 0.0 && vel0.y < 0.0) vel0.y *= dmp;
        if(pos0.y > bound.y && vel0.y > 0.0) vel0.y *= dmp;
        
        vel_pos = vel0;
    }
}
