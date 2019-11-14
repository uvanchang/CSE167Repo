#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

#include "shader.h"
#include "Node.h"
#include "Transform.h"
#include "Geometry.h"
#include "Sphere.h"
#include "Skybox.h"
#include "BezierCurve.h"
#include "Track.h"

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
    static GLFWwindow* window;
    static const std::string windowTitle;
    static Transform* world;
    static Transform* skybox;
    static Track* track;
    static glm::vec3 curPoint;
    static glm::vec3 lastPoint;
    static bool leftButtonPressed;
    static glm::mat4 projection;
    static glm::mat4 view;
    static GLuint uboMatrices;
    static double fov;
    static glm::vec3 eye, center, up;
    static GLuint selectedPoint;
    static bool isPaused;
    static bool isRider;
    
    static bool initializeObjects();
    static void cleanUp();
    static GLFWwindow* createWindow(int width, int height);
    static void resizeCallback(GLFWwindow* window, int width, int height);
    static void idleCallback();
    static void displayCallback(GLFWwindow*);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void positionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static glm::vec3 trackBallMapping(glm::vec2 point);
};

#endif
