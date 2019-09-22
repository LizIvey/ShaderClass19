#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>
#include <math.h>

using namespace std;
GLuint MyShaders;
glm::vec3 Pos_1(0.0f, 0.0f, 0.0f);

/*
Assignment #3:

	1- Show a diamond that is our playing field. Make it take up the entire area. -> DONE!

	2- One shape, you choose, make it cool looking. No more triangle shape. -> DONE!

	3- Give the shape a random velocity...something reasonable ->

	4- Add a QTimer to get an automatic update function. See my Game Engine playlist on how to do this. ->

	5- Update your shape position every frame. -> */

void MeGLWindow::DrawDiamond()
{
	/*glBegin: Specifies the primitive or primitives that will be created from vertices presented between glBegin 
	and the subsequent glEnd. Ten symbolic constants are accepted: GL_POINTS, GL_LINES, GL_LINE_STRIP,
	GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON.*/

	/*GL_LINE_LOOP: Draws a connected group of line segments from the first vertex to the last, then back to the first. 
	Vertices n and n + 1 define line n. The last line, however, is defined by vertices N and 1 . N lines are drawn.*/
	glBegin(GL_LINE_LOOP); 
	glVertex2f(0.0f, 1.0f); 
	glVertex2f(1.0f, 0.0f); 
	glVertex2f(0.0f, -1.0f); 
	glVertex2f(-1.0f, 0.0f); 
	glEnd();//end of lines
}

void MeGLWindow::DrawBall(float x, float y, float r, int LineSeg)
{
	glBegin(GL_POLYGON); //Draws a single, convex polygon. Vertices 1 through N define this polygon.
	for (int i = 0; i < LineSeg; i++)
	{
		float PI = 3.1415926f;
		float theta = 2.0f * PI * float(i) / float(LineSeg); //get the current

		float XX = r * cos(theta); //calculate x
		float YY = r * sin(theta); //calulate y

		glVertex2f(XX + x, YY + y); //output verts
	}
	glEnd(); //end of polygon
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
	//sendDataToOpenGL();
	installShaders();
	DrawBall(0.0f, 0.0f, 0.2, 10);
	DrawDiamond();

	//setting up Qt Timer
	connect(&timer, SIGNAL(timeout()), this, SLOT(BallUpdate()));
	timer.start(0);
}

void MeGLWindow::paintGL()
{
	float theta;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	glm::vec4 UniformColor(0.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 UniformPosition(Pos_1.x, Pos_1.y, 0);

	GLint UniformColorLoc = glGetUniformLocation(MyShaders, "Color");//uniform color
	GLint UniformPositionLoc = glGetUniformLocation(MyShaders, "Pos");//uniform position

	//Step 1: uniform position and color of the diamond
	glUniform3fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawDiamond();

	//Step 2: I am going to draw a circle @ the origin
	UniformColor.g = 0;
	UniformColor.r = 1;
	glUniform3fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawBall(0.0f, 0.0f, 0.1f, 30);
}

void  MeGLWindow::BallUpdate()
{

}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(MyShaders);
}

