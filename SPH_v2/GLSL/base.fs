layout (location = 0) out vec4 a;

uniform sampler2D T;
uniform float u;
uniform vec4 A;
uniform ivec2 size;

void main() {
    if(u == 0.0) {
        a = A;
    }else{
        a = min(max(texture(T, gl_FragCoord.xy * 1.0/vec2(size)) * u + A, 0.0), 1.0);
    }
}
