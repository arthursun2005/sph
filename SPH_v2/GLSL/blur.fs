layout (location = 0) out vec4 A;

uniform sampler2D T;

uniform int radius;

void main() {
    ivec2 c = ivec2(gl_FragCoord.xy - 0.5);
    
    vec4 a = vec4(0.0, 0.0, 0.0, 0.0);
    
    for(int x = -radius; x <= radius; ++x) {
        for(int y = -radius; y <= radius; ++y) {
            a += texelFetch(T, c - ivec2(x, y), 0);
        }
    }
    
    A = a/(4 * radius * radius + 4 * radius + 1);
}

