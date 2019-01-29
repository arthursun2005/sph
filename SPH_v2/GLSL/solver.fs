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
uniform ivec2 size;
uniform vec2 gravity;

uniform sampler2D list;
uniform usampler2D grid;

vec2 plusOne(in vec2 coord) {
    coord.x += 1.0;
    if(coord.x > size.x) {
        coord.x = 0.5;
        coord.y += 1.0;
    }
    return coord;
}

void main() {
    vec2 coord = gl_FragCoord.xy * 1.0/vec2(size);
    vec2 pos0 = texture(uP, coord).xy;
    vec2 vel0 = texture(uV, coord).xy;
    
    float A = max(texture(uW, coord).x, p0);
    float Ap = (A - p0) * K;
    
    vec2 accel = vec2(0.0, 0.0);
    
    uint id = flatten_texel_center(gl_FragCoord.xy, size);
    
    ivec2 inx = ivec2(floor(pos0/h));
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            ivec2 i = inx + ivec2(x, y);
            uint hh = hash(i)%(size.x * size.y);
            vec2 offset = texture(list, unflatten(hh, size)).xy;
            while(flatten_texel_center(offset, size) < count) {
                vec2 offsetCoord = offset * 1.0/vec2(size);
                uvec2 inxB = texture(grid, offsetCoord).xy;
                
                if(inxB.x != hh) {
                    break;
                }
                
                if(inxB.y == id) {
                    offset = plusOne(offset);
                    continue;
                }
                
                vec2 joord = unflatten(inxB.y, size);
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
                    
                    accel -= ((Ap + Bp) / (2.0 * A * B)) * W * n * h / A * C_1;
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
