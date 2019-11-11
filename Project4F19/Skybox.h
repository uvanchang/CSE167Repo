#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Node.h"

class Skybox : public Node
{
private:
    GLuint vao;
    GLuint vbo;
public:
    Skybox(std::vector<std::string> facesFilenames);
    ~Skybox();
    void draw(glm::mat4 C);
    void update();
    static GLuint cubemapTexture;
};

#endif
