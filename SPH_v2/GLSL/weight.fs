layout (location = 0) out float weight;

uniform sampler2D uP;
uniform int count;
uniform float h;
uniform ivec2 size;

uniform sampler2D list;
uniform usampler2D grid;

void main() {
    uint id = flatten_texel_center(gl_FragCoord.xy, size);
    
    if(id >= count) discard;
    
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    vec2 pos0 = texture(uP, coord).xy;
    
    float w = 0.0;
    
    ivec2 inx = ivec2(floor(pos0/h));
    for(int x = -GRID_SEARCH_R; x <= GRID_SEARCH_R; ++x) {
        for(int y = -GRID_SEARCH_R; y <= GRID_SEARCH_R; ++y) {
            ivec2 i = inx + ivec2(x, y);
            uint hh = hash(i)%(size.x * size.y);
            vec2 offset = texture(list, unflatten(hh, size)).xy;
            while(flatten_texel_center(offset, size) < count) {
                vec2 offsetCoord = offset * 1.0/vec2(size);
                uvec2 inxB = texture(grid, offsetCoord).xy;
                
                if(inxB.x != hh) {
                    break;
                }
                
                vec2 pos1 = texture(uP, unflatten(inxB.y, size)).xy;
                
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                if(r2 < h * h) {
                    w += pow(1.0 - r2 / (h * h), 3.0);
                }
                
                offset.x += 1.0;
                if(offset.x > size.x) {
                    offset.x = 0.5;
                    offset.y += 1.0;
                }
            }
        }
    }
    
    weight = w;
}
