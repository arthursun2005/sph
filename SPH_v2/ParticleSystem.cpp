//
//  ParticleSystem.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "ParticleSystem.hpp"

const char* texMappingVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

void main() {
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

)";

const char* drawingVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

layout (location = 1) in vec2 pos;

uniform sampler2D T;
uniform vec2 scl2;
uniform vec2 scl1;

out vec2 coord;

void main() {
    coord = uv;
    gl_Position = vec4(texture(T, uv).xy * scl1 + pos * scl2, 0.0, 1.0);
}

)";

const char* drawingFShader = R"(

#version 410 core

layout (location = 0) out vec4 A;

uniform sampler2D uV;

in vec2 coord;

void main() {
    vec2 v = texture(uV, coord).xy;
    float m = dot(v, v);
    A = vec4(0.5, 0.5, 1.0, 1.0) + 0.1 * vec4(m, m, 0.0, 0.0);
}

)";

const char* setiFShader = R"(

#version 410 core

layout (location = 0) out ivec4 A;

uniform ivec4 a;

void main() {
    A = a;
}

)";

const char* setFShader = R"(

#version 410 core

layout (location = 0) out vec4 A;

uniform vec4 a;

void main() {
    A = a;
}

)";

const char* setRectFShader = R"(

#version 410 core

layout (location = 0) out vec2 A;

uniform vec2 xy;
uniform int hx;
uniform int hy;
uniform float sp;
uniform int root;
uniform int count;

void main() {
    ivec2 ul = ivec2(gl_FragCoord.xy - 0.5);
    int on = ul.x + root * ul.y - count;
    int h = int(floor(on/hx));
    int dx = on%hx - hx / 2;
    int dy = h - hy / 2;
    A = xy + vec2(dx * sp, dy * sp);
}

)";

const char* weightFShader = R"(

#version 410 core

layout (location = 0) out float weight;

uniform sampler2D uP;
uniform int count;
uniform float h;
uniform int root;
uniform vec2 invSize;
uniform vec2 bound;

uniform int px;
uniform int py;

uniform sampler2D list;
uniform sampler2D grid;

vec2 indexToCoord(in int index) {
    return vec2(float(index%root) + 0.5, floor(index/root) + 0.5) * invSize;
}

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 pos0 = texture(uP, coord).xy;
    
    float w = 0.0;
    /*
    int ty = 0;
    
    while(ty < root) {
        int tx = 0;
        while(tx < root) {
            if(tx + ty * root >= count) break;
            vec2 joord = vec2(tx + 0.5, ty + 0.5) * invSize;
            vec2 pos1 = texture(uP, joord).xy;
            vec2 dp = pos1 - pos0;
            float r2 = dot(dp, dp);
            if(r2 < h * h) {
                w += pow(1.0 - r2 / (h * h), 3.0);
            }
            ++tx;
        }
        ++ty;
    }
     */
    
    ivec2 inx = ivec2(floor(pos0/h));
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            ivec2 i = inx + ivec2(x, y);
            int hh = int(i.x * px + i.y * py)%(root * root);
            vec2 offset = texture(list, indexToCoord(hh)).xy;
            while(offset.y < root) {
                vec2 offsetCoord = offset * invSize;
                ivec2 inxB = ivec2(texture(grid, offsetCoord).xy);
                
                if(inxB.x != hh) {
                    break;
                }
                
                vec2 pos1 = texture(uP, indexToCoord(inxB.y)).xy;
                
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                if(r2 < h * h) {
                    w += pow(1.0 - r2 / (h * h), 3.0);
                }
                
                
                offset.x += 1.0;
                if(offset.x > root) {
                    offset.x = 0.5;
                    offset.y += 1.0;
                }
            }
        }
    }
    
    /*
    if(pos0.x < 0.0) {
        float r = -pos0.x;
        w += poly6 * pow(h * h - r * r, 3.0);
    }
    if(pos0.x > bound.x) {
        float r = pos0.x - bound.x;
        w += poly6 * pow(h * h - r * r, 3.0);
    }
    
    if(pos0.y < 0.0) {
        float r = -pos0.y;
        w += poly6 * pow(h * h - r * r, 3.0);
    }
    if(pos0.y > bound.y) {
        float r = pos0.y - bound.y;
        w += poly6 * pow(h * h - r * r, 3.0);
    }
    */
    weight = w;
}

)";

