layout (location = 0) out float weight;

uniform sampler2D uP;
uniform int count;
uniform float h;
uniform int root;
uniform vec2 invSize;

uniform sampler2D list;
uniform usampler2D grid;

vec2 indexToCoord(in uint index) {
    return vec2(float(index%root) + 0.5, floor(float(index)/float(root)) + 0.5) * invSize;
}

uint ii(in vec2 p) {
    uvec2 k = uvec2(p - 0.5);
    return k.x + k.y * root;
}

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 pos0 = texture(uP, coord).xy;
    
    float w = 0.0;
    
    ivec2 inx = ivec2(floor(pos0/h));
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            ivec2 i = inx + ivec2(x, y);
            uint hh = uint((i.x * PX) ^ (i.y * PY))%(root * root);
            vec2 offset = texture(list, indexToCoord(hh)).xy;
            while(ii(offset) < count) {
                vec2 offsetCoord = offset * invSize;
                uvec2 inxB = texture(grid, offsetCoord).xy;
                
                if(inxB.x != hh) {
                    break;
                }
                
                vec2 pos1 = texture(uP, indexToCoord(inxB.y)).xy;
                
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                if(r2 < h * h) {
                    w += pow(1.0 - r2 / (h * h), 3.0);
                }
                
                offset.x += 1.0;
                if(offset.x > root) {
                    offset.x = 0.5;
                    offset.y += 1.0;
                }
            }
        }
    }
    
    weight = w;
}
