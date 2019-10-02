#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>
#include <Qt\qtimer.h>

//namespace Math { struct Vector2D{}; }

class MeGLWindow : public QGLWidget
{
	Q_OBJECT
	//void sendDataToOpenGL();
	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	std::string readShaderCode(const char* filename);
	void installShaders();
	//void DrawBall(float x, float y, float r, int LineSeg);
	void DrawDiamond();
	QTimer timer;
	void HandleBoundaries();

protected:
	void initializeGL();
	void paintGL();
	//void keyPressEvent(QKeyEvent*);

public:
	~MeGLWindow();
private slots:
	void BallUpdate();
};

#endif
