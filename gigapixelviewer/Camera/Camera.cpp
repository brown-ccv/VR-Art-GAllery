#include "Camera.h"



Camera::Camera(glm::vec3 Postion /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 front /*= glm::vec3(0.0f, 0.0f, -1.0f)*/, glm::vec3 Up /*= glm::vec3(0.0, 1.0, 0.0)*/)
:myPosition(Postion),
mySpeed(0.1f), myMouseSpeed(0.1f), myFoV(45.0f), inputFlag(MovingFlag::stop), lastMousePosition(0.0f, 0.0f),
myCanRotate(false), myFront(normalize(front)), myUp(glm::normalize(Up))
{

}

Camera::~Camera()
{
}

void Camera::RotateX(float angle)
{
	glm::vec3 Haxis = glm::cross(yAxis, myFront);
	myRightv = glm::normalize(Haxis);

	rotate(myFront, angle, myRightv);
	myFront = glm::normalize(myFront);
	myUp = glm::normalize(cross(myFront, myRightv));
}

void Camera::RotateY(float angle)
{
	glm::vec3 Haxis = glm::cross(yAxis, myFront);
	myRightv = glm::normalize(Haxis);

	rotate(myFront, angle, yAxis);
	myFront = glm::normalize(myFront);
	myUp = glm::normalize(glm::cross(myFront, myRightv));
}

void Camera::RotateView(glm::vec2 npos)
{
	if (!myCanRotate)
	{
		lastMousePosition.x = npos.x;
		lastMousePosition.y = npos.y;

		return;
	}

	float xAngle = (npos.x - lastMousePosition.x)*myMouseSpeed;
	float yAngle = (npos.y - lastMousePosition.y)*myMouseSpeed;
	RotateX(yAngle); // invert the angle to feel it more natural
	RotateY(-xAngle);
	lastMousePosition.x = npos.x;
	lastMousePosition.y = npos.y;
}

void Camera::SetCaRotate(bool canMove)
{
	myCanRotate = canMove;
}

void Camera::Move(MovingFlag direction)
{
	inputFlag = direction;
}

void Camera::Stop(MovingFlag direction)
{
	inputFlag &= !direction;
}

glm::mat4& Camera::GetView()
{
	// Camera matrix
	return glm::lookAt(myPosition, myPosition + myFront, myUp);
}

glm::vec3 Camera::position()
{
	return myPosition;
}

void Camera::rotate(glm::vec3& vector, float angle, const glm::vec3& axis)
{
	glm::quat quat_view(0, vector.x, vector.y, vector.z);

	float angleRadians = (angle) * (PI / 180);
	float halfSin = sinf(angleRadians / 2);
	float halfCos = cosf(angleRadians / 2);
	glm::quat rotation(halfCos, axis.x * halfSin, axis.y * halfSin, axis.z * halfSin);
	glm::quat conjugate = inverse(rotation);
	glm::quat q = rotation * quat_view;
	q *= conjugate;

	vector.x = q.x;
	vector.y = q.y;
	vector.z = q.z;

}


void Camera::Update()
{
	if (inputFlag == MovingFlag::forward)
	{
		myPosition += myFront * mySpeed;
	}
	if (inputFlag == MovingFlag::backward)
	{
		myPosition -= myFront * mySpeed;
	}
	if (inputFlag == MovingFlag::right)
	{
		myPosition += glm::normalize(glm::cross(myFront, myUp)) * mySpeed;
	}
	if (inputFlag == MovingFlag::left)
	{
		myPosition -= glm::normalize(glm::cross(myFront, myUp)) * mySpeed;
	}
}
