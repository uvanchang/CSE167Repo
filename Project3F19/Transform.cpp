#include "Transform.h"

Transform::Transform(glm::mat4 M, GLuint shaderProgram, int id)
{
    this->M = M;
    this->setShaderProgram(shaderProgram);
    this->id = id;
    degree = 0.0f;
}

Transform::~Transform()
{
    for (Node* child: children)
    {
        delete child;
    }
}

void Transform::draw(glm::mat4 C)
{
    glm::mat4 newM = C * M;
    for (Node* node: children)
    {
        node->draw(newM);
    }
}

void Transform::update()
{
    float rotateDegree = 0.0f;
    switch (id)
    {
        case 1:
        case 2:
            rotateDegree = moveDir * glm::radians(0.5f);
            M = M * glm::translate(glm::vec3(0, 5, 0)) * glm::rotate(rotateDegree, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(0, -5, 0));
            degree += rotateDegree;
            if (degree > glm::radians(50.0f))
            {
                moveDir = -1;
            }
            if (degree < -glm::radians(50.0f))
            {
                moveDir = 1;
            }
            break;
        case 3:
        case 4:
            rotateDegree = moveDir * glm::radians(3.0f);
            M = M * glm::translate(glm::vec3(0, 5, 0)) * glm::rotate(rotateDegree, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(0, -5, 0));
            degree += rotateDegree;
            if (degree > glm::radians(50.0f))
            {
                moveDir = -1;
            }
            if (degree < -glm::radians(50.0f))
            {
                moveDir = 1;
            }
            break;
        default:
            break;
    }
    for (Node* node: children)
    {
        node->update();
    }
}

void Transform::addChild(Node* node)
{
    children.push_back(node);
    if (getShaderProgram() != -1)
    {
        node->setShaderProgram(getShaderProgram());
    }
}

void Transform::setMoveDir(int dir) {
    moveDir = dir;
}
