//
//  Setup.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "Setup.hpp"
#include <fstream>
#include <sstream>

GLuint CreateProgram(const char * _vs[], const char * _fs[]) {
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

GLuint LoadProgram(const char * _vs, const char * _fs, const char * _all) {
    std::ifstream vs(_vs, std::ios::in);
    std::ifstream fs(_fs, std::ios::in);
    std::ifstream all(_all, std::ios::in);
    
    std::stringstream vstr, fstr, astr;
    
    bool failed = false;
    
    if(vs.is_open()) {
        vstr << vs.rdbuf();
        vs.close();
    }else{
        printf("%s can't be opened \n", _vs);
        failed = true;
    }
    
    if(fs.is_open()) {
        fstr << vs.rdbuf();
        fs.close();
    }else{
        printf("%s can't be opened \n", _fs);
        failed = true;
    }
    
    if(_all && all.is_open()) {
        astr << all.rdbuf();
        all.close();
    }else if(_all) {
        printf("%s can't be opened \n", _all);
        failed = true;
    }
    
    if(failed)
        return 0;
    
    std::string __v = astr.str();
    std::string __f(__v);
    std::string __vs = vstr.str();
    std::string __fs = fstr.str();
    
    __v.append(__vs);
    __f.append(__fs);
    
    const char* fvs = __v.c_str();
    const char* ffs = __f.c_str();
    
    return CreateProgram(&fvs, &ffs);
}

void checkShader(GLuint x) {
    int status;
    glGetShaderiv(x, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
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
    baseShader.init("GLSL/base.vs", "GLSL/base.fs", "GLSL/shared.glsl");
    
    glGenVertexArrays(1, &baseVAO);
    glGenBuffers(1, &baseVBO);
    
    glBindVertexArray(baseVAO);
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
