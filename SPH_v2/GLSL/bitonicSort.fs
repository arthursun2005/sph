layout (location = 0) out uvec2 f;

uniform usampler2D grid;

uniform int j;
uniform int k;
uniform int count2;
uniform ivec2 size;

void main() {
    int i = flatten_texel_center(gl_FragCoord.xy, size);
    
    int ixj = i ^ j;
    bool p = ((i&k) == 0) ^^ (i > ixj);
    
    uvec2 p1 = texture(grid, unflatten(i, size)).xy;
    uvec2 p2 = texture(grid, unflatten(ixj, size)).xy;
    
    if(i >= count2 || ixj >= count2) {
        
        f = p1;
        
    }else{
    
        if(p) f = (p1.x <= p2.x) ? p1 : p2;
        else f = (p1.x >= p2.x) ? p1 : p2;
        
    }
    vec2 k = unflatten(i, size) * size;
    //f = uvec2(i, uint(k.x));
}
