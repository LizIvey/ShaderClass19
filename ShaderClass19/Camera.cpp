#include <Camera.h>
#include <C:\Users\krazi\Desktop\Tech Art III\ShaderClass19\Middleware\glm\glm\gtx\transform.hpp>
Camera::Camera() :
	viewDirection(0.0f, 0.0f, -1.0f),
UP(0.0f, 1.0f, 0.0f)
{
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, UP);
}

const float Movement_Speed = 0.1f;

/*void Camera::moveForward()
{

}

void Camera::moveBackward()
{

}

void Camera::strafeLeft()
{

}

void Camera::strafeRight()
{

}*/