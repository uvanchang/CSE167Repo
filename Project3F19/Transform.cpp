#include "Transform.h"

bool Transform::boundingSphereOn = false;
bool Transform::cullingOn = false;

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

int Transform::draw(glm::mat4 C, std::vector<std::pair<glm::vec3, glm::vec3>> frustumPlanes)
{
    if (id == 2 && !boundingSphereOn)
    {
        return 0;
    }
    
    if (id == 1 && cullingOn)
    {
        center = glm::vec3(C[3][0], C[3][1], C[3][2]);
        //for (std::pair<glm::vec3, glm::vec3> plane: frustumPlanes)
        //{
        for(int i = 0; i < frustumPlanes.size(); i++)
        {
            auto plane = frustumPlanes[i];
            float dist = glm::dot(center, plane.second) - glm::dot(plane.first, plane.second);
            if (dist > radius)
            {
                return 0;
            }
        }
    }
    
    int count = 0;
    glm::mat4 newM = C * M;
    for (Node* node: children)
    {
        count += node->draw(newM, frustumPlanes);
    }
    if (id == 1)
    {
        return count + 1;
    }
    else
    {
        return count;
    }
}

void Transform::update()
{
    float rotateDegree = 0.0f;
    switch (id)
    {
        case 3:
        case 4:
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
        case 5:
        case 6:
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
