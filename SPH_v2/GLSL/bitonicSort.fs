layout (location = 0) out uvec2 f;

uniform usampler2D grid;

uniform int root;

uniform int j;
uniform int k;
uniform int count2;
uniform vec2 invSize;

vec2 ii(in int index) {
    return vec2(float(index%root) + 0.5, floor(float(index)/float(root)) + 0.5) * invSize;
}

void main() {
    ivec2 l = ivec2(gl_FragCoord.xy - 0.5);
    int i = l.x + l.y * root;
    
    int ixj = i ^ j;
    bool p = ((i&k) == 0) ^^ (i > ixj);
    
    uvec2 p1 = texture(grid, ii(i)).xy;
    uvec2 p2 = texture(grid, ii(ixj)).xy;
    
    //if(p) f = (p1.x <= p2.x) ? p1 : p2;
    //else f = (p1.x >= p2.x) ? p1 : p2;
    f = uvec2(12, 34);
}
