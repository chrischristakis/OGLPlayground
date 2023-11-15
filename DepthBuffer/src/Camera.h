#ifndef CAMERA_H
#define CAMERA_H
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
	float pitch = 0, yaw = 0;

	float sensitivity = 0.05f;
	float speed = 0.05f;
	bool firstMouseMove = true;

	double prevX = 0, prevY = 0;

	const glm::vec3 INITIAL_LOOK_DIR = {0, 0, -1};

	glm::vec3 position;

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	void recalculateFrontVector() {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		// Calculate lookDir with intrinsic yaw, then pitch rotation (intrinsic = reversed extrinsic)
		glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec4 lookDir = yawMat * pitchMat * glm::vec4(INITIAL_LOOK_DIR, 1);

		front = glm::normalize(glm::vec3(lookDir));
	}

public:
	Camera(glm::vec3 startPos, glm::vec3 camUp, float pitch = 0, float yaw = 0): pitch(pitch), yaw(yaw) {
		position = startPos;
		front = INITIAL_LOOK_DIR;
		up = camUp;
		right = glm::normalize(glm::cross(front, camUp));

		recalculateFrontVector();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void update(GLFWwindow* window) {
		right = glm::normalize(glm::cross(front, up));
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			position += front * speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			position -= front * speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			position -= right * speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			position += right * speed;

		// Up and down movement
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			position += up * speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			position -= up * speed;
	}

	void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouseMove) {
			prevX = xpos;
			prevY = ypos;
			firstMouseMove = false;
		}

		float offsetX = xpos - prevX;
		float offsetY = ypos - prevY;

		pitch -= offsetY * sensitivity;
		yaw -= offsetX * sensitivity;

		recalculateFrontVector();

		prevX = xpos;
		prevY = ypos;
	}

};
#endif