//
//  ParticleSystem.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "ParticleSystem.hpp"


/*
const char* sortVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

uniform vec2 invSize;

uniform int root;
uniform int stage;
uniform int count;
uniform int order;

uniform int stride;
uniform int it;

uniform usampler2D grid;

out vec2 c1;
out vec2 c2;

out float comp;

vec2 ii(in int index) {
    return vec2(float(index%root) + 0.5, floor(float(index)/float(root)) + 0.5) * invSize;
}

void main() {
    int i = gl_VertexID;
    
    int localId = i%it;
    
    bool up = (stride&i) != 0;
    
    c1 = ii(i);
    
    if(up) {
        c2 = ii(i - stride);
    }else{
        c2 = ii(i + stride);
    }
    
    bool big = localId < (it >> 1) ? up : !up;
    comp = big ? 1.0 : -1.0;
    
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

)";

const char* sortFShader = R"(

#version 410 core

layout (location = 0) out uvec2 f;

//uniform usampler2D grid;

in vec2 c1;
in vec2 c2;
in float comp;

void main() {
    //uvec2 p1 = texture(grid, c1).xy;
    //uvec2 p2 = texture(grid, c2).xy;
    //f = p2;
    f = uvec2(1, 2);
    //f = (!(p1.x < p2.x) ^^ (comp > 0.0)) ? p2 : p1;
}

)";
*/

/*
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
 */


/*
const char* sortVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

void main() {
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

)";

const char* sortFShader = R"(

#version 410 core

layout (location = 0) out uvec2 f;

uniform usampler2D grid;

uniform vec2 invSize;
uniform int root;
uniform int stage;
uniform int count;
uniform int order;

ivec2 ii(in int index) {
    return ivec2(index%root, floor(index/root));
}

vec2 ii(in int index) {
    return vec2(float(index%root) + 0.5, floor(float(index)/float(root)) + 0.5) * invSize;
}

void main() {
    ivec2 l = ivec2(gl_FragCoord.xy - 0.5);
    int i = l.x + l.y * root;
    
    int it = 1 << (stage + 1);
    int it2 = 1 << stage;
    int localId = i%it;
    
    int stride = 1 << (stage - order);
    bool up = (stride&i) != 0;
    bool big = localId < it2 ? up : !up;
    int _st = up ? -stride : stride;
    int i2 = i + _st;
    
    //uvec2 p1 = texelFetch(grid, ivec2(gl_FragCoord.xy - 0.5), 0).xy;
    //uvec2 p2 = texelFetch(grid, ii(i2), 0).xy;
    uvec2 p1 = texture(grid, gl_FragCoord.xy * invSize).xy;
    uvec2 p2 = texture(grid, ii(i2)).xy;
    //f = p1;
    f = (!(p1.x < p2.x) ^^ big) ? p2 : p1;
    //f = uvec2(i, 0);
}

)";
*/







/*

const char* sortVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

uniform int root;

uniform int stride;
uniform int size;
uniform int at;
uniform vec2 invSize;

out vec2 c1;
out vec2 c2;
out float comp;

uniform bool dir;

vec2 ii(in uint index) {
    return vec2((index%root) + 0.5, floor(index/root) + 0.5) * invSize;
}

void main() {
    int i = gl_VertexID;
    c1 = ii(i);
    if(i < size/2 + at) {
        c2 = ii(i + stride);
        comp = dir ? 1.0 : -1.0;
    }else{
        c2 = ii(i - stride);
        comp = dir ? -1.0 : 1.0;
    }
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

)";

const char* sortFShader = R"(

#version 410 core

layout (location = 0) out uvec2 f;

uniform usampler2D grid;

in vec2 c1;
in vec2 c2;
in float comp;

uvec2 d = uvec2(1, 0);

void main() {
    uvec2 p1 = texture(grid, c1).xy;
    uvec2 p2 = texture(grid, c2).xy;
    if(comp > 0.0) f = (p1.x <= p2.x) ? p1 : p2;
    else f = (p1.x >= p2.x) ? p1 : p2;
    d.x += 1;
    f = d;
}

)";

*/



