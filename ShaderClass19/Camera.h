#include <glm.hpp>

class Camera
{
	glm::vec3 position;
	glm::vec3 viewDirection;
	const glm::vec3 UP;

public:
	Camera();
	glm::mat4 getWorldToViewMatrix() const;

	/*void camera.moveForward();
	void camera.moveBackward();
	void camera.strafeLeft();
	void camera.strafeRight();*/
};