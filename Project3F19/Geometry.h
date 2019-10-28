#ifndef  _GEOMETRY_H_
#define _GEOMETRY_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class Geometry : public Node
{
private:
	glm::mat4 C;
	std::vector<Node*> nodes;
public:
	void addChild(Node*);
};

#endif