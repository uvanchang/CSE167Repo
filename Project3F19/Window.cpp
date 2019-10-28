#include "Window.h"

int Window::width;
int Window::height;

const char* Window::windowTitle = "GLFW Starter Project";

// Objects to display.
Model* Window::bunny;
Model* Window::dragon;
Model* Window::bear;

Light* Window::light;

// The object currently displaying.
Object* Window::currentObj; 

glm::vec3 Window::curPoint;
glm::vec3 Window::lastPoint;
bool Window::leftButtonPressed;

int Window::mouseMode;

glm::mat4 Window::projection; // Projection matrix.

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

glm::vec3 lightPos(-6.0f, 6.0f, 6.0f);

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

bool normalColoring = false;

bool Window::initializeObjects()
{
	// Initialize Models from 3 obj files.
	bunny = new Model("bunny.obj");
	dragon = new Model("dragon.obj");
	bear = new Model("bear.obj");

	light = new Light("sphere.obj", lightPos);
	
	// Set bunny to be the first object to appear.
	currentObj = bunny;

	mouseMode = 1;

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete bunny;
	delete dragon;
	delete bear;
	delete light;
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(60.0), 
		double(width) / (double)height, 1.0, 1000.0);
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Specify the values of the uniform variables we are going to use.
	GLuint currentObjID = ((Model*)currentObj)->ID;
	glm::mat4 currentObjModel = currentObj->getModel();
	glUseProgram(currentObjID);
	glUniformMatrix4fv(glGetUniformLocation(currentObjID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(currentObjID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(currentObjID, "model"), 1, GL_FALSE, glm::value_ptr(currentObjModel));
	glUniform1i(glGetUniformLocation(currentObjID, "isLight"), 0);

	glUniform3fv(glGetUniformLocation(currentObjID, "viewPos"), 1, glm::value_ptr(eye));

	glUniform3fv(glGetUniformLocation(currentObjID, "light.position"), 1, glm::value_ptr(light->lightPos));
	glUniform3f(glGetUniformLocation(currentObjID, "light.ambient"), 0.84725f, 0.795f, 0.0745f);
	glUniform3f(glGetUniformLocation(currentObjID, "light.diffuse"), 0.75164f, 0.60648f, 0.22648f);
	glUniform3f(glGetUniformLocation(currentObjID, "light.specular"), 0.628281f, 0.555802f, 0.366065f);
	glUniform1f(glGetUniformLocation(currentObjID, "light.linear"), 0.09f);

	if (normalColoring) {
		glUniform1i(glGetUniformLocation(currentObjID, "normalColoring"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(currentObjID, "normalColoring"), 0);
		if (((Model*)currentObj)->fileName.compare("bunny.obj") == 0)
		{
			glUniform3f(glGetUniformLocation(currentObjID, "material.ambient"), 0.8215f, 0.1745f, 0.0215f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.diffuse"), 0.0f, 0.0f, 0.0f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.specular"), 0.833f, 0.827811f, 0.833f);
			glUniform1f(glGetUniformLocation(currentObjID, "material.shininess"), 128.0f);
		}
		else if (((Model*)currentObj)->fileName.compare("dragon.obj") == 0)
		{
			glUniform3f(glGetUniformLocation(currentObjID, "material.ambient"), 0.1745f, 0.8215f, 0.0215f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.diffuse"), 0.633f, 0.27811f, 0.533f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.specular"), 0.0f, 0.0f, 0.0f);
			glUniform1f(glGetUniformLocation(currentObjID, "material.shininess"), 128.0f);
		}
		else if (((Model*)currentObj)->fileName.compare("bear.obj") == 0)
		{
			glUniform3f(glGetUniformLocation(currentObjID, "material.ambient"), 0.2f, 0.2f, 0.9f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.diffuse"), 0.2343f, 0.342f, 0.3733f);
			glUniform3f(glGetUniformLocation(currentObjID, "material.specular"), 0.833f, 0.827811f, 0.833f);
			glUniform1f(glGetUniformLocation(currentObjID, "material.shininess"), 128.0f);
		}
	}

	// Render the object.
	currentObj->draw();

	GLuint lightID = light->ID;
	glm::mat4 lightModel = light->getModel();
	glUseProgram(lightID);
	glUniformMatrix4fv(glGetUniformLocation(lightID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform1i(glGetUniformLocation(lightID, "isLight"), 1);

	glUniform3fv(glGetUniformLocation(lightID, "light.position"), 1, glm::value_ptr(light->lightPos));
	glUniform3f(glGetUniformLocation(lightID, "light.ambient"), 0.84725f, 0.795f, 0.0745f);;

	// Render the light object.
	light->draw();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;
		case GLFW_KEY_1:
			mouseMode = 1;
			break;
		case GLFW_KEY_2:
			mouseMode = 2;
			break;
		case GLFW_KEY_3:
			mouseMode = 3;
			break;
		case GLFW_KEY_F1:
			// Set currentObj to bunnyPoints.
			currentObj = bunny;
			break;
		case GLFW_KEY_F2:
			// Set currentObj to dragonPoints.
			currentObj = dragon;
			break;
		case GLFW_KEY_F3:
			// Set currentObj to bearPoints.
			currentObj = bear;
			break;
		case GLFW_KEY_N:
			// Toggle between normal coloring and Phong model.
			normalColoring = !normalColoring;
			break;
		default:
			break;
		}
	}
}

void Window::positionCallback(GLFWwindow* window, double xpos, double ypos)
{
	curPoint = trackBallMapping(glm::vec2(xpos, ypos));

	if (leftButtonPressed)
	{
		switch (mouseMode)
		{
		case 1:
			currentObj->rotate(lastPoint, curPoint);
			break;
		case 2:
			light->rotate(lastPoint, curPoint);
			break;
		case 3:
			currentObj->rotate(lastPoint, curPoint);
			light->rotate(lastPoint, curPoint);
			break;
		}
	}

	lastPoint = curPoint;
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			leftButtonPressed = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			leftButtonPressed = false;
		}
	}
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	switch (mouseMode)
	{
	case 1:
		currentObj->changeSize(yoffset);
		break;
	case 2:
		light->changeDistance(yoffset);
		break;
	case 3:
		currentObj->changeSize(yoffset);
		light->changeDistance(yoffset);
		break;
	}
}

glm::vec3 Window::trackBallMapping(glm::vec2 point)
{
	glm::vec3 v;
	float d;

	v.x = (2.0f * point.x - width) / width;
	v.y = (height - 2.0f * point.y) / height;
	v.z = 0.0f;

	d = glm::length(v);

	d = (d < 1.0f) ? d : 1.0f;
	v.z = sqrtf(1.001f - d * d);

	v = glm::normalize(v);
	return v;
}