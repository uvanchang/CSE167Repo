#include "Window.h"

int Window::width;
int Window::height;

const char* Window::windowTitle = "GLFW Starter Project";

// Objects to display.
Cube* Window::cube;
PointCloud* Window::cubePoints;
PointCloud* Window::bunnyPoints;
PointCloud* Window::dragonPoints;
PointCloud* Window::bearPoints;

// The object currently displaying.
Object* Window::currentObj; 

GLfloat Window::currentSize;

glm::mat4 Window::projection; // Projection matrix.

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

GLuint Window::program; // The shader program id.

GLuint Window::projectionLoc; // Location of projection in shader.
GLuint Window::viewLoc; // Location of view in shader.
GLuint Window::modelLoc; // Location of model in shader.
GLuint Window::colorLoc; // Location of color in shader.

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
	glUseProgram(program);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "color");

	return true;
}

bool Window::initializeObjects()
{
	// Set currentSize to start at 10
	currentSize = 10;

	// Initialzie PointClouds to 3 obj files.
	bunnyPoints = new PointCloud("bunny", objFileToPoints("bunny.obj"), currentSize);
	dragonPoints = new PointCloud("dragon", objFileToPoints("dragon.obj"), currentSize);
	bearPoints = new PointCloud("bear", objFileToPoints("bear.obj"), currentSize);

	// Set bunnyPoints to be the first object to appear.
	currentObj = bunnyPoints;
	return true;
}

std::vector<glm::vec3> Window::objFileToPoints(std::string fileName)
{
	std::ifstream objFile(fileName); // The obj file we are reading.
	std::vector<glm::vec3> points;

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point.
				points.push_back(point);
			}
		}
	}
	else
	{
		std::cerr << "Can't open the file " << fileName << std::endl;
	}

	objFile.close();

	float minX = points[0].x, minY = points[0].y, minZ = points[0].z;
	float maxX = points[0].x, maxY = points[0].y, maxZ = points[0].z;

	for (int i = 1; i < points.size(); i++)
	{
		if (points[i].x < minX)
		{
			minX = points[i].x;
		}
		if (points[i].y < minY)
		{
			minY = points[i].y;
		}
		if (points[i].z < minZ)
		{
			minZ = points[i].z;
		}
		if (points[i].x > maxX)
		{
			maxX = points[i].x;
		}
		if (points[i].y > maxY)
		{
			maxY = points[i].y;
		}
		if (points[i].z > maxZ)
		{
			maxZ = points[i].z;
		}
	}
	
	std::cout << minX << " " << maxX << " " << (maxX - minX) / 2 << std::endl;
	std::cout << minY << " " << maxY << " " << (maxY - minY) / 2 << std::endl;
	std::cout << minZ << " " << maxZ << " " << (maxZ - minZ) / 2 << std::endl;

	for (int i = 0; i < points.size(); i++)
	{
		points[i].x -= (maxX + minX) / 2;
		points[i].y -= (maxY + minY) / 2;
		points[i].z -= (maxZ + minZ) / 2;
	}

	return points;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cube;
	delete cubePoints;

	// Delete the shader program.
	glDeleteProgram(program);
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

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	currentObj->update();
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Specify the values of the uniform variables we are going to use.
	glm::mat4 model = currentObj->getModel();
	glm::vec3 color = currentObj->getColor();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(colorLoc, 1, glm::value_ptr(color));

	// Render the object.
	currentObj->draw();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;
		case GLFW_KEY_F1:
			// Set currentObj to bunnyPoints.
			currentObj = bunnyPoints;
			break;
		case GLFW_KEY_F2:
			// Set currentObj to dragonPoints.
			currentObj = dragonPoints;
			break;
		case GLFW_KEY_F3:
			// Set currentObj to bearPoints.
			currentObj = bearPoints;
			break;
		case GLFW_KEY_P:
			if (mods == GLFW_MOD_SHIFT) // Make currentObj point size bigger.
			{
				currentSize++;
				std::cout << "P pressed" << std::endl;
			}
			else                        // Make currentObj point size smaller.
			{
				currentSize--;
				std::cout << "p pressed" << std::endl;
			}
			std::cout << currentSize << std::endl;
			((PointCloud*)currentObj)->updatePointSize(currentSize);
			break;
		default:
			break;
		}
	}
}
