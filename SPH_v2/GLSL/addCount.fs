layout(location = 0) out float k;

uniform sampler2D grid;
uniform ivec2 size;

void main() {
    vec2 k = vec2(gl_FragCoord.x - 1.0, gl_FragCoord.y);
    if(k.x < 0.0) {
        k.x = size.x - 0.5;
        k.y -= 1.0;
    }
    k = texelFetch(grid, ivec2(k), 0).x;
}

