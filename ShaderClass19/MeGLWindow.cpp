#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>
#include <math.h>

using namespace std;
GLuint ColorMe;
glm::vec3 TriPos_1(0.0f, 0.0f, 0.0f);
glm::vec3 TriPos_2(0.0f, 0.0f, 0.0f);

/*
Assignment #3:

	1- Show a diamond that is our playing field. Make it take up the entire area. -> DONE!

	2- One shape, you choose, make it cool looking. No more triangle shape. 

	3- Give the shape a random velocity...something reasonable

	4- Add a QTimer to get an automatic update function. See my Game Engine playlist on how to do this.

	5- Update your shape position every frame.*/

void MeGLWindow::DrawDiamond()
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0f, 1.0f); 
	glVertex2f(1.0f, 0.0f); 
	glVertex2f(0.0f, -1.0f); 
	glVertex2f(-1.0f, 0.0f); 
	glEnd();
}

void MeGLWindow::DrawCircle(float x, float y, float r, int LineSeg)
{
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < LineSeg; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(LineSeg); //get the current

		float XX = r * cos(theta); //calculate x
		float YY = r * sin(theta); //calulate y

		glVertex2f(XX + x, YY + y); //output verts
	}
	glEnd();
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
	//sendDataToOpenGL();
	installShaders();
	DrawCircle(0.0f, 0.0f, 0.2, 10);
	DrawDiamond();
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
	float theta;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	glm::vec4 UniformColor(0.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 UniformPosition(TriPos_1.x, TriPos_1.y, 0);

	GLint UniformColorLoc = glGetUniformLocation(ColorMe, "Color");
	GLint UniformPositionLoc = glGetUniformLocation(ColorMe, "Offset");

	//Step 1: uniform position and color of the diamond
	glUniform3fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawDiamond();

	//Step 2: I am going to draw a circle @ the origin
	UniformColor.g = 0;
	UniformColor.r = 1;
	glUniform3fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawCircle(0.0f, 0.0f, 0.1f, 30);
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(ColorMe);
}

