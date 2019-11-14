#ifndef _BEZIERCURVE_H_
#define _BEZIERCURVE_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class BezierCurve : public Node
{
private:
    glm::mat4 C;
    std::vector<glm::vec3> coeff;
    GLuint vao;
    GLuint vbo;
    int indicesNum;
public:
    std::vector<glm::vec3> p;
    BezierCurve(std::vector<glm::vec3> p);
    ~BezierCurve();
    void draw(glm::mat4 C);
    void update();
    void updateCoeff();
    glm::vec3 getPoint(float t);
};

#endif
