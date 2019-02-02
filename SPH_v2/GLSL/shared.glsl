#version 410 core

#define PX 1300967

#define PY 179482213

#define PI 3.14159265359

#define C_1 (45.0 / PI)

#define GRID_SEARCH_R 4

uint hash(ivec2 X) {
    return (X.x * PX) ^ (X.y * PY);
}

int flatten(vec2 X, ivec2 size) {
    ivec2 f = size * ivec2(X);
    return f.x + f.y * size.x;
}

int flatten_texel_center(vec2 texel_center, ivec2 size) {
    ivec2 f = ivec2(texel_center - 0.5);
    return f.x + f.y * size.x;
}

vec2 unflatten(int X, ivec2 size) {
    return vec2((X%size.x) + 0.5, (X)/(size.x) + 0.5) / vec2(size);
}

vec2 unflatten(uint X, ivec2 size) {
    return vec2((X%size.x) + 0.5, (X)/(size.x) + 0.5) / vec2(size);
}
