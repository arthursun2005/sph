layout (location = 0) out uvec2 f;

uniform usampler2D grid;

uniform float stride;
uniform vec2 start;
uniform bool dir;
uniform int count2;
uniform ivec2 size;

void main() {
    vec2 ixj;
    if(gl_FragCoord.x > 0.5) {
        ixj = start - vec2(stride, 0.0);
    }else{
        ixj = start + vec2(stride, 0.0);
    }
    
    vec2 i = vec2(start.x + gl_FragCoord.x / float(size.x), start.y);
    
    bool p = (i.x > ixj.x) ^^ dir;
    
    //uvec2 p1 = texture(grid, i).xy;
    //uvec2 p2 = texture(grid, ixj).xy;
    
    //if(p) f = (p1.x <= p2.x) ? p1 : p2;
    //else f = (p1.x >= p2.x) ? p1 : p2;
    
}

