layout (location = 0) out uvec2 f;

uniform usampler2D grid;

uniform int j;
uniform int k;
uniform int count2;
uniform ivec2 size;

in vec2 coord;

void main() {
    int i = flatten_texel_center(gl_FragCoord.xy, size);
    
    if(i >= count2) discard;
    
    int ixj = i ^ j;
    
    uvec2 p1 = texture(grid, coord).xy;
    uvec2 p2 = texture(grid, unflatten(ixj, size)).xy;
    
    if(((i&k) == 0) ^^ (i > ixj)) {
        f = p1.x <= p2.x ? p1 : p2;
    }else{
        f = p1.x >= p2.x ? p1 : p2;
    }
}
