void main() {

    uvec2 l = uvec2(gl_FragCoord.xy - 0.5);
    uint id = l.x + l.y * root;

    uint it = 1 << stage;
    uint it2 = 1 << (stage - 1);
    uint localId = id%it;

    uvec2 ans = uvec2(0, 0);
    uint ia = 0;
    uint ib = 0;
    uvec2 uvec_max = uvec2(root * root, 0);
    while(ia + ib <= localId) {
        uint l2 = ib + id - localId + it2;
        uvec2 a1 = texture(grid, ii(ia + id - localId)).xy;
        uvec2 a2 = l2 < count ? texture(grid, ii(l2)).xy : uvec_max;
        if((a1.x <= a2.x || ib >= it2) && ia < it2) {
            ans = a1;
            ++ia;
        }else{
            ans = a2;
            ++ib;
        }
    }
    f = ans;
    
}
