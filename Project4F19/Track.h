#ifndef _TRACK_H_
#define _TRACK_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Geometry.h"
#include "BezierCurve.h"

class Track : public Geometry
{
private:
    glm::mat4 C;
    GLuint vao;
    GLuint vbos[2];
    GLuint ebo;
    BezierCurve children[8];
public:
    Track(std::string filename);
    Track();
    void draw(glm::mat4 C);
    void update();
};

#endif
