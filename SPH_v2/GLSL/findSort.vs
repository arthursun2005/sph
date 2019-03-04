layout (location = 0) in vec2 uv;

out vec2 frag;

uniform int count;
uniform ivec2 size;
uniform int block;
uniform isampler2D grid;

void main() {
    int id = flatten(uv * vec2(size), size);
    int local = id - (id%block);
    int position = local;
    ivec2 self = texelFetch(grid, ivec2(uv * vec2(size)), 0).xy;
    for(int i = local; i < (local + block); ++i) {
        if(i == id) continue;
        if(i >= count) break;
        ivec2 x = texelFetch(grid, ivec2(unflatten(i, size)), 0).xy;
        if(self.x > x.x) ++position;
        if(self.x == x.x && id > i) ++position;
    }
    frag = vec2(self);
    vec2 P = unflatten(position, size) * 1.0/vec2(size);
    
    gl_Position = vec4(P * 2.0 - 1.0, 0.0, 1.0);
}


