#include "Track.h"

Track::Track()
{
    std::string filename = "objs/sphere.obj";
    std::ifstream objFile(filename); // The obj file we are reading.
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec3> faceIndices;
    
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
            else if (label == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                
                normals.push_back(normal);
            }
            else if (label == "f") {
                glm::ivec3 indices;
                std::string v1IndexStr, v2IndexStr, v3IndexStr;
                
                ss >> v1IndexStr >> v2IndexStr >> v3IndexStr;
                indices.x = atoi(v1IndexStr.c_str()) - 1;
                indices.y = atoi(v2IndexStr.c_str()) - 1;
                indices.z = atoi(v3IndexStr.c_str()) - 1;
                
                faceIndices.push_back(indices);
            }
        }
    }
    else
    {
        std::cerr << "Can't open the file " << filename << std::endl;
    }
    
    objFile.close();
    
    // Find center point of model
    float minX = points[0].x, minY = points[0].y, minZ = points[0].z;
    float maxX = points[0].x, maxY = points[0].y, maxZ = points[0].z;
    
    for (std::vector<glm::vec3>::size_type i = 1; i < points.size(); i++)
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
    
    // Find max distance away from center
    float avgX = (maxX - minX) / 2, avgY = (maxY - minY) / 2, avgZ = (maxZ - minZ) / 2;
    float maxDist;
    
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
    for (std::vector<glm::vec3>::size_type i = 0; i < points.size(); i++)
    {
        points[i].x -= (maxX + minX) / 2;
        points[i].x *= 7.5f / maxDist;
        points[i].y -= (maxY + minY) / 2;
        points[i].y *= 7.5f / maxDist;
        points[i].z -= (maxZ + minZ) / 2;
        points[i].z *= 7.5f / maxDist;
    }
    
    indicesNum = faceIndices.size() * 3;
    
    // Model matrix.
    C = glm::mat4(1.0f);
    
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbos);
    
    // Bind to the VAO.
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
                 points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
                 normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * faceIndices.size(),
                 faceIndices.data(), GL_STATIC_DRAW);
    
    // Unbind from the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind from the VAO.
    glBindVertexArray(0);
    
    printf("Finished Track\n");
}

Track::~Track()
{
    // Delete the VBOs, EBO, and VAO.
    glDeleteBuffers(2, vbos);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    
    glDeleteProgram(getShaderProgram());
}

void Track::draw(glm::mat4 C)
{
    this->C = C;
    for (BezierCurve* curve: curves)
    {
        curve->draw(C);
        
        // anchor point
        glm::mat4 model = glm::translate(curve->p[0]) * glm::scale(glm::vec3(0.01));
        glUseProgram(getShaderProgram());
        glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(getShaderProgram(), "color"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // control point 1
        model = glm::translate(curve->p[1]) * glm::scale(glm::vec3(0.01));
        glUseProgram(getShaderProgram());
        glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(getShaderProgram(), "color"), 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        // control point 2
        model = glm::translate(curve->p[2]) * glm::scale(glm::vec3(0.01));
        glUseProgram(getShaderProgram());
        glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(getShaderProgram(), "color"), 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // control handle
    glUseProgram(getShaderProgram());
    glUniformMatrix4fv(glGetUniformLocation(getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(this->C));
    glUniform3fv(glGetUniformLocation(getShaderProgram(), "color"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.0)));
    
    glBindVertexArray(lineVao);
    glDrawArrays(GL_LINES, 0, curves.size() * 2);
    glBindVertexArray(0);
}

void Track::update()
{
}

void Track::setCurves(std::vector<BezierCurve*> curves)
{
    this->curves = curves;
    std::vector<glm::vec3> points;
    
    for (BezierCurve* curve: this->curves)
    {
        curve->setShaderProgram(getShaderProgram());
        points.push_back(curve->p[1]);
        points.push_back(curve->p[2]);
    }
    std::rotate(points.rbegin(), points.rbegin() + 1, points.rend());
    
    glGenVertexArrays(1, &lineVao);
    glGenBuffers(1, &lineVbo);
    
    glBindVertexArray(lineVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
                 points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Track::movePoint(int num, glm::vec3 translate)
{
    BezierCurve* curve1 = curves[num / 3];
    std::cout << "first curve " << num / 3 << std::endl;
    int curveIndex = num / 3 - 1;
    if (curveIndex < 0)
    {
        curveIndex = curves.size() - 1;
    }
    if (num % 3 == 2)
    {
        curveIndex = num / 3 + 1;
        if (curveIndex > 7)
        {
            curveIndex = 0;
        }
    }
    BezierCurve* curve2 = curves[curveIndex];
    std::cout << "second curve " << curveIndex << std::endl;
    if (num % 3 == 0)  // anchor point
    {
        curve1->p[0] += translate;
        curve1->p[1] += translate;
        curve2->p[2] += translate;
        curve2->p[3] += translate;
    }
    else               // control point
    {
        if (num % 3 == 1)
        {
            std::cout << "1 " << num << std::endl;
            curve1->p[1] += translate;
            curve2->p[2] -= translate;
        }
        else
        {
            std::cout << num << std::endl;
            curve1->p[2] += translate;
            curve2->p[1] -= translate;
        }
    }
    curve1->updateCoeff();
    curve2->updateCoeff();
    
    std::vector<glm::vec3> points;
    
    for (BezierCurve* curve: this->curves)
    {
        curve->setShaderProgram(getShaderProgram());
        points.push_back(curve->p[1]);
        points.push_back(curve->p[2]);
    }
    std::rotate(points.rbegin(), points.rbegin() + 1, points.rend());
    
    glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
                 points.data(), GL_STATIC_DRAW);
}
