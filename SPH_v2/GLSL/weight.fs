layout (location = 0) out float weight;

uniform sampler2D uP;
uniform int count;
uniform float h;
uniform ivec2 size;

uniform sampler2D list;
uniform isampler2D grid;

vec2 plusOne(in vec2 coord) {
    coord.x += 1.0;
    if(coord.x > size.x) {
        coord.x = 0.5;
        coord.y += 1.0;
    }
    return coord;
}

void main() {
    int id = flatten(gl_FragCoord.xy, size);
    
    if(id >= count) discard;
    
    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec2 pos0 = texelFetch(uP, coord, 0).xy;

    float w = 0.0;

    ivec2 inx = ivec2(floor(pos0/h));
    for(int x = -GRID_SEARCH_R; x <= GRID_SEARCH_R; ++x) {
        for(int y = -GRID_SEARCH_R; y <= GRID_SEARCH_R; ++y) {
            ivec2 i = inx + ivec2(x, y);
            int hh = hash(i, size.x * size.y);
            vec2 offset = texelFetch(list, ivec2(unflatten(hh, size)), 0).xy;
            while(flatten(offset, size) < count) {
                ivec2 inxB = texelFetch(grid, ivec2(offset), 0).xy;
                
                if(inxB.x != hh) {
                    break;
                }
                
                if(inxB.y == id) {
                    offset = plusOne(offset);
                    continue;
                }
                
                vec2 pos1 = texelFetch(uP, ivec2(unflatten(inxB.y, size)), 0).xy;
                
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                if(r2 < h * h) {
                    w += pow(1.0 - r2 / (h * h), 3.0);
                }
                
                offset = plusOne(offset);
            }
        }
    }
    
    weight = w;
}
