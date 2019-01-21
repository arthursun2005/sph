//
//  Setup.hpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef Setup_hpp
#define Setup_hpp

#include <iostream>
#include <float.h>
#include <cmath>
#include <unistd.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

static const GLfloat baseQuad[] = {
    -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f
};

static GLuint baseVAO;
static GLuint baseVBO;

void initBases();
void freeBases();

void genPoly(float* const vs, int n, float s, float aoffset, int offset);

GLuint CreateProgram(const char * _vs[], const char * _fs[]);

void checkShader(GLuint x);

static GLuint texture_count = 0;

inline int nextPowerOfTwo(int x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

struct Shader
{
    GLuint program;
    
    Shader() {
    }
    
    void init(const char** vs, const char** fs){
        program = CreateProgram(vs, fs);
    }
    
    void bind() {
        glUseProgram(program);
    }
    
    void free() {
        glDeleteProgram(program);
    }
    
    void uniform1i(const char* n, int i0) const
    {
        glUniform1i(glGetUniformLocation(program, n), i0);
    }
    
    void uniform2i(const char* n, int i0, int i1) const
    {
        glUniform2i(glGetUniformLocation(program, n), i0, i1);
    }
    
    void uniform3i(const char* n, int i0, int i1, int i2) const
    {
        glUniform3i(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    void uniform4i(const char* n, int i0, int i1, int i2, int i3) const
    {
        glUniform4i(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    void uniform1f(const char* n, float f0) const
    {
        glUniform1f(glGetUniformLocation(program, n), f0);
    }
    
    void uniform2f(const char* n, float f0, float f1) const
    {
        glUniform2f(glGetUniformLocation(program, n), f0, f1);
    }
    
    void uniform2f(const char* n, const glm::vec2& x) const
    {
        glUniform2f(glGetUniformLocation(program, n), x.x, x.y);
    }
    
    void uniform3f(const char* n, float f0, float f1, float f2) const
    {
        glUniform3f(glGetUniformLocation(program, n), f0, f1, f2);
    }
    
    void uniform3f(const char* n, const glm::vec3& x) const
    {
        glUniform3f(glGetUniformLocation(program, n), x.x, x.y, x.z);
    }
    
    void uniform4f(const char* n, float f0, float f1, float f2, float f3) const
    {
        glUniform4f(glGetUniformLocation(program, n), f0, f1, f2, f3);
    }
    
    void uniform4f(const char* n, const glm::vec4& x) const
    {
        glUniform4f(glGetUniformLocation(program, n), x.x, x.y, x.z, x.w);
    }
};

class Texture;

struct FrameBuffer
{
    GLuint fbo;
    
    FrameBuffer() {}
    
    void init(){
        glGenFramebuffers(1, &fbo);
    }
    
    void free() {
        glDeleteFramebuffers(1, &fbo);
    }
    
    void bind(const Texture& texture) const;
};

class Texture
{
    
public:
    
    GLuint id;
    GLuint texture;
    FrameBuffer target;
    
    Texture() {
        id = ++texture_count;
    }
    
    void init(GLenum mode)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        target.init();
    }
    
    void image(GLenum format, GLuint x, GLuint y, GLenum type, void* pixels) const
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, type, pixels);
        target.bind(*this);
    }
    
    void bind() const
    {
        glActiveTexture(GL_TEXTURE0 + id);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    
    inline void free()
    {
        glDeleteTextures(1, &texture);
        target.free();
    }
};

class DoubleTexture {
    
private:
    
    Texture textures[2];
    uint8_t x = 0;
    
public:
    
    DoubleTexture(){
    }
    
    void init(GLenum mode)
    {
        textures[0].init(mode);
        textures[1].init(mode);
    }
    
    void image(GLenum format, GLuint x, GLuint y, GLenum type, void* pixels) {
        textures[0].image(format, x, y, type, pixels);
        textures[1].image(format, x, y, type, pixels);
    }
    
    void free()
    {
        textures[0].free();
        textures[1].free();
    }
    
    inline const Texture& operator [] (int i) const {
        return textures[i^x];
    }
    
    inline Texture& operator [] (int i) {
        return textures[i^x];
    }
    
    inline void swap() {
        x ^= 0b1;
    }
};

static const char* baseVShader = R"(

#version 410 core

layout (location = 0) in vec2 position;

uniform vec2 S;
uniform vec2 P;

out vec2 at;

void main() {
    at = position;
    gl_Position = vec4(position * S + P, 0.0, 1.0);
}

)";

static Shader baseShader;

#endif /* Setup_hpp */
