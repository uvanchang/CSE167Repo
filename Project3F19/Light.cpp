#include "Light.h"

Light::Light(std::string fileName, glm::vec3 lightPos) : Model(fileName)
{
	this->lightPos = lightPos;
}

Light::~Light()
{}

void Light::draw()
{
	model = glm::mat4(1.0);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.05f));
	Model::draw();
}

void Light::rotate(glm::vec3 lastPoint, glm::vec3 curPoint)
{
	glm::vec3 direction = curPoint - lastPoint;
	float velocity = glm::length(direction);

	glm::vec3 rotationAxis = glm::cross(lastPoint, curPoint);
	float rotationAngle = velocity;

	lightPos = glm::rotate(lightPos, rotationAngle, rotationAxis);
}

void Light::changeDistance(double offset)
{
	if (offset > 0) {
		lightPos *= 0.99;
	}
	else {
		lightPos *= 1.01;
	}
}