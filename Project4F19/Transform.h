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
    int id;
    int moveDir;
    float degree;
    glm::vec3 center;
    float radius = 25.0f;
public:
    Transform(glm::mat4 M, GLuint shaderProgram = -1, int id = 0);
    ~Transform();
    int draw(glm::mat4 C, std::vector<std::pair<glm::vec3, glm::vec3>> frustumPlanes);
    void update();
    void addChild(Node* node);
    void setMoveDir(int dir);
    static bool boundingSphereOn;
    static bool cullingOn;
};

#endif