const char* solverFShader = R"(

#version 410 core

layout (location = 0) out vec2 vel;

uniform sampler2D uP;
uniform sampler2D uW;
uniform sampler2D uV;
uniform int count;
uniform float spiky;
uniform float h;
uniform float e;
uniform float dt;
uniform float p0;
uniform float P;
uniform float K;
uniform int root;
uniform vec2 bound;
uniform vec2 invSize;
uniform vec2 gravity;

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 pos0 = texture(uP, coord).xy;
    vec2 vel0 = texture(uV, coord).xy;
    
    float A = max(texture(uW, coord).x, p0);
    float Ap = (A - p0) * K;
    
    int ty = 0;
    
    vec2 accel = vec2(0.0, 0.0);
    
    while(ty < root) {
        int tx = 0;
        while(tx < root) {
            if(tx + ty * root >= count) break;
            if(gl_FragCoord.x != (tx + 0.5) || gl_FragCoord.y != (ty + 0.5)) {
                vec2 joord = vec2(tx + 0.5, ty + 0.5) * invSize;
                vec2 pos1 = texture(uP, joord).xy;
                vec2 dp = pos1 - pos0;
                float r2 = dot(dp, dp);
                float h2 = h * h;
                if(r2 < h2) {
                    float B = max(texture(uW, joord).x, p0);
                    float Bp = (B - p0) * K;
                    
                    float r = sqrt(r2);
                    vec2 n = normalize(dp);
                    float W = spiky * pow(1.0 - r / h, 2.0);
                    
                    vec2 vd = texture(uV, joord).xy - vel0;
                    
                    accel += ((Ap + Bp) / (2.0 * A * B)) * W * n * h / A;
                    accel += (1.0/B) * e * vd / A;
                }
            }
            ++tx;
        }
        ++ty;
    }
    
    accel += gravity;
    
    vel0 += accel * dt;
    
    pos0 += vel0 * dt;
    
    /*
    float v = -spiky * Ap/A * dt;
    
    if(pos0.x < 0.0) {
        if(vel0.x < 0.0) vel0.x += v * pow(pos0.x, 2.0);
    }
    if(pos0.x > bound.x) {
        if(vel0.x > 0.0) vel0.x -= v * pow(bound.x - pos0.x, 2.0);
    }
    
    if(pos0.y < 0.0) {
        if(vel0.y < 0.0) vel0.y += v * pow(pos0.y, 2.0);
    }
    if(pos0.y > bound.y) {
        if(vel0.y > 0.0) vel0.y -= v * pow(bound.y - pos0.y, 2.0);
    }
    */
    
    float dmp = -0.5;
    
    if(pos0.x < 0.0) {
        if(vel0.x < 0.0) vel0.x *= dmp;
    }
    if(pos0.x > bound.x) {
        if(vel0.x > 0.0) vel0.x *= dmp;
    }
    
    if(pos0.y < 0.0) {
        if(vel0.y < 0.0) vel0.y *= dmp;
    }
    if(pos0.y > bound.y) {
        if(vel0.y > 0.0) vel0.y *= dmp;
    }
    
    
    vel = vel0;
}

)";

const char* stepFShader = R"(

#version 410 core

layout (location = 0) out vec2 pos;

uniform sampler2D uV;
uniform sampler2D uP;
uniform vec2 bound;
uniform vec2 invSize;
uniform float dt;

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    vec2 vel0 = texture(uV, coord).xy;
    vec2 pos0 = texture(uP, coord).xy;
    
    pos0 += vel0 * dt;
    
    if(pos0.x < 0.0) pos0.x = 0.0;
    if(pos0.x > bound.x) pos0.x = bound.x;
    
    if(pos0.y < 0.0) pos0.y = 0.0;
    if(pos0.y > bound.y) pos0.y = bound.y;
    
    pos = pos0;
}

)";

const char* mapToGridFShader = R"(

#version 410 core

layout (location = 0) out ivec2 f;

uniform sampler2D uP;
uniform vec2 invSize;
uniform float h;
uniform int root;

uniform int px;
uniform int py;

