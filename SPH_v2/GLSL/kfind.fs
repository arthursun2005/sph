layout (location = 0) out ivec2 A;

uniform isampler2D grid;
uniform int block;
uniform ivec2 size;
uniform int count;

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy - 0.5);
    int id = flatten(gl_FragCoord.xy, size);
    int md = id%block;
    int s = texelFetch(grid, coord, 0).x;
    int position = 0;
    for(int i = 0; i < count; i += block) {
        int b = block >> 1;
        int j = i;
        int end = i + block;
        if(i == id - md) {
            //continue;
        }
        if(end > count) end = count;
        int e0 = texelFetch(grid, ivec2(unflatten(i, size) - 0.5), 0).x;
        int e1 = texelFetch(grid, ivec2(unflatten(end - 1, size) - 0.5), 0).x;
        if(s <= e0) continue;
        if(s >= e1) {position += block - 1; continue;}
        while(b > 0) {
            int i0 = texelFetch(grid, ivec2(unflatten(j, size) - 0.5), 0).x;
            int i1 = texelFetch(grid, ivec2(unflatten(j + 1, size) - 0.5), 0).x;
            if(s > i0 && s > i1) {
                j += b;
                b >>= 1;
            }else if(s < i0 && s < i1) {
                j -= b;
                b >>= 1;
            }else{
                break;
            }
        }
        position += j - (j%block);
    }
    A = ivec2(position, 0);
    //A = texelFetch(grid, ivec2(unflatten(position, size) - 0.5), 0).xy;
}
