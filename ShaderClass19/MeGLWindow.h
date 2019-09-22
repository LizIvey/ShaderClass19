#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>

class MeGLWindow : public QGLWidget
{
	//void sendDataToOpenGL();
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	std::string readShaderCode(const char* filename);
	void installShaders();
	void DrawCircle(float x, float y, float r, int LineSeg);
	void DrawDiamond();

protected:
	void initializeGL();
	void paintGL();
	void keyPressEvent(QKeyEvent*);

public:
	~MeGLWindow();
};

#endif

