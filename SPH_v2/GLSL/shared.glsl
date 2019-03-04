#version 410 core

#define PX 11777

#define PY 7121

#define M_PI 3.14159265358979

#define C_1 (45.0 / M_PI)

#define GRID_SEARCH_R 2

int hash(ivec2 X, int M) {
    return ((X.x * PX) ^ (X.y * PY))%M;
}

int flatten(vec2 texel_center, ivec2 size) {
    ivec2 f = ivec2(texel_center);
    return f.x + f.y * size.x;
}

vec2 unflatten(int X, ivec2 size) {
    return vec2(float(X%size.x) + 0.5, float(X/size.x) + 0.5);
}




