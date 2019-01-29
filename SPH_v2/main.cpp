//
//  main.cpp
//  SPH_v2
//
//  Created by Arthur Sun on 1/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include "ParticleSystem.hpp"

#include <thread>
#include <chrono>

ParticleSystem* ps;

GLFWwindow *window;
const GLFWvidmode* mode;

double mouseX = 0.0f, mouseY = 0.0f;
double pmouseX = mouseX, pmouseY = mouseY;
float width, height;
float dt = 0.016f;
float windowScl = 0.75f;
float scl = 100.0f;
bool pressed;

int framesPerSecond = 0;
float lastSecondTime = glfwGetTime();

bool dragged = false;

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_RELEASE && !dragged) {
        ps->addRect((mouseX * 2.0f - width) * 2.0f / scl, (mouseY * 2.0f - height) * -2.0f / scl, 30, 30);
        //ps->addParticle((mouseX * 2.0f - width) * 2.0f, (mouseY * 2.0f - height) * -2.0f);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) {
        if(GLFW_KEY_C) {
            printf("%d \n", ps->getCount());
        }
    }
}

void initialize() {
    initBases();
    
    ps = new ParticleSystem(0.05f, 0.025f, 1 << 12);
}

void free() {
    freeBases();
    
    delete ps;
    ps = nullptr;
}

void draw() {
    ps->solve(dt * 0.2f, 1);
    ps->render(0, glm::vec2(2.0f * width, 2.0f * height), glm::vec2(scl, scl));
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    scl *= exp(yoffset * 0.001f);
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    if(!glfwInit()) return 1;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    
    width = (float)mode->width * windowScl;
    height = (float)mode->height * windowScl;
    
    window = glfwCreateWindow(width, height, "SPH", NULL, NULL);
    
    glfwMakeContextCurrent(window);
    glewInit();
    
    initialize();
    
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    
    glfwSetCursor(window, cursor);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    
    ps->addRect(100.0f / scl, 100.0f / scl, 30, 30);
    do {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        float currentTime = glfwGetTime();
        bool pressedNow = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        ++framesPerSecond;
        if(currentTime - lastSecondTime >= 1.0f) {
            printf("%f ms/frame \n", 1000.0f * (currentTime - lastSecondTime)/(float)framesPerSecond);
            framesPerSecond = 0;
            lastSecondTime = currentTime;
        }
        
        if(pressedNow == GLFW_PRESS) {
            if(!dragged && pressed == GLFW_PRESS) {
                if(mouseX != pmouseX || mouseY != pmouseY) dragged = true;
            }
        }else{
            dragged = false;
        }
        
        if(dragged) {
        }
        
        draw();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        pmouseX = mouseX;
        pmouseY = mouseY;
        
        pressed = pressedNow;
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(30));
    } while (glfwWindowShouldClose(window) == GL_FALSE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    free();
    glfwDestroyCursor(cursor);
    glfwTerminate();
    return 0;
}

