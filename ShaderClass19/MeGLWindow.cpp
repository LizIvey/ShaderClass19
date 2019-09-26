#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>
#include <math.h>
#include <Vector2D.h>

using namespace std;

GLuint MyShaders;
glm::vec2 Pos_1(0.0f, 0.0f);
glm::vec2 BallPosition(0.0f, 0.0f);
int debugCount = 0;

/*
Assignment #3:

	1- Show a diamond that is our playing field. Make it take up the entire area. -> DONE!

	2- One shape, you choose, make it cool looking. No more triangle shape. -> DONE!

	3- Give the shape a random velocity...something reasonable ->

	4- Add a QTimer to get an automatic update function. See my Game Engine playlist on how to do this. ->

	5- Update your shape position every frame. -> */

struct Vertex
{
	glm::vec2 position;
	glm::vec3 color;
};

void MeGLWindow::DrawDiamond()
{
	Vertex Diamond[] =
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec2(0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec2(-1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
	};

	GLuint vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Diamond), Diamond, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0,1,2,3 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

Vertex Ball[] =
{
	glm::vec2(0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),

	glm::vec2(0.1f, -0.1f),
	glm::vec3(0.0f, 1.0f, 0.0f),

	glm::vec2(-0.1f, -0.1f),
	glm::vec3(0.0f, 1.0f, 0.0f),
};

const unsigned int NUM_VERTS = sizeof(Ball) / sizeof(*Ball);

void DrawBALL()
{
	GLuint vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Ball), Ball, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0,1,2};
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
}

void DrawBall(glm::vec2 CirclePos, float r, int LineSeg)
{
	glBegin(GL_TRIANGLE_FAN); 
	for (int i = 0; i < LineSeg; i++)
	{
		float PI = 3.1415926f;
		float theta = 2.0f * PI * float(i) / float(LineSeg); //get the current

		float XX = r * cos(theta); //calculate x
		float YY = r * sin(theta); //calulate y

		glVertex2f(XX + CirclePos.x, YY + CirclePos.y); //output verts
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
	//DrawBall(glm::vec2(0.0f, 0.0f), 0.2, 10);
	//DrawDiamond();

	//setting up Qt Timer
	connect(&timer, SIGNAL(timeout()), this, SLOT(BallUpdate()));
	timer.start(0);
}

void MeGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	glm::vec4 UniformColor(0.0f, 1.0f, 1.0f, 1.0f);
	glm::vec2 UniformPosition(Pos_1.x, Pos_1.y);

	GLint UniformColorLoc = glGetUniformLocation(MyShaders, "Color");//uniform color
	GLint UniformPositionLoc = glGetUniformLocation(MyShaders, "Pos");//uniform position

	//Step 1: uniform position and color of the diamond0
	glUniform2fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawDiamond();
	glDrawElements(GL_LINE_LOOP, 5, GL_UNSIGNED_SHORT, 0);

	

	//Step 2: I am going to draw a circle @ the origin
	UniformColor.g = 0;
	UniformColor.r = 1;
	glUniform2fv(UniformPositionLoc, 1, &UniformPosition[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawBall(glm::vec2(0.0f, 0.0f), 0.1f, 20);
	//DrawBALL();
	//glDrawElements(GL_TRIANGLES, 5, GL_UNSIGNED_SHORT, 0);
}

void  MeGLWindow::BallUpdate()
{
	/*debugCount++;
	if (debugCount < 100 == 0)
	{
		cout << "Frame!" << debugCount << endl;
	}*/
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(MyShaders);
}

