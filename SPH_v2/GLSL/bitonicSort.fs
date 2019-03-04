layout (location = 0) out ivec2 f;

uniform isampler2D grid;

uniform int j;
uniform int k;
uniform int count2;
uniform ivec2 size;

in vec2 coord;

void main() {
    int i = flatten(gl_FragCoord.xy, size);
    
    if(i >= count2) discard;
    
    int ixj = i ^ j;
    
    ivec2 c = ivec2(coord * vec2(size) - 0.5);
    
    ivec2 p1 = texelFetch(grid, c, 0).xy;
    ivec2 p2 = texelFetch(grid, ivec2(unflatten(ixj, size) - 0.5), 0).xy;
    
    if(((i&k) == 0) ^^ (i > ixj)) {
        f = p1.x <= p2.x ? p1 : p2;
    }else{
        f = p1.x >= p2.x ? p1 : p2;
    }
}
