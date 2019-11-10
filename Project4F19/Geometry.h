#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class Geometry : public Node
{
private:
    glm::mat4 C;
    GLuint vao;
    GLuint vbos[2];
    GLuint ebo;
    int indicesNum;
public:
    Geometry(std::string filename);
    ~Geometry();
    int draw(glm::mat4 C, std::vector<std::pair<glm::vec3, glm::vec3>> frustumPlanes);
    void update();
};

#endif
