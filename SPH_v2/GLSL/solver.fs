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
uniform vec2 bound;

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

vec2 arg(float a) {
    return vec2(cos(a), sin(a));
}

void main() {
    int id = flatten(gl_FragCoord.xy, size);
    
    if(id >= count) discard;
    
    ivec2 coord = ivec2(gl_FragCoord.xy);
    
    vec2 pos0 = texelFetch(uP, coord, 0).xy;
    vec2 vel0 = texelFetch(uV, coord, 0).xy;
    
    float A = max(texelFetch(uW, coord, 0).x, p0);
    float Ap = (A - p0) * K;
    
    vec2 accel = vec2(0.0, 0.0);
    
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
                
                ivec2 joord = ivec2(unflatten(inxB.y, size));
                vec2 pos1 = texelFetch(uP, joord, 0).xy;
                
                vec2 dp = pos1 - pos0;
                
                float es = h * 0.00001;
                if(dp.x < es && dp.x > -es && dp.y < es && dp.y > -es) {
                    if(id < inxB.y) {
                        offset = plusOne(offset);
                        continue;
                    }
                    vel0 += arg(id) * h;
                    offset = plusOne(offset);
                    continue;
                }
                
                float r2 = dot(dp, dp);
                float h2 = h * h;
                if(r2 < h2) {
                    float B = max(texelFetch(uW, joord, 0).x, p0);
                    float Bp = (B - p0) * K;
                    
                    float r = sqrt(r2);
                    vec2 n = normalize(dp);
                    float W = pow(1.0 - r / h, 2.0);
                    
                    vec2 vd = texelFetch(uV, joord, 0).xy - vel0;
                    
                    accel -= ((Ap + Bp) / (2.0 * A * B)) * W * n * h / A * C_1;
                    accel += (1.0/B) * e * vd / A * C_1;
                }
                
                
                offset = plusOne(offset);
            }
        }
    }
    
    accel += gravity;
    
    vel0 += accel * dt;
    
    pos0 += vel0 * dt;
    
    float dmp = -0.333;
    
    if(pos0.x < 0.0) {if(vel0.x < 0.0) {vel0.x *= dmp;} vel0.x += 0.5;}
    if(pos0.x > bound.x) {if(vel0.x > 0.0) {vel0.x *= dmp;} vel0.x -= 0.5;}
    
    if(pos0.y < 0.0) {if(vel0.y < 0.0) {vel0.y *= dmp;} vel0.y += 0.5;}
    if(pos0.y > bound.y) {if(vel0.y > 0.0) {vel0.y *= dmp;} vel0.y -= 0.5;}
    
    vel = vel0;
}
