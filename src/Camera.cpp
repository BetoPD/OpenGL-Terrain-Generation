#include "Camera.h"

Camera::Camera()
{
    cameraPos = glm::vec3(0.0f, 10.0f, 3.0f);
    cameraTarget = glm::vec3(0.0f, 10.0f, 10.0f);
    cameraDirection = glm::normalize(cameraPos - cameraTarget);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
}

Camera::~Camera()
{
}

glm::mat4 Camera::Rotate(float speed, float radius)
{
    camX = sin(speed) * radius;
    camZ = cos(speed) * radius;
    view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    return view;
}

void Camera::SetCameraPos(glm::vec3 pos)
{
    cameraPos = pos;
    SetCamera();
}

void Camera::SetCameraTarget(glm::vec3 target)
{
    cameraTarget = target;
    SetCamera();
}

void Camera::SetCameraDirection(glm::vec3 direction)
{
    cameraDirection = direction;
    SetCamera();
}

glm::mat4 Camera::GetProjectionMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
    return glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

void Camera::SetCamera()
{
    cameraDirection = glm::normalize(cameraPos - cameraTarget);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
}
