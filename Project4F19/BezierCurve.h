#ifndef _BEZIERCURVE_H_
#define _BEZIERCURVE_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Geometry.h"

class BezierCurve : public Geometry
{
private:
    glm::mat4 C;
    GLuint vao;
    GLuint vbos[2];
    GLuint ebo;
    int indicesNum;
public:
    BezierCurve(std::string filename);
    BezierCurve();
    void draw(glm::mat4 C);
    void update();
    glm::vec3 getPoint(GLuint t);
};

#endif
