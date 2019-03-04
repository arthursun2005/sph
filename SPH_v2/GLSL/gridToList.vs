layout (location = 0) in vec2 uv;

uniform isampler2D grid;
uniform ivec2 size;
uniform int count;

out vec2 A;

void main() {
    int id = flatten(uv * vec2(size), size);
    if(id >= count) {
        A = vec2(-1.0, 0.0);
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    }else{
        ivec2 cd = ivec2(uv * vec2(size) - 0.5);
        int k = texelFetch(grid, cd, 0).x;
        vec2 c = unflatten(k, size) * 1.0/vec2(size);
        A = uv * vec2(size);
        if(id > 0) {
            int y = texelFetch(grid, ivec2(unflatten(id - 1, size) - 0.5), 0).x;
            if(y >= k) A.x = -1.0;
        }
        gl_Position = vec4(c * 2.0 - 1.0, 0.0, 1.0);
    }
}
