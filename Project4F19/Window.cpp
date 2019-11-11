#include "Window.h"

int Window::width;
int Window::height;

GLFWwindow* Window::window;

const std::string Window::windowTitle = "GLFW Starter Project";

Transform* Window::world;
Transform* Window::skybox;

glm::vec3 Window::curPoint;
glm::vec3 Window::lastPoint;
bool Window::leftButtonPressed;

glm::mat4 Window::projection; // Projection matrix.
double Window::fov = glm::radians(60.0);

glm::vec3 Window::eye(0, 0, 20); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

float Window::hNear = 2 * tan(Window::fov / 2) * 1.0;
float Window::wNear = hNear * float(Window::width) / (float)Window::height;

std::vector<std::pair<glm::vec3, glm::vec3>> Window::frustumPlanes(6);

bool Window::initializeObjects()
{
    GLuint skyboxShader = LoadShaders("shaders/SkyboxShader.vert", "shaders/SkyboxShader.frag");
    skybox = new Transform(glm::translate(eye), skyboxShader);
    
    Skybox* skyboxObj = new Skybox({"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"});

    skybox->addChild(skyboxObj);
    
    world = new Transform(glm::translate(glm::vec3(0)));
    
    GLuint sphereShader = LoadShaders("shaders/SphereShader.vert", "shaders/SphereShader.frag");
    
    Transform* sphere = new Transform(glm::translate(glm::vec3(0.0)), sphereShader);
    
    world->addChild(sphere);
    
    Sphere* sphereGeo = new Sphere("objs/sphere.obj");
    
    sphere->addChild(sphereGeo);
    
    glUseProgram(sphere->getShaderProgram());
    glUniform1i(glGetUniformLocation(sphere->getShaderProgram(), "skybox"), 0);
    
    glUseProgram(skybox->getShaderProgram());
    glUniform1i(glGetUniformLocation(skybox->getShaderProgram(), "skybox"), 0);
    
    return true;
}

void Window::cleanUp()
{
    // Deallcoate the objects.
    delete world;
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
    window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
    
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
    Window::projection = glm::perspective(fov, double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback()
{
}

void Window::displayCallback(GLFWwindow* window)
{	
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    world->draw(glm::mat4(1));
    
    skybox->draw(glm::mat4(1));
    
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
        glm::vec3 direction = curPoint - lastPoint;
        float velocity = glm::length(direction);
        
        glm::vec3 rotationAxis = glm::cross(lastPoint, curPoint);
        float rotationAngle = velocity;
        
        center -= eye;
        center = rotate(center, rotationAngle, rotationAxis);
        center += eye;
        //eye = rotate(eye, rotationAngle, rotationAxis);
    }
    view = glm::lookAt(Window::eye, Window::center, Window::up);
    
    lastPoint = curPoint;
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            leftButtonPressed = true;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            leftButtonPressed = false;
        }
    }
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        fov -= glm::radians(0.5);
    }
    else
    {
        fov += glm::radians(0.5);
    }
    
    projection = glm::perspective(fov, double(width) / (double)height, 1.0, 1000.0);
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
