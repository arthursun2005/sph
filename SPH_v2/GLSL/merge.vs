layout (location = 0) in vec2 uv;

uniform int stage;

uniform ivec2 size;

uniform int count;

uniform isampler2D grid;

out vec2 frag;

void main() {
    int id = gl_VertexID;
    
    int it = 1 << stage;
    int it2 = 1 << (stage - 1);
    int local = id%it;
    
    ivec2 self = texelFetch(grid, ivec2(unflatten(id, size) - 0.5), 0).xy;
    
    int io = 0, is = 0;
    
    if(local < it2) {
        is = local;
        if(id - local + it2 < count) {
            ivec2 o = texelFetch(grid, ivec2(unflatten(io + id - local + it2, size) - 0.5), 0).xy;
            while(o.x < self.x || (o.x == self.x && is < io)) {
                ++io;
                if(io >= it2) break;
                if(io + id - local + it2 >= count) break;
                o = texelFetch(grid, ivec2(unflatten(io + id - local + it2, size) - 0.5), 0).xy;
            }
        }
    }else{
        is = local - it2;
        ivec2 o = texelFetch(grid, ivec2(unflatten(io + id - local, size) - 0.5), 0).xy;
        while(o.x < self.x || (o.x == self.x && is <= io)) {
            ++io;
            if(io >= it2) break;
            o = texelFetch(grid, ivec2(unflatten(io + id - local, size) - 0.5), 0).xy;
        }
    }
    
    frag = vec2(self);
    vec2 P = unflatten(id + io + is - local, size) * 1.0/vec2(size);
    gl_Position = vec4(P * 2.0 - 1.0, 0.0, 1.0);
}


