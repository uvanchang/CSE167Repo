#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

#include "Object.h"
#include "PointCloud.h"
#include "shader.h"
#include "Model.h"
#include "Light.h"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

class Window
{
public:
	static int width;
	static int height;
	static const char* windowTitle;
	static Model* bunny;
	static Model* dragon;
	static Model* bear;
	static Light* light;
	static Object* currentObj;
	static glm::vec3 curPoint;
	static glm::vec3 lastPoint;
	static bool leftButtonPressed;
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::vec3 eye, center, up;
	static Material material;
	static int mouseMode;

	static bool initializeObjects();
	static void cleanUp();
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);
	static void displayCallback(GLFWwindow*);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void positionCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackBallMapping(glm::vec2 point);
};

#endif
