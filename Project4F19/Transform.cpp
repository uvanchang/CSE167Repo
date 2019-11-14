#include "Transform.h"
#include "Window.h"

Transform::Transform(glm::mat4 M, GLuint shaderProgram, GLuint id)
{
    this->M = M;
    this->setShaderProgram(shaderProgram);
    this->id = id;
    this->lastTime = glfwGetTime();
    this->dist = 0;
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
    float currentTime = glfwGetTime();
    if (id == 1)
    {
        float deltaTime;
        if (!Window::isPaused)
        {
            deltaTime = currentTime - lastTime;
        }
        else {
            deltaTime = 0;
        }
        glm::vec3 position = spherePosition(deltaTime);
        M = glm::translate(position) * glm::mat4(glm::mat3(M));
        if (Window::isRider)
        {
            Window::eye = position;
            Window::center = position + sphereTangent(deltaTime);
            Window::view = glm::lookAt(Window::eye, Window::center, Window::up);
        }
    }
    lastTime = currentTime;
        
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

glm::vec3 Transform::spherePosition(float deltaTime)
{
    float frameDistance = 5 * deltaTime;
    glm::vec3 position;
    
    if ((int)(frameDistance + dist) > (int)dist) // should go to next curve
    {
        frameDistance = frameDistance + dist - (int)(frameDistance + dist);
    }
    position = Window::track->curves[(int)dist]->getPoint(dist + (frameDistance / Window::track->curves[(int)dist]->length) - (int)dist);
    
    dist += frameDistance / Window::track->curves[(int)dist]->length;
    if (dist >= Window::track->curves.size())
    {
        dist -= Window::track->curves.size();
    }
    
    return position;
}

glm::vec3 Transform::sphereTangent(float deltaTime)
{
    float frameDistance = 5 * deltaTime;
    glm::vec3 position = Window::track->curves[(int)dist]->getTangent(dist + (frameDistance / Window::track->curves[(int)dist]->length) - (int)dist);
    return position;
}
