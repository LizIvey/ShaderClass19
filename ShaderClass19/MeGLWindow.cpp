#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>

using namespace std;
GLuint ColorMe;
glm::vec3 TriPos_1(-0.4f, 0.2f, 0.0f);
glm::vec3 TriPos_2(-0.4f, 0.2f, 0.0f);

/*
Assignment #3:

	1- Show a diamond that is our playing field. Make it take up the entire area.

	2- One shape, you choose, make it cool looking. No more triangle shape.

	3- Give the shape a random velocity...something reasonable

	4- Add a QTimer to get an automatic update function. See my Game Engine playlist on how to do this.

	5- Update your shape position every frame.*/

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
};

void MeGLWindow::sendDataToOpenGL()
{
	Vertex verts[] =
	{
		glm::vec3(0.8f, -0.3f, +0.0f),
		glm::vec3(+1.0f, +0.0f, +0.0f),

		glm::vec3(+0.6f, -0.6f, +0.0f),
		glm::vec3(+0.0f, +1.0f, +0.0f),

		glm::vec3(+0.9f,  -0.6f, +0.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),
	};

	GLuint VertBufferID;
	glGenBuffers(1, &VertBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);//color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 2));

	GLushort indices[] = {0,1,2}; //better way to generate multiple triangles with repeating verts (takes the verts locations & make triangle)
	GLuint IndexBufferID;
	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

bool MeGLWindow::checkProgramStatus(GLuint ColorMe)
{
	return checkStatus(ColorMe, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
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

	ColorMe = glCreateProgram();
	glAttachShader(ColorMe, vertexShaderID);
	glAttachShader(ColorMe, fragmentShaderID);
	glLinkProgram(ColorMe);

	if (!checkProgramStatus(ColorMe))
		return;

	glUseProgram(ColorMe);

}

void MeGLWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
}

void MeGLWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		TriPos_1.y += 0.4f;
		break;
	case Qt::Key::Key_S:
		TriPos_1.y -= 0.4f;
		break;
	case Qt::Key::Key_A:
		TriPos_1.x -= 0.4f;
		break;
	case Qt::Key::Key_D:
		TriPos_1.x += 0.4f;
		break;
	case Qt::Key::Key_Up:
		TriPos_2.y -= 0.4f;
		break;
	case Qt::Key::Key_Down:
		TriPos_2.y += 0.4f;
		break;
	case Qt::Key::Key_Left:
		TriPos_2.x -= 0.4f;
		break;
	case Qt::Key::Key_Right:
		TriPos_2.x += 0.4f;
		break;
	}
	repaint();
}

void MeGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	glm::vec4 UniformColor(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 offset(TriPos_1.x, TriPos_1.y, 0);

	GLint UniformColorLoc = glGetUniformLocation(ColorMe, "Color");
	GLint offsetUniformLoc = glGetUniformLocation(ColorMe, "Offset");
	GLint UniformYFlipLoc = glGetUniformLocation(ColorMe, "yflip");

	//Triangle 1
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	glUniform3fv(offsetUniformLoc, 1, &offset[0]);
	glUniform1f(UniformYFlipLoc, 1.0f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0); //draw indices info 

	//Triangle 2
	offset.x = TriPos_2.x;
	offset.y = TriPos_2.y;
	UniformColor.g = 1;
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	glUniform3fv(offsetUniformLoc, 1, &offset[0]);
	glUniform1f(UniformYFlipLoc, -1.0f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(ColorMe);
}

