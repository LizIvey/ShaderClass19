#include <gl\glew.h>
#include <MeGLWindow.h>
#include <glfw3.h>

/*
Assignment #2:

	1. in paintgl -> update and draw
	2. two triangles one of different solid color use uniforms to do that
	3. wsad controls one triangle and arrow keys control the others

	The update function must update the position of the triangles based on keyboard input.*/

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

void sendDataToOpenGL()
{
	GLfloat verts[] =
	{
		0.8f, -0.3f,//trying to do a push
		1.0f, 0.0f, 0.0f,
		0.6f, -0.6f,
		1.0f, 0.0f, 0.0f,
		0.9f, -0.6f,
		1.0f, 0.0f, 0.0f,
		-0.8f, 0.5f,
		0.0f, 1.0f, 0.0f,
		-1.0f, 0.2f,
		0.0f, 1.0f, 0.0f,
		-0.7f, 0.2f,
		0.0f, 1.0f, 0.0f,
	};

	GLuint VertBufferID;
	glGenBuffers(1, &VertBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

	glEnableVertexAttribArray(1);//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	GLushort indices[] = {0,1,2, 3,4,5}; //better way to generate multiple triangles with repeating verts (takes the verts locations & make triangle)
	GLuint IndexBufferID;
	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

//this is to color my triangles
void InstallShaders()
{
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapt[1];
	adapt[0] = vertexShaderCode;
	glShaderSource(vert, 1, adapt, 0);
	adapt[0] = fragmentShaderCode;
	glShaderSource(frag, 1, adapt, 0);

	glCompileShader(vert);
	glCompileShader(frag);

	GLuint ColorMe = glCreateProgram();
	glAttachShader(ColorMe, vert);
	glAttachShader(ColorMe, frag);
	glLinkProgram(ColorMe);

	glUseProgram(ColorMe);
}

void MeGLWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	InstallShaders();
}

void MeGLWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0); //draw indices info 
}