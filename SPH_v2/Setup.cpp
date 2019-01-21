//
//  Setup.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Setup.hpp"

const char* baseFShader = R"(

#version 410 core

layout (location = 0) out vec4 a;

uniform sampler2D T;
uniform float u;
uniform vec4 A;
uniform vec2 invSize;

void main() {
    if(u == 0.0) {
        a = A;
    }else{
        a = min(max(texture(T, gl_FragCoord.xy * invSize) * u + A, 0.0), 1.0);
    }
}

)";

GLuint CreateProgram(const char * _vs[], const char * _fs[]){
    GLuint programID = glCreateProgram();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, _vs, nullptr);
    glShaderSource(fs, 1, _fs, nullptr);
    glCompileShader(vs);
    glCompileShader(fs);
    checkShader(vs);
    checkShader(fs);
    glAttachShader(programID, vs);
    glAttachShader(programID, fs);
    glLinkProgram(programID);
    glDetachShader(programID, vs);
    glDetachShader(programID, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return programID;
}

void checkShader(GLuint x) {
    int status;
    glGetShaderiv(x, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int length;
        glGetShaderiv(x, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length];
        glGetShaderInfoLog(x, length, nullptr, log);
        printf("%s \n", log);
    }
}

void FrameBuffer::bind(const Texture& texture) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.texture, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initBases() {
    baseShader.init(&baseVShader, &baseFShader);
    
    glGenVertexArrays(1, &baseVAO);
    glBindVertexArray(baseVAO);
    glGenBuffers(1, &baseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, baseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseQuad), baseQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void freeBases() {
    baseShader.free();
    glDeleteVertexArrays(1, &baseVAO);
    glDeleteBuffers(1, &baseVBO);
}

void genPoly(float* const vs, int n, float s, float aoffset, int offset) {
    float a = 2.0f * M_PI / n;
    float k_c = cosf(a);
    float k_s = sinf(a);
    float x = cosf(aoffset) * s;
    float y = sinf(aoffset) * s;
    for(uint32_t i = 0; i < n; ++i) {
        vs[(i << 1) + offset] = x;
        vs[(i << 1) + 1 + offset] = y;
        x = x * k_c - y * k_s;
        y = vs[(i << 1) + offset] * k_s + y * k_c;
    }
}
