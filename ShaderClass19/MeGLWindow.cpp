#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <math.h>
#include <stdlib.h>
#include <C:\Users\krazi\Desktop\Tech Art III\ShaderClass19\Middleware\glm\glm\gtx\normal.hpp>
#include <C:\Users\krazi\Desktop\Tech Art III\ShaderClass19\Middleware\glm\glm\gtx\transform.hpp>
#include <Primitives/ShapeData.h>
#include <Primitives/ShapeGenerator.h>

/*rotating cube hmwk!*/

using namespace std;
using glm::vec3;
using glm::mat4;

GLuint MyShaders;
GLint NumIndices;

const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

void MeGLWindow::DrawCube()
{
	ShapeData Cube = ShapeGenerator::makeCube();

	GLuint CubeBufferID;

	glGenBuffers(1, &CubeBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, CubeBufferID);
	glBufferData(GL_ARRAY_BUFFER, Cube.vertexBufferSize(), Cube.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 2));

	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Cube.indexBufferSize(), Cube.indices, GL_STATIC_DRAW);
	NumIndices = Cube.numIndices;
	Cube.cleanup();
}

bool checkStatus(GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer;

		delete[] buffer;

		return false;
	}

	return true;
}

bool MeGLWindow::checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool MeGLWindow::checkProgramStatus(GLuint MyShaders)
{
	return checkStatus(MyShaders, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string MeGLWindow::readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load... " << fileName;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void MeGLWindow::installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	MyShaders = glCreateProgram();
	glAttachShader(MyShaders, vertexShaderID);
	glAttachShader(MyShaders, fragmentShaderID);
	glLinkProgram(MyShaders);

	if (!checkProgramStatus(MyShaders))
		return;

	glUseProgram(MyShaders);
}

void MeGLWindow::initializeGL()
{
	glewInit();
	installShaders();
	DrawCube();
	glEnable(GL_DEPTH_TEST); //enabling the depth test; by default this is turned off
}

void MeGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);//clears the depth buffer and the color buffer
	glViewport(0, 0, width(), height());

	//creating & initlizing the ProjectionMatrix & the ModelTransformMatrix
	mat4 TranslationMat = glm::translate(mat4(), vec3(0.0f, 0.0f, -3.0f));
	mat4 RotationMat = glm::rotate(mat4(), 54.0f, vec3(1.0f, 0.0f, 0.0f));
	mat4 ProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);

	mat4 FullTransformationMat = ProjectionMatrix * TranslationMat * RotationMat;

	//draw cube
	//feeding the ModelTransformMatrix & ProjectionTransformMatrix into the vertex shader
	GLint FullTransformMatUnitformLoc = glGetUniformLocation(MyShaders, "FullTransformMatrix");
	glUniformMatrix4fv(FullTransformMatUnitformLoc, 1, GL_FALSE, &FullTransformationMat[0][0]);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_SHORT, 0);
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0); 
	glDeleteProgram(MyShaders);
}