#ifndef _NODE_H_
#define _NODE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "shader.h"

class Node
{
private:
    GLuint shaderProgram;
public:
    virtual void draw(glm::mat4 C) = 0;
    virtual void update() = 0;
    GLuint getShaderProgram() {
        return shaderProgram;
    }
    void setShaderProgram(GLuint shaderProgram)
    {
        this->shaderProgram = shaderProgram;
    }
};

#endif
