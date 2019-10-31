#include "Window.h"

int Window::width;
int Window::height;

const char* Window::windowTitle = "GLFW Starter Project";

Transform* Window::world;
Transform* Window::robot;

GLuint Window::uboMatrices;

glm::vec3 Window::curPoint;
glm::vec3 Window::lastPoint;
bool Window::leftButtonPressed;

int Window::mouseMode;

glm::mat4 Window::projection; // Projection matrix.
double Window::fov = glm::radians(60.0);

glm::vec3 Window::eye(225, 50, 225); // Camera position.
glm::vec3 Window::center(0, 0, 0); // The point we are looking at.
glm::vec3 Window::up(0, 1, 0); // The up direction of the camera.

glm::vec3 lightPos(-6.0f, 6.0f, 6.0f);

// View matrix, defined by eye, center and up.
glm::mat4 Window::view = glm::lookAt(Window::eye, Window::center, Window::up);

bool normalColoring = false;

bool Window::initializeObjects()
{
    world = new Transform(glm::mat4(1.0));
    
    GLuint shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
    robot = new Transform(glm::mat4(1.0), shaderProgram);
    
    GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, "Matrices");
    glUniformBlockBinding(shaderProgram, uniformBlockIndex, 0);
    
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            Transform* transform = new Transform(glm::translate(glm::vec3(50 * i, 0, 50 * j)));
            transform->addChild(robot);
            world->addChild(transform);
        }
    }
    
    Transform* body = new Transform(glm::translate(glm::vec3(0, 0, 0)));
    Transform* head = new Transform(glm::translate(glm::vec3(0, 11.25, 0)));
    Transform* leftEye = new Transform(glm::translate(glm::vec3(2.5, -1, 6.5)) * glm::scale(glm::vec3(0.1)));
    Transform* rightEye = new Transform(glm::translate(glm::vec3(-2.5, -1, 6.5)) * glm::scale(glm::vec3(0.1)));
    Transform* leftArm = new Transform(glm::translate(glm::vec3(11, 0, 0)) * glm::rotate(glm::radians(10.0f), glm::vec3(0, 0, 1)), 0, 1);
    Transform* rightArm = new Transform(glm::translate(glm::vec3(-11, 0, 0)) * glm::rotate(-glm::radians(10.0f), glm::vec3(0, 0, 1)), 0, 2);
    Transform* leftLeg = new Transform(glm::scale(glm::vec3(1, 0.5, 1)) * glm::translate(glm::vec3(4, -23, 0)), 0, 3);
    Transform* rightLeg = new Transform(glm::scale(glm::vec3(1, 0.5, 1)) * glm::translate(glm::vec3(-4, -23, 0)), 0, 4);
    
    leftArm->setMoveDir(1);
    rightArm->setMoveDir(-1);
    leftLeg->setMoveDir(-1);
    rightLeg->setMoveDir(1);
    
    robot->addChild(body);
    robot->addChild(head);
    robot->addChild(leftArm);
    robot->addChild(rightArm);
    robot->addChild(leftLeg);
    robot->addChild(rightLeg);
    
    head->addChild(leftEye);
    head->addChild(rightEye);
    
    Geometry* bodyGeo = new Geometry("objs/body_s.obj");
    Geometry* headGeo = new Geometry("objs/head_s.obj");
    Geometry* eyeGeo = new Geometry("objs/eyeball_s.obj");
    Geometry* limbGeo = new Geometry("objs/limb_s.obj");
    
    body->addChild(bodyGeo);
    head->addChild(headGeo);
    leftEye->addChild(eyeGeo);
    rightEye->addChild(eyeGeo);
    leftArm->addChild(limbGeo);
    rightArm->addChild(limbGeo);
    leftLeg->addChild(limbGeo);
    rightLeg->addChild(limbGeo);
    
    mouseMode = 1;
    
    glGenBuffers(1, &uboMatrices);
    
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
      
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    
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
    Window::projection = glm::perspective(fov, double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback()
{
    robot->update();
}

void Window::displayCallback(GLFWwindow* window)
{	
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    world->draw(glm::mat4(1.0));
    
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
        glm::vec3 direction = curPoint - lastPoint;
        float velocity = glm::length(direction);
        
        glm::vec3 rotationAxis = glm::cross(lastPoint, curPoint);
        float rotationAngle = velocity;
        
        center -= eye;
        center = rotate(center, rotationAngle, rotationAxis);
        center += eye;
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
