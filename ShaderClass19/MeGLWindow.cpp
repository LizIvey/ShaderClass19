#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>
#include <math.h>

using namespace std;

GLuint MyShaders;
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
	glm::vec3 position;
	glm::vec3 color;
};

namespace
{
	Vertex Ball[] =
	{
		glm::vec3(0.0f, 0.2f, 0.0f),//0
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(0.1f, 0.0f, 0.0f),//1
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(0.1f, -0.1f, 0.0f),//2
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(-0.1f, -0.1f, 0.0f),//3
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(-0.1f, 0.0f, 0.0f),//4
		glm::vec3(0.0f, 1.0f, 0.0f),
	};

	Vertex Diamond[] =
	{
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
	};

	const unsigned int NUM_VERTS = sizeof(Ball) / sizeof(*Ball);

	GLuint BoundVertexBufferID;
	GLuint BallvertexBufferID;

	glm::vec3 transformedPoints[NUM_VERTS];
	glm::vec3 Pos_1(0.0f, 0.0f, 0.0f);
	glm::vec3 Pos_2(0.0f, 0.0f, 0.0f);
}

void MeGLWindow::DrawDiamond()
{
	glGenBuffers(1, &BoundVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BoundVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Diamond), Diamond, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0,1,2,3 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void DrawBALL()
{
	glGenBuffers(1, &BallvertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BallvertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Ball), Ball, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)*6, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0,1,2,3,4 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
}

void DrawBall(glm::vec2 CirclePos, float r, int LineSeg)
{
	glBegin(GL_TRIANGLE_FAN); 
	for (int i = 0; i < 30; i++)
	{
		float PI = 3.1415926f;
		float theta = 2.0f * PI * float(i) / float(LineSeg); //get the current

		float XX = 0.1 * cos(theta); //calculate x
		float YY = 0.1 * sin(theta); //calulate y

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
	glm::vec3 TriPos(Pos_1.x, Pos_1.y, 0.0f);
	glm::vec3 BoundPos(Pos_2.x, Pos_2.y, 0.0f);

	GLint UniformColorLoc = glGetUniformLocation(MyShaders, "Color");//uniform color
	GLint UniformPositionLoc = glGetUniformLocation(MyShaders, "Pos");//uniform position

	//Step 1: uniform position and color of the diamond
	glUniform3fv(UniformPositionLoc, 1, &BoundPos[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	DrawDiamond();
	glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_SHORT, 0);

	glm::vec3 translatedVerts[NUM_VERTS];
	for (unsigned int i = 0; i < NUM_VERTS; i++)
	{
		translatedVerts[i] = translatedVerts[i] + Pos_1;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(translatedVerts), translatedVerts);
	glEnableVertexAttribArray(0);

	//Step 2: I am going to draw a circle @ the origin
	UniformColor.g = 0;
	UniformColor.r = 1;
	glUniform3fv(UniformPositionLoc, 1, &TriPos[0]);
	glUniform3fv(UniformColorLoc, 1, &UniformColor[0]);
	//DrawBall(glm::vec2(0.0f, 0.0f), 0.1f, 20);
	DrawBALL();
	glDrawElements(GL_POLYGON, 6, GL_UNSIGNED_SHORT, 0);
}

void  MeGLWindow::BallUpdate()
{
	glm::vec3 velocity(0.0001f, 0.0001f, +0.0f); //Change the num of zeros to slow or speed the velocity of tri1
	Pos_1 = Pos_1 + velocity;
	repaint();
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0);
	glDeleteProgram(MyShaders);
}

