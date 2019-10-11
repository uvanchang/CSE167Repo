#ifndef _MODEL_H_
#define _MODEL_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Object.h"

class Model : public Object
{
private:
	GLuint vao;
	GLuint vbos[2];
	GLuint ebo;
	int indicesNum;
public:
	Model(std::string fileName);
	~Model();

	void draw();

	void rotate(glm::vec3 lastPoint, glm::vec3 curPoint);
	void changeSize(double offset);
};

#endif