int hash(in vec2 uv) {
    vec2 p = texture(uP, uv).xy;
    return (int(floor(p.x/h)) * px + int(floor(p.y/h)) * py)%(root * root);
}

void main() {
    vec2 coord = gl_FragCoord.xy * invSize;
    ivec2 u = ivec2(gl_FragCoord.xy - 0.5);
    f = ivec2(hash(coord), u.x + u.y * root);
}

)";

const char* sortFShader = R"(

#version 410 core

layout (location = 0) out ivec2 f;

uniform sampler2D grid;
uniform int root;
uniform int order;
uniform int count;
uniform vec2 invSize;

ivec2 ii(in int index) {
    vec2 coord = vec2(float(index%root) + 0.5, floor(index/root) + 0.5) * invSize;
    return ivec2(texture(grid, coord).xy);
}

void main() {
    ivec2 h = ivec2(gl_FragCoord.xy - 0.5);
    int id = h.x + h.y * root;
    int it = int(pow(2, order));
    int it2 = it/2;
    int localId = id%it;
    ivec2 ans = ivec2(0, 0);
    int ia = 0;
    int ib = 0;
    while(ia + ib - 1 < localId) {
        ivec2 a1 = ii(ia + id - localId);
        ivec2 a2 = ii(ib + id - localId + it2);
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

)";



const char* gridToListVShader = R"(

#version 410 core

layout (location = 0) in vec2 uv;

uniform sampler2D grid;
uniform int root;
uniform vec2 invSize;

out vec2 A;

void main() {
    int k = int(texture(grid, uv).x);
    vec2 c = vec2(float(k%root) + 0.5, floor(k/root) + 0.5) * invSize;
    A = uv * root;
    gl_Position = vec4(c * 2.0 - 1.0, 0.0, 1.0);
}


)";



const char* gridToListFShader = R"(

#version 410 core

layout (location = 0) out vec2 a;

in vec2 A;

void main() {
    a = A;
}

)";


const char* debugFShader = R"(
)";


void ParticleSystem::createStandardShaders() {
    setShader.init(&texMappingVShader, &setFShader);
    setiShader.init(&texMappingVShader, &setiFShader);
    rectShader.init(&texMappingVShader, &setRectFShader);
    drawShader.init(&drawingVShader, &drawingFShader);
    solverShader.init(&texMappingVShader, &solverFShader);
    stepShader.init(&texMappingVShader, &stepFShader);
    weightShader.init(&texMappingVShader, &weightFShader);
    mapToGridShader.init(&texMappingVShader, &mapToGridFShader);
    
    debugShader.init(&texMappingVShader, &debugFShader);
    sortShader.init(&texMappingVShader, &sortFShader);
    
    gridToListShader.init(&gridToListVShader, &gridToListFShader);
}

void ParticleSystem::blitRoot(const Texture &target, int start, int count) {
    glBindFramebuffer(GL_FRAMEBUFFER, target.target.fbo);
    glBindVertexArray(particleVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, start, count);
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

void ParticleSystem::solveOnce(float dt) {
    glm::vec2 bound(2.0f, 200.0f);
    
    setiShader.bind();
    setiShader.uniform4i("a", max_count, 0, 0, 0);
    
    grid[1].bind();
    blitRoot(grid[1], 0, max_count);
    
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
    
    
    
    int logs = ceilf(log2f((float)count));
    
    
    sortShader.bind();
    sortShader.uniform2f("invSize", invSize);
    sortShader.uniform1i("root", root);
    
    int nc = 1 << logs;
    
    for(int n = 1; n <= logs; ++n) {
        grid[0].bind();
        
        sortShader.uniform1i("grid", grid[1].id);
        sortShader.uniform1i("order", n);
        
        blitRoot(grid[0], 0, nc);
        
        grid.swap();
    }
    
    
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
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, offsetList.target.fbo);
    glBindVertexArray(particleReverseVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, max_count - count, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    
    /// summing up weights
    
    weights.bind();
    
    weightShader.bind();
    weightShader.uniform1i("count", count);
    weightShader.uniform1f("h", h);
    weightShader.uniform1i("uP", positions[1].id);
    weightShader.uniform2f("invSize", invSize);
    weightShader.uniform2f("bound", bound);
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