void ParticleSystem::createStandardShaders() {
    setShader.init("GLSL/textureMapping.vs", "GLSL/set.fs", "GLSL/shared.glsl");
    setiShader.init("GLSL/textureMapping.vs", "GLSL/iset.fs", "GLSL/shared.glsl");
    rectShader.init("GLSL/textureMapping.vs", "GLSL/setRect.fs", "GLSL/shared.glsl");
    drawShader.init("GLSL/drawing.vs", "GLSL/drawing.fs", "GLSL/shared.glsl");
    solverShader.init("GLSL/textureMapping.vs", "GLSL/solver.fs", "GLSL/shared.glsl");
    stepShader.init("GLSL/textureMapping.vs", "GLSL/step.fs", "GLSL/shared.glsl");
    weightShader.init("GLSL/textureMapping.vs", "GLSL/weight.fs", "GLSL/shared.glsl");
    mapToGridShader.init("GLSL/textureMapping.vs", "GLSL/mapParticlesToGrid.fs", "GLSL/shared.glsl");
    
    boundShader.init("GLSL/textureMapping.vs", "GLSL/bound.fs", "GLSL/shared.glsl");
    sortShader.init("GLSL/textureMapping.vs", "GLSL/bitonicSort.fs", "GLSL/shared.glsl");
    
    gridToListShader.init("GLSL/gridToList.vs", "GLSL/gridToList.fs", "GLSL/shared.glsl");
}

