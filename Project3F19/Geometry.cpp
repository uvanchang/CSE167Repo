#include "Geometry.h"
// bounding sphere radius = 2.313938
Geometry::Geometry(std::string filename)
{
    std::ifstream objFile(filename); // The obj file we are reading.
    std::vector<glm::vec3> inputVertices;
    std::vector<glm::vec3> inputNormals;
    std::vector<int> vertexIndices;
    std::vector<int> normalIndices;
    
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
                inputVertices.push_back(point);
            }
            else if (label == "vn")
            {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                
                inputNormals.push_back(normal);
            }
            else if (label == "f")
            {
                std::string vertex1, vertex2, vertex3;
                int v, vt, vn;
                char slash;
                
                ss >> vertex1 >> vertex2 >> vertex3;

                std::stringstream first;
                first << vertex1;
                first >> v >> slash >> vt >> slash >> vn;
                v--;
                vt--;
                vn--;
                
                vertexIndices.push_back(v);
                normalIndices.push_back(vn);
                
                std::stringstream second;
                second << vertex2;
                second >> v >> slash >> vt >> slash >> vn;
                v--;
                vt--;
                vn--;
                
                vertexIndices.push_back(v);
                normalIndices.push_back(vn);
                
                std::stringstream third;
                third << vertex3;
                third >> v >> slash >> vt >> slash >> vn;
                v--;
                vt--;
                vn--;
                
                vertexIndices.push_back(v);
                normalIndices.push_back(vn);
            }
        }
    }
    else
    {
        std::cerr << "Can't open the file " << filename << std::endl;
    }
    
    objFile.close();
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<int> indices;
    
    for (unsigned i = 0; i < vertexIndices.size(); i++)
    {
      vertices.push_back(inputVertices[vertexIndices[i]]);
      normals.push_back(inputNormals[normalIndices[i]]);
      indices.push_back(i);
    }
    
    // Find center point of model
    float minX = vertices[0].x, minY = vertices[0].y, minZ = vertices[0].z;
    float maxX = vertices[0].x, maxY = vertices[0].y, maxZ = vertices[0].z;
    
    for (std::vector<glm::vec3>::size_type i = 1; i < vertices.size(); i++)
    {
        if (vertices[i].x < minX)
        {
            minX = vertices[i].x;
        }
        if (vertices[i].y < minY)
        {
            minY = vertices[i].y;
        }
        if (vertices[i].z < minZ)
        {
            minZ = vertices[i].z;
        }
        if (vertices[i].x > maxX)
        {
            maxX = vertices[i].x;
        }
        if (vertices[i].y > maxY)
        {
            maxY = vertices[i].y;
        }
        if (vertices[i].z > maxZ)
        {
            maxZ = vertices[i].z;
        }
    }
    
    // Find max distance away from center
    float avgX = (maxX - minX) / 2, avgY = (maxY - minY) / 2, avgZ = (maxZ - minZ) / 2;
    float maxDist = 0;
    
    if (avgX >= avgY && avgX >= avgZ)
    {
        maxDist = avgX;
    }
    else if (avgY >= avgX && avgY >= avgZ)
    {
        maxDist = avgY;
    }
    else if (avgZ >= avgX && avgZ >= avgY)
    {
        maxDist = avgZ;
    }
    
    // Normalizing scale
    for (std::vector<glm::vec3>::size_type i = 0; i < vertices.size(); i++)
    {
        vertices[i].x -= (maxX + minX) / 2;
        vertices[i].x *= 7.5f / maxDist;
        vertices[i].y -= (maxY + minY) / 2;
        vertices[i].y *= 7.5f / maxDist;
        vertices[i].z -= (maxZ + minZ) / 2;
        vertices[i].z *= 7.5f / maxDist;
    }
    
    indicesNum = vertexIndices.size();
    
    // Model matrix.
    C = glm::mat4(1.0f);
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbos);
    
    // Bind to the VAO.
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                 vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
                 normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
    
    // Unbind from the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind from the VAO.
    glBindVertexArray(0);
    
    printf("Finished %s\n", filename.c_str());
}

Geometry::~Geometry()
{
    // Delete the VBOs, EBO, and VAO.
    glDeleteBuffers(2, vbos);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    
    glDeleteProgram(getShaderProgram());
}

int Geometry::draw(glm::mat4 C, std::vector<std::pair<glm::vec3, glm::vec3>> frustumPlanes)
{
    this->C = C;
    
    glUseProgram(getShaderProgram());
    glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(this->C));
    
    // Bind to the VAO.
    glBindVertexArray(vao);
    // Draw triangles using the indices in the second VBO, which is an
    // element array buffer.
    glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
    // Unbind from the VAO.
    glBindVertexArray(0);
    
    return 0;
}

void Geometry::update()
{
}
