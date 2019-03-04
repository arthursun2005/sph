layout (location = 0) out ivec2 f;

in vec2 frag;

void main() {
    f = ivec2(frag);
}

