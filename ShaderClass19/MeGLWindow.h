#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>

class MeGLWindow : public QGLWidget
{
	void sendDataToOpenGL();
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	std::string readShaderCode(const char* filename);
	void installShaders();

protected:
	void initializeGL();
	void paintGL();
	void keyPressEvent(QKeyEvent*);

public:
	~MeGLWindow();
};

#endif

