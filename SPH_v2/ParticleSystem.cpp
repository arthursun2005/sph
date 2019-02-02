//
//  ParticleSystem.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "ParticleSystem.hpp"

void ParticleSystem::createStandardShaders() {
    setShader.init("GLSL/textureMapping.vs", "GLSL/set.fs", "GLSL/shared.glsl");
    isetShader.init("GLSL/textureMapping.vs", "GLSL/iset.fs", "GLSL/shared.glsl");
    usetShader.init("GLSL/textureMapping.vs", "GLSL/uset.fs", "GLSL/shared.glsl");
    rectShader.init("GLSL/textureMapping.vs", "GLSL/setRect.fs", "GLSL/shared.glsl");
    drawShader.init("GLSL/drawing.vs", "GLSL/drawing.fs", "GLSL/shared.glsl");
    solverShader.init("GLSL/textureMapping.vs", "GLSL/solver.fs", "GLSL/shared.glsl");
    stepShader.init("GLSL/textureMapping.vs", "GLSL/step.fs", "GLSL/shared.glsl");
    weightShader.init("GLSL/textureMapping.vs", "GLSL/weight.fs", "GLSL/shared.glsl");
    mapToGridShader.init("GLSL/textureMapping.vs", "GLSL/mapParticlesToGrid.fs", "GLSL/shared.glsl");
    
    boundShader.init("GLSL/textureMapping.vs", "GLSL/bound.fs", "GLSL/shared.glsl");
    sortShader.init("GLSL/bitonicSort.vs", "GLSL/bitonicSort.fs", "GLSL/shared.glsl");
    
    gridToListShader.init("GLSL/gridToList.vs", "GLSL/gridToList.fs", "GLSL/shared.glsl");
}

void ParticleSystem::blitRoot(const Texture &target, int start, int count) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.target.fbo);
    glBindVertexArray(particleVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, start, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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
    rectShader.uniform2i("size", size);
    rectShader.uniform1i("count", count);
    
    blitRoot(positions[1], count, c);
    
    count += c;
}

void ParticleSystem::map() {
    float n0 = 0.0f;
    float n1 = 0.0f;
    float n2 = 0.0f;
    float n3 = 0.0f;
    
    //if(sorting_k == 2) {
    
    usetShader.bind();
    
    usetShader.uniform4ui("a", max_count, 0, 0, 0);
    
    grid[1].bind();
    blitRoot(grid[1], 0, count);
    
    
    /// mapping the keys
    /// will be sorted later using: merge sort
    
    grid[1].bind();
    
    mapToGridShader.bind();
    mapToGridShader.uniform1i("count", count);
    mapToGridShader.uniform1f("h", h);
    mapToGridShader.uniform1i("uP", positions[1].id);
    mapToGridShader.uniform2i("size", size);
    
    blitRoot(grid[1], 0, count);
    
    //}
    
    
    n0 = glfwGetTime();
    
    sortShader.bind();
    sortShader.uniform2i("size", size);
    
    /*
    for(int y = 0; y < root; ++y) {
        for(int n = 1; n <= rootLog2; ++n) {
            for(int k = 1; k <= n; ++k) {
                
                grid[0].bind();
                sortShader.uniform1i("grid", grid[1].id);
                //sortShader.uniform1i("order", k);
                //sortShader.uniform1i("stage", n);
                //sortShader.uniform1i("stride", 1 << (n - k));
                //sortShader.uniform1i("it", 1 << (n + 1));
                
                int t = rootLog2 - n + k - 1;
                int s = 1 << (n - k);
                int l = 1 << (n - k + 1);
                int h = 1 << t;
                int k2 = 1 << (k - 1);
                for(int u = 0; u < h; ++u) {
                    sortShader.uniform2f("start", (u * l) * invRoot, y * invRoot);
                    sortShader.uniform1f("stride", s * invRoot);
                    sortShader.uniform1i("dir", (u%(1 << k)) < k2);
                    
                    blit(grid[0].target.fbo, u * l, y, l, 1);
                    //grid[0].bind();
                    //blitRoot(grid[0], u * l, l);
                }
                
                
                //blitRoot(grid[0], 0, nc);
                
                grid.swap();
            }
        }
    }
    */
    
    int nc = nextPowerOfTwo(count);
    
    sortShader.uniform1i("count2", nc);
    
    
    
    
    for(int k = 2; k <= nc; k <<= 1) {
        for(int j = k >> 1; j > 0; j >>= 1) {
            
            grid[0].bind();
            
            sortShader.uniform1i("grid", grid[1].id);
            
            sortShader.uniform1i("j", j);
            sortShader.uniform1i("k", k);
            
            blit(grid[0].target.fbo, root, root);
            //blitRoot(grid[0], 0, nc);
            
            grid.swap();
            
        }
    }
    
    
    /*
     
     
    int _i = 0;
    
    
    
    for(; sorting_k <= nc; sorting_k <<= 1) {
        
        if(sorting_j == 0) sorting_j = sorting_k >> 1;
        for(; sorting_j > 0; sorting_j >>= 1) {
            if(_i >= sortingPasses) {
                break;
            }
            
            grid[0].bind();
            
            sortShader.uniform1i("grid", grid[1].id);
            
            sortShader.uniform1i("j", sorting_j);
            sortShader.uniform1i("k", sorting_k);
            
            blit(grid[0].target.fbo, root, root);
            //blitRoot(grid[0], 0, nc);
            
            grid.swap();
            
            ++_i;
        }
        if(_i >= sortingPasses) {
            break;
        }
    }
    
    if(sorting_k >= nc) sorting_k = 2;
    */
    
    n1 = glfwGetTime();
    
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
            printf("2(%u, %u) ", tp[i2 + 0], tp[i2 + 1]);
            printf("\t");
        }
        printf("\n");
    }
    */
    
    /// every cell bucket without a cell will have an index of MAX
    
    //if(sorting_k > 2) return;
    
    setShader.bind();
    setShader.uniform4f("a", root + 0.5f, root + 0.5f, 0, 0);
    
    offsetList.bind();
    blitRoot(offsetList, 0, max_count);
    
    
    gridToListShader.bind();
    gridToListShader.uniform1i("grid", grid[1].id);
    gridToListShader.uniform2i("size", size);
    
    offsetList.bind();
    
    blitRoot(offsetList, 0, count);
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, offsetList.target.fbo);
    glBindVertexArray(particleReverseVAO);
    glViewport(0, 0, root, root);
    glDrawArrays(GL_POINTS, max_count - count, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //printf("%f %f %f\n", n1 - n0, n2 - n1, n3 - n2);
}

