#include "Model.h"

Model::Model(std::string fileName)
{
	std::ifstream objFile(fileName); // The obj file we are reading.
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> faceIndices;

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		this->fileName = fileName;
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
		std::cerr << "Can't open the file " << fileName << std::endl;
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
	model = glm::mat4(1.0f);

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

	ID = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	printf("Finished %s\n", fileName.c_str());
}

Model::~Model()
{
	// Delete the VBOs, EBO, and VAO.
	glDeleteBuffers(2, vbos);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	glDeleteProgram(ID);
}

void Model::draw()
{
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Draw triangles using the indices in the second VBO, which is an 
	// element array buffer.
	glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void Model::rotate(glm::vec3 lastPoint, glm::vec3 curPoint)
{
	glm::vec3 direction = curPoint - lastPoint;
	float velocity = glm::length(direction);

	glm::vec3 rotationAxis = glm::cross(lastPoint, curPoint);
	float rotationAngle = velocity;

	model = glm::rotate(model, rotationAngle, rotationAxis);
}

void Model::changeSize(double offset)
{
	model = glm::scale(model, glm::vec3(1 - ((float) offset) * 0.05f));
}

void Model::translate(glm::vec3 pos) {
	model = glm::translate(model, pos);
}

void Model::scale(float num) {
	model = glm::scale(model, glm::vec3(num));
}