layout (location = 0) out vec4 a;

uniform sampler2D T;
uniform float u;
uniform vec4 A;
uniform vec2 invSize;

void main() {
    if(u == 0.0) {
        a = A;
    }else{
        a = min(max(texture(T, gl_FragCoord.xy * invSize) * u + A, 0.0), 1.0);
    }
}
