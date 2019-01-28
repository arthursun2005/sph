//
//  ParticleSystem.hpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include "Setup.hpp"

class ParticleSystem
{
    float h;
    float p0;
    float K;
    float e;
    
    float spiky;
    float radius;
    
    int max_count;
    int root;
    
    int px = 1300967;
    int py = 179482213;
    
    DoubleTexture positions;
    DoubleTexture velocities;
    Texture weights;
    DoubleTexture grid;
    Texture bitPositions;
    Texture offsetList;
    
    glm::vec2 invSize;
    
    int count;
    
    GLuint particleUVs;
    
    GLuint particleReverseUVs;
    
    GLuint particleVAO;
    
    GLuint particleReverseVAO;
    
    GLuint drawingVAO;
    GLuint particleShape;
    int particleShapeVerticesNum;
    
    void blitRoot(const Texture& target, int start, int count);
    void blitQuad(const Texture& target);
    
    void map();
    
public:
    
    Shader drawShader;
    Shader setShader;
    Shader setiShader;
    Shader solverShader;
    Shader stepShader;
    Shader weightShader;
    Shader rectShader;
    Shader mapToGridShader;
    
    Shader sortShader;
    Shader gridToListShader;
    
    Shader boundShader;
    
    glm::vec2 gravity;
    
    ParticleSystem(float h, float r, int c) : h(h), radius(r), root(ceilf(sqrtf((float)c))), p0(1.0f), K(15000.0f), e(16.0f)
    {
        gravity = glm::vec2(0.0f, -9.8f);
        max_count = root * root;
        invSize = glm::vec2(1.0f/(float)root, 1.0f/(float)root);
        
        /// triangles look quite nice
        particleShapeVerticesNum = 3;
        
        positions.init(GL_NEAREST);
        positions.image(GL_RG16F, GL_RG, root, root, GL_HALF_FLOAT, 0);
        
        velocities.init(GL_NEAREST);
        velocities.image(GL_RG16F, GL_RG, root, root, GL_HALF_FLOAT, 0);
        
        weights.init(GL_NEAREST);
        weights.image(GL_RG16F, GL_RG, root, root, GL_HALF_FLOAT, 0);
        
        grid.init(GL_NEAREST);
        grid.image(GL_RG32UI, GL_RG_INTEGER, root, root, GL_UNSIGNED_INT, 0);
        
        bitPositions.init(GL_NEAREST);
        bitPositions.image(GL_RGB16F, GL_RGB, root, root, GL_HALF_FLOAT, 0);
        
        offsetList.init(GL_NEAREST);
        offsetList.image(GL_RG16F, GL_RG, root, root, GL_HALF_FLOAT, 0);
        
        glGenVertexArrays(1, &particleVAO);
        glGenVertexArrays(1, &drawingVAO);
        glGenVertexArrays(1, &particleReverseVAO);
        glGenBuffers(1, &particleReverseUVs);
        glGenBuffers(1, &particleUVs);
        glGenBuffers(1, &particleShape);
        
        float* tp = (float*)malloc(sizeof(float) * max_count * 2);
        
        glBindVertexArray(particleVAO);
        
        for(int y = 0; y < root; ++y) {
            for(int x = 0; x < root; ++x) {
                int i = (x + y * root) << 1;
                /// texel coords are pixel centered
                tp[i + 0] = (x + 0.5f) / (float)root;
                tp[i + 1] = (y + 0.5f) / (float)root;
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, particleUVs);
        glBufferData(GL_ARRAY_BUFFER, 2 * max_count * sizeof(float), tp, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(drawingVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, particleUVs);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        float vs[particleShapeVerticesNum << 1];
        genPoly(vs, particleShapeVerticesNum, 1.0f, 0.0f, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, particleShape);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glVertexAttribDivisor(0, 1);
        glVertexAttribDivisor(1, 0);
        
        glBindVertexArray(particleReverseVAO);
        
        for(int y = 1; y <= root; ++y) {
            for(int x = 1; x <= root; ++x) {
                int i = ((root - x) + (root - y) * root) << 1;
                tp[i + 0] = (x - 0.5f) / (float)root;
                tp[i + 1] = (y - 0.5f) / (float)root;
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, particleReverseUVs);
        glBufferData(GL_ARRAY_BUFFER, 2 * max_count * sizeof(float), tp, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
        free(tp);
        
        count = 0;
        
        createStandardShaders();
        resetVelocities();
    }
    
    ~ParticleSystem() {
        positions.free();
        velocities.free();
        weights.free();
        grid.free();
        offsetList.free();

        drawShader.free();
        setShader.free();
        setiShader.free();
        solverShader.free();
        stepShader.free();
        weightShader.free();
        rectShader.free();
        mapToGridShader.free();
        bitPositions.free();
        
        boundShader.free();
        sortShader.free();
                
        gridToListShader.free();
        
        glDeleteVertexArrays(1, &particleVAO);
        glDeleteBuffers(1, &particleUVs);
        
        glDeleteVertexArrays(1, &particleReverseVAO);
        glDeleteBuffers(1, &particleReverseUVs);
        
        glDeleteVertexArrays(1, &drawingVAO);
        glDeleteBuffers(1, &particleShape);
    }
    
    inline int getCount() const
    {
        return count;
    }
    
    void createStandardShaders();
    
    void addParticle(float x, float y);
    void addRect(float x, float y, uint32_t hx, uint32_t hy);
    
    void resetVelocities();
    
    void solveOnce(float dt);
    void solve(float dt);
    void solve(float dt, int its);
    
    void render(GLuint target, const glm::vec2& invSize, const glm::vec2& scl);
};

#endif /* ParticleSystem_hpp */
