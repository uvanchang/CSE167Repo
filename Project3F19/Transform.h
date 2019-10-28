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
public:
	Transform(std::string filename);
};

#endif