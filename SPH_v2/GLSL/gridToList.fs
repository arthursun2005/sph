layout (location = 0) out vec2 a;

in vec2 A;

void main() {
    if(A.x < 0.0) discard;
    a = A;
}
