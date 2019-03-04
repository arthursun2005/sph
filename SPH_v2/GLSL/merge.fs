layout (location = 0) out ivec2 f;

uniform isampler2D grid;

uniform int stage;
uniform int count;
uniform ivec2 size;

void main() {
    int id = flatten(gl_FragCoord.xy, size);
    
    if(id >= count) discard;
    
    int it = 1 << stage;
    int it2 = 1 << (stage - 1);
    int localId = id%it;
    
    ivec2 ans = ivec2(0, 0);
    int ia = 0;
    int ib = 0;
    ivec2 uvec_max = ivec2(size.x * size.y, 0);
    
    ivec2 a1 = texelFetch(grid, ivec2(unflatten(id - localId, size) - 0.5), 0).xy;
    ivec2 a2 = texelFetch(grid, ivec2(unflatten(id - localId + it2, size) - 0.5), 0).xy;
    
    while(ia + ib <= localId) {
        if((a1.x <= a2.x || ib >= it2) && ia < it2) {
            ans = a1;
            ++ia;
            a1 = texelFetch(grid, ivec2(unflatten(ia + id - localId, size) - 0.5), 0).xy;
        }else{
            ans = a2;
            ++ib;
            int l2 = ib + id - localId + it2;
            a2 = l2 < count ? texelFetch(grid, ivec2(unflatten(l2, size) - 0.5), 0).xy : uvec_max;
        }
    }
    
    f = ans;
}
