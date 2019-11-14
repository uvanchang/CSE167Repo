#include "BezierCurve.h"

BezierCurve::BezierCurve(std::vector<glm::vec3> p)
{
    this->p = p;
    this->coeff = {glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)};
    coeff[0] = -p[0] + 3.0f * p[1] - 3.0f * p[2] + p[3];
    coeff[1] = 3.0f * p[0] - 6.0f * p[1] + 3.0f * p[2];
    coeff[2] = -3.0f * p[0] + 3.0f * p[1];
    coeff[3] = p[0];
    
    std::vector<glm::vec3> points;
    
    for (float i = 0.0f; i < 1.0f; i += 1.0f/150.0f)
    {
        points.push_back(getPoint(i));
    }
    
    indicesNum = points.size();
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
                 points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    C = glm::mat4(1.0f);
    
    std::cout << "Initialized BezierCurve" << std::endl;
}

BezierCurve::~BezierCurve()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glDeleteProgram(getShaderProgram());
}

void BezierCurve::draw(glm::mat4 C)
{
    glUseProgram(getShaderProgram());
    glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(this->C));
    glUniform3fv(glGetUniformLocation(getShaderProgram(), "color"), 1, glm::value_ptr(glm::vec3(0)));
    
    glBindVertexArray(vao);
    glPointSize(10);
    glDrawArrays(GL_LINE_STRIP, 0, indicesNum);
    glBindVertexArray(0);
}

void BezierCurve::update()
{
}

void BezierCurve::updateCoeff()
{
    coeff[0] = -p[0] + 3.0f * p[1] - 3.0f * p[2] + p[3];
    coeff[1] = 3.0f * p[0] - 6.0f * p[1] + 3.0f * p[2];
    coeff[2] = -3.0f * p[0] + 3.0f * p[1];
    coeff[3] = p[0];
    
    std::vector<glm::vec3> points;
    
    for (float i = 0.0f; i < 1.0f; i += 1.0f/150.0f)
    {
        points.push_back(getPoint(i));
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
                 points.data(), GL_STATIC_DRAW);
}

glm::vec3 BezierCurve::getPoint(float t)
{
    return coeff[0] * pow(t, 3.0f) + coeff[1] * pow(t, 2.0f) + coeff[2] * t + coeff[3];
}

