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
#include <vector>

class ParticleSystem
{
    float h;
    float p0;
    float K;
    float e;
    
    float spiky;
    float radius;
    
    int max_count;
    int rootLog2;
    float invRoot;
    int root;
        
    DoubleTexture positions;
    DoubleTexture velocities;
    Texture weights;
    DoubleTexture grid;
    Texture offsetList;
    Texture counts;
    
    glm::vec2 invSize;
    glm::ivec2 size;
    
    int count;
    
    GLuint particleUVs;
    
    GLuint particleVAO;
    
    GLuint drawingVAO;
    GLuint particleShape;
    int particleShapeVerticesNum;
    
    void blitRoot(const Texture& target, int start, int count);
    
    void map();
    
    GLuint packedData;
    GLuint packedDataVAO;
    
public:
    
    Shader drawShader;
    Shader setShader;
    Shader isetShader;
    Shader usetShader;
    Shader solverShader;
    Shader stepShader;
    Shader weightShader;
    Shader rectShader;
    Shader mapToGridShader;
    
    Shader sortShader;
    Shader gridToListShader;
    
    glm::vec2 gravity;
    
    ParticleSystem(float h, float r, int level) : h(h), radius(r), root(1 << level), p0(1.0f), K(25000.0f), e(0.7f), max_count(1 << (level << 1)), rootLog2(level)
    {
        invRoot = 1.0f/(float)root;
        gravity = glm::vec2(0.0f, -9.8f);
        invSize = glm::vec2(invRoot, invRoot);
        size = glm::ivec2(root, root);
        
        particleShapeVerticesNum = 3;
        
        positions.init(GL_NEAREST);
        positions.image(GL_RG32F, GL_RG, root, root, GL_FLOAT, 0);
        
        velocities.init(GL_NEAREST);
        velocities.image(GL_RG32F, GL_RG, root, root, GL_FLOAT, 0);
        
        weights.init(GL_NEAREST);
        weights.image(GL_R32F, GL_RED, root, root, GL_FLOAT, 0);
        
        grid.init(GL_NEAREST);
        grid.image(GL_RG32I, GL_RG_INTEGER, root, root, GL_INT, 0);
        
        counts.init(GL_NEAREST);
        counts.image(GL_R32F, GL_RED, root, root, GL_FLOAT, 0);
        
        offsetList.init(GL_NEAREST);
        offsetList.image(GL_RG32F, GL_RG, root, root, GL_FLOAT, 0);
        
        glGenVertexArrays(1, &particleVAO);
        glGenVertexArrays(1, &drawingVAO);
        glGenBuffers(1, &particleUVs);
        glGenBuffers(1, &particleShape);
        
        std::vector<float> tp(max_count * 2);
        
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
        glBufferData(GL_ARRAY_BUFFER, 2 * max_count * sizeof(float), tp.data(), GL_STATIC_DRAW);
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
        
        glBindVertexArray(0);
        
        count = 0;
        
        createStandardShaders();
        resetVelocities();
        
        /*
        size_t positionSize = sizeof(glm::vec2) * max_count;
        size_t velocitySize = sizeof(glm::vec2) * max_count;
        size_t weightsSize = sizeof(float) * max_count;
        size_t gridSize = sizeof(glm::vec2) * max_count;
        
        size_t positionOffset = 0;
        size_t velocityOffset = positionOffset + positionSize;
        size_t weightsOffset = velocityOffset + velocitySize;
        size_t gridOffset = weightsOffset + weightsSize;
        
        glGenBuffers(1, &packedData);
        glGenVertexArrays(1, &packedDataVAO);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, packedData);
        
        glBufferData(GL_SHADER_STORAGE_BUFFER, positionSize + velocitySize + weightsSize + gridSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        
        check_gl_error;
        
        glBindVertexArray(packedDataVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, packedData);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, packedData, positionOffset, positionSize);
        //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, packedData, velocityOffset, velocitySize);
        //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, packedData, weightsOffset, weightsSize);
        //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, packedData, gridOffset, gridSize);
        
        check_gl_error;

        glBindVertexArray(0);
         */
    }
    
    ~ParticleSystem() {
        positions.free();
        velocities.free();
        weights.free();
        grid.free();
        offsetList.free();

        drawShader.free();
        setShader.free();
        isetShader.free();
        usetShader.free();
        solverShader.free();
        stepShader.free();
        weightShader.free();
        rectShader.free();
        mapToGridShader.free();
        
        sortShader.free();
                
        gridToListShader.free();
        
        glDeleteVertexArrays(1, &particleVAO);
        glDeleteBuffers(1, &particleUVs);
        
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
    
    void run();
};

#endif /* ParticleSystem_hpp */
