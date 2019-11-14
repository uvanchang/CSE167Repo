#include "Transform.h"

Transform::Transform(glm::mat4 M, GLuint shaderProgram)
{
    this->M = M;
    this->setShaderProgram(shaderProgram);
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
