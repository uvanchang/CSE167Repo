#ifndef _SPHERE_H_
#define _SPHERE_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class Sphere : public Node
{
private:
    glm::mat4 C;
    GLuint vao;
    GLuint vbos[2];
    GLuint ebo;
    int indicesNum;
public:
    Sphere(std::string filename);
    ~Sphere();
    void draw(glm::mat4 C);
    void update();
};

#endif
