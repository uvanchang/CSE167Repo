#ifndef _TRACK_H_
#define _TRACK_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"
#include "BezierCurve.h"

class Track : public Node
{
private:
    glm::mat4 C;
    GLuint vao;
    GLuint vbos[2];
    GLuint ebo;
    GLuint lineVao;
    GLuint lineVbo;
    int indicesNum;
    std::vector<BezierCurve*> curves;
public:
    Track();
    ~Track();
    void draw(glm::mat4 C);
    void update();
    void setCurves(std::vector<BezierCurve*> curves);
    void movePoint(int num, glm::vec3 translate);
};

#endif
