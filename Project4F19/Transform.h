#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class Transform : public Node
{
private:
    glm::mat4 M;
    std::vector<Node*> children;
    int moveDir;
public:
    Transform(glm::mat4 M, GLuint shaderProgram = -1, int id = 0);
    ~Transform();
    void draw(glm::mat4 C);
    void update();
    void addChild(Node* node);
    void setMoveDir(int dir);
};

#endif
