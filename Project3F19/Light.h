#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Model.h"

class Light : public Model
{
public:
	Light(std::string fileName, glm::vec3 lightPos);
	~Light();
	glm::vec3 lightPos;
	void draw();
	void rotate(glm::vec3 lastPoint, glm::vec3 curPoint);
	void changeDistance(double offset);
};

#endif

