#include <gl\glew.h>
#include <MeGLWindow.h>

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

void sendDataToOpenGL()
{
	GLfloat verts[] =
	{
		//top triangle
		+0.0f, 1.0f, //0 position
		//-1.0f, 1.0f,
		255.0f, 69.0f, 0.0f,

		-0.5f, +0.0f, //1
		//-0.7f, 1.0f,
		255.0f, 69.0f, 0.0f,

		0.5f, +0.0f, //2
		//-1.0f, 0.0f,
		255.0f, 69.0f, 0.0f,

		//Bottom Left triangle
		//-0.5f, 0.0f, //1
		-1.0f, -1.0f, //3
		//-0.7f, 0.0f,
		255.0f, 69.0f, 0.0f, //color for each vertex 3

		+0.0f, -1.0f, //4
		//-0.7f, 0.2f,
		255.0f, 69.0f, 0.0f, //color for each vertex 4

		//Bottom Right triangle
		//0.5f, 0.0f, //2
		1.0f, -1.0f, //5
		//-0.5f, 0.2f,
		255.0f, 69.0f, 0.0f, //color for each vertex 5

		+0.0f, -1.0f, //6
		//-0.5f, 0.0f,
		255.0f, 69.0f, 0.0f, //color for each vertex 6

		-0.3f, 0.0f, //7
		255.0f, 223.0f, 0.0f, 

		-0.3f, 1.0f, //8
		255.0f, 223.0f, 0.0f,

		-0.2f, 0.0f, //9
		255.0f, 223.0f, 0.0f,

		-0.1f, 1.0f, //10
		255.0f, 223.0f, 0.0f,
	};

	GLuint VertBufferID;
	glGenBuffers(1, &VertBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, VertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

	glEnableVertexAttribArray(1);//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0,1,2, 1,3,4, 2,5,6 }; //better way to generate multiple triangles with repeating verts (takes the verts locations & make triangle)
	//GLushort indices[] = { 0,1,2, 2,1,3, 3,4,5, 5,3,6, 7,8,9, 8,9,10}; //better way to generate multiple triangles with repeating verts (takes the verts locations & make triangle)
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
	//glDrawArrays(GL_TRIANGLES, 0, 3); //draw array info
	//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); //draw indices info 
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0); //draw indices info 
}