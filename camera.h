#pragma once

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    Camera(const glm::vec3 pos, const glm::vec3 target, const glm::vec3 upVector) : position(pos)
    {
        forward = glm::normalize(target - pos);
        right = glm::normalize(glm::cross(forward, upVector));
        up = glm::cross(right, forward);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    void processInput(GLFWwindow* window, float deltaTime)
    {
        const float cameraSpeed = 2.5f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += forward * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= forward * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * cameraSpeed;

        // Mouse movement
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(ypos - lastY);
        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
    }

private:
    double lastX = 720, lastY = 450;
    bool firstMouse = true;

    float yaw;
    float pitch;
    glm::vec3 worldUp;

    void updateCameraVectors()
    {
        glm::vec3 newForward;

        newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newForward.y = sin(glm::radians(pitch));
        newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        forward = glm::normalize(newForward);

        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::cross(right, forward);
    }
};