void ParticleSystem::blitRoot(const Texture &target, int start, int count) {
    glBindFramebuffer(GL_FRAMEBUFFER, target.target.fbo);
    glBindVertexArray(particleVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, start, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleSystem::blitQuad(const Texture &target) {
    glBindFramebuffer(GL_FRAMEBUFFER, target.target.fbo);
    glBindVertexArray(baseVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, 0, 6);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleSystem::resetVelocities() {
    setShader.bind();
    setShader.uniform4f("a", 0.0f, 0.0f, 0.0f, 0.0f);
    
    velocities[0].bind();
    blitRoot(velocities[0], 0, max_count);
    
    velocities[1].bind();
    blitRoot(velocities[1], 0, max_count);
}

void ParticleSystem::addParticle(float x, float y) {
    assert(count < max_count);
    
    positions[1].bind();
    
    setShader.bind();
    setShader.uniform4f("a", x, y, 0.0f, 0.0f);
    
    blitRoot(positions[1], count, 1);
    
    ++count;
}

void ParticleSystem::addRect(float x, float y, uint32_t hx, uint32_t hy) {
    uint32_t c = hx * hy;
    assert(count + c <= max_count);
    positions[1].bind();
    
    rectShader.bind();
    rectShader.uniform2f("xy", x, y);
    rectShader.uniform1i("hx", hx);
    rectShader.uniform1i("hy", hy);
    rectShader.uniform1f("sp", h);
    rectShader.uniform1i("root", root);
    rectShader.uniform1i("count", count);
    
    blitRoot(positions[1], count, c);
    
    count += c;
}

void ParticleSystem::map() {
    float n0 = 0.0f;
    float n1 = 0.0f;
    float n2 = 0.0f;
    float n3 = 0.0f;
    
    /*
    setiShader.bind();
    
    setiShader.uniform4i("a", max_count, 0, 0, 0);
    
    grid[1].bind();
    blitRoot(grid[1], 0, count);
    
    
    /// mapping the keys
    /// will be sorted later using: merge sort
    
    grid[1].bind();
    
    mapToGridShader.bind();
    mapToGridShader.uniform1i("count", count);
    mapToGridShader.uniform1f("h", h);
    mapToGridShader.uniform1i("uP", positions[1].id);
    mapToGridShader.uniform1i("px", px);
    mapToGridShader.uniform1i("py", py);
    mapToGridShader.uniform2f("invSize", invSize);
    mapToGridShader.uniform1i("root", root);
    
    blitRoot(grid[1], 0, count);
    */
    
    
    
    int logs = ceilf(log2f((float)count));
    
    int nc = 1 << logs;
    
    n0 = glfwGetTime();
    
    sortShader.bind();
    sortShader.uniform2f("invSize", invSize);
    sortShader.uniform1i("root", root);
    //sortShader.uniform1i("count", count);
    /*
    for(int n = 1; n <= logs; ++n) {
        for(int k = 1; k <= n; ++k) {
            
            grid[0].bind();
            sortShader.uniform1i("grid", grid[1].id);
            //sortShader.uniform1i("order", k);
            //sortShader.uniform1i("stage", n);
            //sortShader.uniform1i("stride", 1 << (n - k));
            //sortShader.uniform1i("it", 1 << (n + 1));
            
            int t = logs - n + k - 1;
            int s = 1 << (n - k);
            int l = 1 << (n - k + 1);
            int h = 1 << t;
            int k2 = 1 << (k - 1);
            for(int u = 0; u < h; ++u) {
                sortShader.uniform1i("at", u * l);
                sortShader.uniform1i("size", l);
                sortShader.uniform1i("stride", s);
                sortShader.uniform1i("dir", (u%(1 << k)) < k2);
                
                //grid[0].bind();
                blitRoot(grid[0], u * l, l);
            }
     
             
            //blitRoot(grid[0], 0, nc);
            
            grid.swap();
        }
    }
   */
    bool debug = false;
    sortShader.bind();
    
    sortShader.uniform2f("invSize", invSize);
    sortShader.uniform1i("root", root);
    /*
    for(int k = 2; k <= nc; k <<= 1) {
        for(int j = k >> 1; j > 0; j >>= 1) {
            grid[0].bind();
            
            sortShader.uniform1i("grid", grid[1].id);
            
            sortShader.uniform1i("j", j);
            sortShader.uniform1i("k", k);
            
            //blitRoot(bitPositions, 0, nc);
            
            //compShader.bind();
            //compShader.uniform1i("grid", grid[1].id);
            
            //blitRoot(grid[0], 0, nc);
            blitQuad(grid[0]);
            
            grid.swap();
        }
    }
    */
    /*
    uint32_t tp[root * root * 2];
    
    grid[1].bind();
    
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, tp);
    
    int i = 0;
    for(int y = 0; y < root; ++y) {
        if(i >= count) break;
        for(int x = 0; x < root; ++x) {
            i = y * root + x;
            if(i >= count) break;
            int i2 = i * 2;
            //float f1 = tp[i2 + 0];
            //float f2 = tp[i2 + 1];
            //if(f1 > root || f2 > root) continue;
            printf("1(%u, %u) ", tp[i2 + 0], tp[i2 + 1]);
            printf("\t");
        }
        printf("\n");
    }
    
    */
    blitQuad(grid[0]);
    //blitRoot(grid[0], 0, count);
    
    grid.swap();
    
    n1 = glfwGetTime();
    
    //tp[root * root * 2];
    /*
    grid[1].bind();
    
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, tp);
    
    i = 0;
    for(int y = 0; y < root; ++y) {
        if(i >= count) break;
        for(int x = 0; x < root; ++x) {
            i = y * root + x;
            if(i >= count) break;
            int i2 = i * 2;
            //float f1 = tp[i2 + 0];
            //float f2 = tp[i2 + 1];
            //if(f1 > root || f2 > root) continue;
            printf("2(%u, %u) ", tp[i2 + 0], tp[i2 + 1]);
            printf("\t");
        }
        printf("\n");
    }
    */
    
    /// every cell bucket without a cell will have an index of MAX
    
    setShader.bind();
    setShader.uniform4f("a", root + 0.5f, root + 0.5f, 0, 0);
    
    offsetList.bind();
    blitRoot(offsetList, 0, max_count);
    
    
    
    gridToListShader.bind();
    gridToListShader.uniform1i("grid", grid[1].id);
    gridToListShader.uniform2f("invSize", invSize);
    gridToListShader.uniform1i("root", root);
    
    offsetList.bind();
    
    //blitRoot(offsetList, 0, count);
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, offsetList.target.fbo);
    glBindVertexArray(particleReverseVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, max_count - count, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    printf("%f %f %f\n", n1 - n0, n2 - n1, n3 - n2);
}

void ParticleSystem::solveOnce(float dt) {
    glm::vec2 bound(10.0f, 200.0f);
    
    /// summing up weights
    
    weights.bind();
    
    weightShader.bind();
    weightShader.uniform1i("count", count);
    weightShader.uniform1f("h", h);
    weightShader.uniform1i("uP", positions[1].id);
    weightShader.uniform2f("invSize", invSize);
    weightShader.uniform1i("root", root);
    weightShader.uniform1i("px", px);
    weightShader.uniform1i("py", py);
    weightShader.uniform1i("list", offsetList.id);
    weightShader.uniform1i("grid", grid[1].id);

    blitRoot(weights, 0, count);
    
    
    
    /// velocities' force solver
    
    velocities[0].bind();
    
    solverShader.bind();
    solverShader.uniform1f("spiky", spiky);
    solverShader.uniform1i("count", count);
    solverShader.uniform1f("h", h);
    solverShader.uniform1f("e", e);
    solverShader.uniform1i("uP", positions[1].id);
    solverShader.uniform1i("uW", weights.id);
    solverShader.uniform1i("root", root);
    solverShader.uniform1i("uV", velocities[1].id);
    solverShader.uniform2f("invSize", invSize);
    solverShader.uniform2f("bound", bound);
    solverShader.uniform2f("gravity", gravity);
    solverShader.uniform1f("p0", p0);
    solverShader.uniform1f("K", K);
    solverShader.uniform1f("dt", dt);
    solverShader.uniform1i("px", px);
    solverShader.uniform1i("py", py);
    solverShader.uniform1i("list", offsetList.id);
    solverShader.uniform1i("grid", grid[1].id);
    
    blitRoot(velocities[0], 0, count);
    
    velocities.swap();
    
    
    
    velocities[0].bind();
    
    boundShader.bind();
    boundShader.uniform1i("uP", positions[1].id);
    boundShader.uniform1i("uV", velocities[1].id);
    boundShader.uniform2f("invSize", invSize);
    boundShader.uniform2f("bound", bound);
    
    blitRoot(velocities[0], 0, count);
    
    velocities.swap();
    
    
    
    
    /// p = p0 + v * dt
    
    positions[0].bind();
    
    stepShader.bind();
    stepShader.uniform1f("dt", dt);
    stepShader.uniform1i("uV", velocities[1].id);
    stepShader.uniform1i("uP", positions[1].id);
    stepShader.uniform2f("bound", bound);
    stepShader.uniform2f("invSize", invSize);
    
    blitRoot(positions[0], 0, count);
    
    positions.swap();
}

void ParticleSystem::solve(float dt) {
    const float r = 0.001f;
    int its = ceilf(sqrtf(gravity.length()/(r * h)) * dt);
    float _dt = dt / (float)its;
    for(int i = 0; i < its; ++i) {
        solveOnce(_dt);
    }
}

void ParticleSystem::solve(float dt, int its) {
    float _dt = dt / (float)its;
    //map();
    for(int i = 0; i < its; ++i) {
        map();
        solveOnce(_dt);
    }
}

void ParticleSystem::render(GLuint target, const glm::vec2& size, const glm::vec2& scl) {
    drawShader.bind();
    drawShader.uniform1i("T", positions[1].id);
    drawShader.uniform1i("uV", velocities[1].id);
    drawShader.uniform2f("S", scl);
    drawShader.uniform2f("scl2", radius * scl / size);
    drawShader.uniform2f("scl1", scl/size);
    
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    glBindVertexArray(drawingVAO);
    glViewport(0, 0, size.x, size.y);
    glDrawArraysInstanced(GL_LINE_LOOP, 0, particleShapeVerticesNum, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
