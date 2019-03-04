uniform isampler2D grid;
uniform ivec2 size;

void main() {
    vec2 tx = unflatten(gl_VertexID/12, size);
    int which = gl_VertexID%12;
    int x = texelFetch(grid, ivec2(tx), 0).x;
    vec2 p = unflatten(x, size);
    /*
    vec2 table[12] = vec2[](
        vec2(p.x, p.y),
        vec2(p.x, p.y + 1.0),
        vec2(size.x - 0.5, p.y + 1.0),
        vec2(p.x, p.y),
        vec2(size.x - 0.5, p.y),
        vec2(size.x - 0.5, p.y + 1.0),
        vec2(0.5, p.y + 1.0),
        vec2(0.5, size.y - 0.5),
        vec2(size.x - 0.5, size.y - 0.5),
        vec2(0.5, p.y + 1.0),
        vec2(size.x - 0.5, p.y + 1.0),
        vec2(size.x - 0.5, size.y - 0.5)
    );
     */
    gl_Position = vec4(p.x * 2.0/vec2(size) - 1.0, 0.0, 1.0);
}