void ParticleSystem::solveOnce(float dt) {
    glm::vec2 bound(10.0f, 200.0f);
    
    /// summing up weights
    
    weights.bind();
    
    weightShader.bind();
    weightShader.uniform1i("count", count);
    weightShader.uniform1f("h", h);
    weightShader.uniform1i("uP", positions[1].id);
    weightShader.uniform2i("size", size);
    weightShader.uniform1i("list", offsetList.id);
    weightShader.uniform1i("grid", grid[1].id);

    blit(weights.target.fbo, root, root);
    
    
    
    /// velocities' force solver
    
    velocities[0].bind();
    
    solverShader.bind();
    solverShader.uniform1f("spiky", spiky);
    solverShader.uniform1i("count", count);
    solverShader.uniform1f("h", h);
    solverShader.uniform1f("e", e);
    solverShader.uniform1i("uP", positions[1].id);
    solverShader.uniform1i("uW", weights.id);
    solverShader.uniform1i("uV", velocities[1].id);
    solverShader.uniform2i("size", size);
    solverShader.uniform2f("bound", bound);
    solverShader.uniform2f("gravity", gravity);
    solverShader.uniform1f("p0", p0);
    solverShader.uniform1f("K", K);
    solverShader.uniform1f("dt", dt);
    solverShader.uniform1i("list", offsetList.id);
    solverShader.uniform1i("grid", grid[1].id);
    
    blit(velocities[0].target.fbo, root, root);
    
    velocities.swap();
    
    
    velocities[0].bind();
    
    boundShader.bind();
    boundShader.uniform1i("uP", positions[1].id);
    boundShader.uniform1i("uV", velocities[1].id);
    boundShader.uniform1i("count", count);
    boundShader.uniform2i("size", size);
    boundShader.uniform2f("bound", bound);
    boundShader.uniform1i("order", 1);
    
    blit(velocities[0].target.fbo, root, root);
    
    velocities.swap();
    
    positions[0].bind();
    
    boundShader.uniform1i("uP", positions[1].id);
    boundShader.uniform1i("order", 0);
    
    blit(positions[0].target.fbo, root, root);
    
    positions.swap();
    
    
    
    
    /// p = p0 + v * dt
    
    positions[0].bind();
    
    stepShader.bind();
    stepShader.uniform1f("dt", dt);
    stepShader.uniform1i("uV", velocities[1].id);
    stepShader.uniform1i("uP", positions[1].id);
    stepShader.uniform2f("bound", bound);
    stepShader.uniform2i("size", size);
    stepShader.uniform1i("count", count);
    
    blit(positions[0].target.fbo, root, root);
    
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
    map();
    for(int i = 0; i < its; ++i) {
        //map();
        solveOnce(_dt);
    }
}

void ParticleSystem::render(GLuint target, const glm::vec2& size, const glm::vec2& scl) {
    drawShader.bind();
    drawShader.uniform1i("T", positions[1].id);
    drawShader.uniform1i("uV", velocities[1].id);
    drawShader.uniform2f("S", scl);
    drawShader.uniform2f("scl1", scl/size);
    drawShader.uniform2f("scl2", radius * scl / size);
    
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    glBindVertexArray(drawingVAO);
    glViewport(0, 0, size.x, size.y);
    glDrawArraysInstanced(GL_LINE_LOOP, 0, particleShapeVerticesNum, count);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
