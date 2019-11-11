#include "Skybox.h"
#include "Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint Skybox::cubemapTexture;

Skybox::Skybox(std::vector<std::string> facesFilenames)
{
    // code adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < facesFilenames.size(); i++)
    {
        unsigned char *data = stbi_load(facesFilenames[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                         );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << facesFilenames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    float skyboxVertices[] = {
        // positions
        -500.0f,  500.0f, -500.0f,
        -500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        
        -500.0f, -500.0f,  500.0f,
        -500.0f, -500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f,  500.0f,
        -500.0f, -500.0f,  500.0f,
        
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        
        -500.0f, -500.0f,  500.0f,
        -500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f, -500.0f,  500.0f,
        -500.0f, -500.0f,  500.0f,
        
        -500.0f,  500.0f, -500.0f,
        500.0f,  500.0f, -500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        -500.0f,  500.0f,  500.0f,
        -500.0f,  500.0f, -500.0f,
        
        -500.0f, -500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f,
        500.0f, -500.0f,  500.0f
    };
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    printf("Finished Skybox\n");
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    glDeleteProgram(getShaderProgram());
}

void Skybox::draw(glm::mat4 C)
{
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    glUseProgram(getShaderProgram());
    glm::mat4 view = glm::mat4(glm::mat3(Window::view)); // remove translation from the view matrix
    glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(Window::projection));
    
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::update()
{
}
