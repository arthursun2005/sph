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

inline void _check_gl_error(const char *file, int line) {
    GLenum err (glGetError());
    
    while(err != GL_NO_ERROR) {
        const char* error;
        
        switch(err) {
            case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        
        printf("GL_%s - %s : %d \n", error, file, line);
        err = glGetError();
    }
}

#define check_gl_error (_check_gl_error(__FILE__, __LINE__))

void initBases();
void freeBases();

void genPoly(float* const vs, int n, float s, float aoffset, int offset);

void print_ftexture_red(int _x, int _y, int _c);
void print_itexture_red(int _x, int _y, int _c);
void print_ftexture_rg(int _x, int _y, int _c);
void print_itexture_rg(int _x, int _y, int _c);

template <class T>
void print_array(T* tp, int _c, int _n, const char* x);

GLuint CreateProgram(const char * _vs[], const char * _fs[], const char* m);
GLuint CreateProgram(const char * _cs[], const char* m);
GLuint LoadProgram(const char * _vs, const char * _fs, const char * _all);
GLuint LoadProgram(const char * _cs, const char * _all);

void checkShader(GLuint x);
void checkProgram(GLuint x);

static GLuint texture_count = 0;
#define reset_texture_count (texture_count = 0)

void blit(GLuint target, GLuint x, GLuint y);

void blit(GLuint target, GLuint x, GLuint y, GLuint w, GLuint h);

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
    
    void init(const char* cs, const char* all){
        program = LoadProgram(cs, all);
    }
    
    void init(const char** vs, const char** fs, const char* m){
        program = CreateProgram(vs, fs, m);
    }
    
    void init(const char* vs, const char* fs, const char* all){
        program = LoadProgram(vs, fs, all);
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
    
    void uniform2i(const char* n, const glm::ivec2& x) const
    {
        glUniform2i(glGetUniformLocation(program, n), x.x, x.y);
    }
    
    void uniform3i(const char* n, int i0, int i1, int i2) const
    {
        glUniform3i(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    void uniform3i(const char* n, const glm::ivec3& x) const
    {
        glUniform3i(glGetUniformLocation(program, n), x.x, x.y, x.z);
    }
    
    void uniform4i(const char* n, int i0, int i1, int i2, int i3) const
    {
        glUniform4i(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    void uniform4i(const char* n, const glm::ivec4& x) const
    {
        glUniform4i(glGetUniformLocation(program, n), x.x, x.y, x.z, x.w);
    }
    
    void uniform1ui(const char* n, unsigned int i0) const
    {
        glUniform1ui(glGetUniformLocation(program, n), i0);
    }
    
    void uniform2ui(const char* n, unsigned int i0, unsigned int i1) const
    {
        glUniform2ui(glGetUniformLocation(program, n), i0, i1);
    }
    
    void uniform2ui(const char* n, const glm::uvec2& x) const
    {
        glUniform2ui(glGetUniformLocation(program, n), x.x, x.y);
    }
    
    void uniform3ui(const char* n, unsigned int i0, unsigned int i1, unsigned int i2) const
    {
        glUniform3ui(glGetUniformLocation(program, n), i0, i1, i2);
    }
    
    void uniform3ui(const char* n, const glm::uvec3& x) const
    {
        glUniform3ui(glGetUniformLocation(program, n), x.x, x.y, x.z);
    }
    
    void uniform4ui(const char* n, unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) const
    {
        glUniform4ui(glGetUniformLocation(program, n), i0, i1, i2, i3);
    }
    
    void uniform4ui(const char* n, const glm::uvec4& x) const
    {
        glUniform4ui(glGetUniformLocation(program, n), x.x, x.y, x.z, x.w);
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
    
    void bind(const Texture& texture, GLenum x) const;
};

class Texture
{
    
public:
    
    GLuint id;
    GLuint texture;
    FrameBuffer target;
    
    Texture() : id(0) {
    }
    
    void init(GLenum mode)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        target.init();
    }
    
    void image(GLenum iformat, GLenum format, GLuint x, GLuint y, GLenum type, void* pixels)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, x, y, 0, format, type, pixels);
        setFBO(GL_FRAMEBUFFER);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    inline void setFBO(GLenum x) {
        target.bind(*this, x);
    }
    
    void bind()
    {
        id = ++texture_count;
        
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
    
public:
    
    DoubleTexture(){
    }
    
    void init(GLenum mode)
    {
        textures[0].init(mode);
        textures[1].init(mode);
    }
    
    void image(GLenum iformat, GLenum format, GLuint x, GLuint y, GLenum type, void* pixels) {
        textures[0].image(iformat, format, x, y, type, pixels);
        textures[1].image(iformat, format, x, y, type, pixels);
    }
    
    void free()
    {
        textures[0].free();
        textures[1].free();
    }
    
    inline const Texture& operator [] (int i) const {
        return textures[i];
    }
    
    inline Texture& operator [] (int i) {
        return textures[i];
    }
    
    inline void swap() {
        std::swap(textures[0], textures[1]);
    }
};

extern Shader baseShader;
extern Shader blurShader;
extern GLuint baseVAO;
extern GLuint baseVBO;

#endif /* Setup_hpp */
