layout (location = 0) out vec2 vel;

uniform sampler2D uP;
uniform sampler2D uW;
uniform sampler2D uV;
uniform int count;
uniform float h;
uniform float e;
uniform float dt;
uniform float p0;
uniform float K;
uniform int root;
uniform vec2 invSize;
uniform vec2 gravity;

uniform sampler2D list;
uniform usampler2D grid;

vec2 indexToCoord(in uint index) {
    return vec2(float(index%root) + 0.5, floor(float(index)/float(root)) + 0.5) * invSize;
}

vec2 plusOne(in vec2 coord) {
    coord.x += 1.0;
    if(coord.x > root) {
        coord.x = 0.5;
        coord.y += 1.0;
    }
    return coord;
}

uint ii(in vec2 p) {
    uvec2 k = uvec2(p - 0.5);
    return k.x + k.y * root;
}

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 pos0 = texture(uP, coord).xy;
    vec2 vel0 = texture(uV, coord).xy;
    
    float A = max(texture(uW, coord).x, p0);
    float Ap = (A - p0) * K;
    
    vec2 accel = vec2(0.0, 0.0);
    
    uvec2 id2 = uvec2(gl_FragCoord.xy - 0.5);
    uint id = ii(id2);
    
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
                
                if(inxB.y == id) {
                    offset = plusOne(offset);
                    continue;
                }
                
                vec2 joord = indexToCoord(inxB.y);
                vec2 pos1 = texture(uP, joord).xy;
                
                
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                float h2 = h * h;
                if(r2 < h2) {
                    float B = max(texture(uW, joord).x, p0);
                    float Bp = (B - p0) * K;
                    
                    float r = sqrt(r2);
                    vec2 n = normalize(dp);
                    float W = pow(1.0 - r / h, 2.0);
                    
                    vec2 vd = texture(uV, joord).xy - vel0;
                    
                    accel -= ((Ap + Bp) / (2.0 * A * B)) * W * n * h / A * 14.3239448783;
                    accel += (1.0/B) * e * vd / A;
                }
                
                
                offset = plusOne(offset);
            }
        }
    }
    
    accel += gravity;
    
    vel0 += accel * dt;
    
    pos0 += vel0 * dt;
    
    vel = vel0;
}
