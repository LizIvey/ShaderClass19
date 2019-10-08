#include <gl\glew.h>
#include <MeGLWindow.h>
#include <iostream>
#include <fstream>
#include <glm.hpp>
#include <QtGui\qkeyevent>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <C:\Users\krazi\Desktop\Tech Art III\ShaderClass19\Middleware\glm\glm\gtx\normal.hpp>
#include <C:\Users\krazi\Desktop\Tech Art III\ShaderClass19\Middleware\glm\glm\gtx\transform.hpp>

/*shape bouncing off of the boundaries and reposition shape, so there is no fighting with boundary collision*/

using namespace std;

GLuint MyShaders;
int debugCount = 0;
glm::vec3 velocity;

bool Collisions = false;

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
		glm::vec3(0.0f, 1.0f, 0.0f),//0
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(1.0f, 0.0f, 0.0f),//1
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(0.0f, -1.0f, 0.0f),//2
		glm::vec3(0.0f, 1.0f, 0.0f),

		glm::vec3(-1.0f, 0.0f, 0.0f),//3
		glm::vec3(0.0f, 1.0f, 0.0f),
	};

	const unsigned int NUM_VERTS = sizeof(Ball) / sizeof(*Ball);
	const unsigned int BOUNDARY_VERTS = sizeof(Diamond) / sizeof(*Diamond);

	GLuint BoundVertexBufferID;
	GLuint BallvertexBufferID;

	glm::vec3 transformedPoints[NUM_VERTS];
	glm::vec3 Pos_1(0.0f, 0.0f, 0.0f);
	glm::vec3 Pos_2(0.0f, 0.0f, 0.0f);
	glm::vec3 OldShapePostion;
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

int randSign()
{
	return rand() % 2 == 0 ? 1 : -1;
}

float randComponent()
{
	return rand() % 100 * 0.00001 * randSign();
}

void MeGLWindow::initializeGL()
{
	glewInit();
	//sendDataToOpenGL();
	installShaders();

	//random seed for moving the shape
	srand(time(NULL));
	velocity = glm::vec3(randComponent(), randComponent(), +0.0f);

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
	OldShapePostion = Pos_1;
	Pos_1 = Pos_1 + velocity;
	ShapeCollisions();
	repaint();
}

glm::vec3 VectorClockwiseRot(float x, float y)
{
	return glm::vec3(y, -x, 0.0f);
}

glm::vec3 VectorCounterClockwiseRot(float x, float y)
{
	return glm::vec3(-y, x, 0.0f);
}

void MeGLWindow::ShapeCollisions()
{
	for (uint i = 0; i < BOUNDARY_VERTS; i++)
	{
		const glm::vec3& First = Diamond[i].position;
		const glm::vec3& Second = Diamond[(i + 1) % BOUNDARY_VERTS].position;

		glm::vec3 Wall = First - Second;
		glm::vec3 Nrml = VectorCounterClockwiseRot(Wall.x, Wall.y); //function rotates the nrml
		glm::vec3 Normalized_Nrmls = glm::normalize(Nrml); //normalizing the normals....giving it a length of 1...only care about its magnitude

		glm::vec3 CurrPos = Pos_1 - First;

		//finding the dot product of the normal and shapes current position
		float Dot = glm::dot(CurrPos, Nrml);
		float DotVelocity = glm::dot(velocity, Normalized_Nrmls);

		if (Collisions || (Dot < 0))
		{
			velocity = velocity - 2 * DotVelocity * Normalized_Nrmls;
			Pos_1 = OldShapePostion;
		}
			//velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		//cout << Collisions << endl;
	}
}

MeGLWindow::~MeGLWindow()
{
	glUseProgram(0); 
	glDeleteProgram(MyShaders);
}