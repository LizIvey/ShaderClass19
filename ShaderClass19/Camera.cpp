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

void Camera::moveForward()
{
	position += Movement_Speed * viewDirection;
}
void Camera::moveBackward()
{
	position += -Movement_Speed * viewDirection;
}

void Camera::moveLeft()
{
	glm::vec3 StrafeDirection = glm::cross(viewDirection, UP);
	position += -Movement_Speed * StrafeDirection;
}

void Camera::moveRight()
{
	glm::vec3 StrafeDirection = glm::cross(viewDirection, UP);
	position += Movement_Speed * StrafeDirection;
}

void Camera::goUp()
{
	position += Movement_Speed * viewDirection;
}

void Camera::goBack()
{
	position += -Movement_Speed * viewDirection;
}

void Camera::goLeft()
{
	glm::vec3 StrafeDirection = glm::cross(viewDirection, UP);
	position += -Movement_Speed * StrafeDirection;
}

void Camera::goRight()
{
	glm::vec3 StrafeDirection = glm::cross(viewDirection, UP);
	position += Movement_Speed * StrafeDirection;
}