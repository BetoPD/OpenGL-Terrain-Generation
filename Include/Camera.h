#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Based on the camera class from https://learnopengl.com/Getting-started/Camera

class Camera
{
public:
    Camera();
    ~Camera();
    glm::mat4 Rotate(float, float);
    void SetCameraPos(glm::vec3);
    void SetCameraTarget(glm::vec3);
    void SetCameraDirection(glm::vec3);
    glm::mat4 GetProjectionMatrix(float fov, float aspectRatio, float nearClip, float farClip);
    void SetCamera();

private:
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    glm::vec3 cameraDirection;
    glm::vec3 up;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
    float camX;
    float camZ;
    glm::mat4 view;
};

#endif