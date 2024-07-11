/*
 Ruled surface
(C) Bedrich Benes 2022
Purdue University
bbenes@purdue.edu
*/

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector> //Standard template library
#include <array>

#include "Terrain.h"
#include "Camera.h"
#include "trackball.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "glfw3.lib")

using namespace std;

int pointSize = 2;
TrackBallC trackball;
bool mouseLeft, mouseMid, mouseRight;
GLdouble mouseX, mouseY;

float terrainWidth = 100.0f;
float terrainHeight = 100.0f;

float terrainPosX = -terrainWidth / 2.0f;
float terrainPosY = 0.0f;
float terrainPosZ = -terrainHeight / 2.0f;

float terrainRotX = 0.0f;
float terrainRotY = 0.0f;

float cameraPosX = 0.0f;
float cameraPosY = 100.0f;
float cameraPosZ = 10.0f;
float cameraTargetX = 50.0f;
float cameraTargetY = 1.0f;
float cameraTargetZ = 0.0f;

int octaves = 20;
float frequency = 0.001f;
float amplitude = 1.0f;
float H = 10.0f;
float waterLevel = 10.0f;
float snowLevel = 40.0f;

// Lighting
float ambientIntensity = 0.1f;
glm::vec3 lightColor = glm::vec3(0.0f, 1.0f, 1.0f);
float diffuseIntensity = 0.5f;
glm::vec3 lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
float specularIntensity = 1.0f;
float shininess = 32.0f;

// Quit when ESC is released
static void KbdCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// set the callbacks for the virtual trackball
// this is executed when the mouse is moving
void MouseCallback(GLFWwindow *window, double x, double y)
{
    // do not forget to pass the events to ImGUI!
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    if (io.WantCaptureMouse)
        return; // make sure you do not call this callback when over a menu
    // now process them
    mouseX = x;
    mouseY = y;
    // we need to perform an action only if a button is pressed
    if (mouseLeft)
        trackball.Rotate(mouseX, mouseY);
    if (mouseMid)
        trackball.Translate(mouseX, mouseY);
    if (mouseRight)
        trackball.Zoom(mouseX, mouseY);
}

// set the variables when the button is pressed or released
void MouseButtonCallback(GLFWwindow *window, int button, int state, int mods)
{
    // do not forget to pass the events to ImGUI!

    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, state);
    if (io.WantCaptureMouse)
        return; // make sure you do not call this callback when over a menu

    // process them
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
    {
        trackball.Set(window, true, mouseX, mouseY);
        mouseLeft = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE)
    {
        trackball.Set(window, false, mouseX, mouseY);
        mouseLeft = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_PRESS)
    {
        trackball.Set(window, true, mouseX, mouseY);
        mouseMid = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_RELEASE)
    {
        trackball.Set(window, true, mouseX, mouseY);
        mouseMid = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS)
    {
        trackball.Set(window, true, mouseX, mouseY);
        mouseRight = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_RELEASE)
    {
        trackball.Set(window, true, mouseX, mouseY);
        mouseRight = false;
    }
}

int main()
{
    // Inicializar GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // negotiate with the OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // make OpenGL window
    GLFWwindow *window = glfwCreateWindow(1000, 1000, "Simple", NULL, NULL);
    // is all OK?
    if (window == NULL)
    {
        std::cout << "Cannot open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Paste the window to the current context
    glfwMakeContextCurrent(window);

    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialization failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    // DEPTH test

    glViewport(0, 0, bufferWidth, bufferHeight);

    // Background color
    glClearColor(0.2f, 0.2f, 0.2f, 10.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glPointSize(pointSize);

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetKeyCallback(window, KbdCallback); // set keyboard callback to quit
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    // Create the terrain
    Terrain terrain(100, 100, octaves, frequency, amplitude);
    Camera camera;

    bool Fill = false;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll the events
        glfwPollEvents();

        // Clean the window
        glClearDepth(100000.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Terrain Position");
        ImGui::SliderFloat("X Position", &terrainPosX, -100.0f, 100.0f);
        ImGui::SliderFloat("Y Position", &terrainPosY, -100.0f, 100.0f);
        ImGui::SliderFloat("Z Position", &terrainPosZ, -100.0f, 100.0f);
        ImGui::SliderFloat("X Rotation", &terrainRotX, 0.0f, 360.0f);
        ImGui::SliderFloat("Y Rotation", &terrainRotY, 0.0f, 360.0f);
        ImGui::End();

        ImGui::Begin("Terrain Controls");
        if (ImGui::SliderInt("Octaves", &octaves, 1, 100, "%d", 0))
        {
            terrain.ChangeOctaves(octaves);
            terrain.Rebuild();
        }
        if (ImGui::SliderFloat("Frequency", &frequency, 0.00f, 1.0f, "%.2f", 0))
        {
            terrain.ChangeFrequency(frequency);
            terrain.Rebuild();
        }
        if (ImGui::SliderFloat("Amplitude", &amplitude, 0.00f, 10.0f, "%.2f", 0))
        {
            terrain.ChangeAmplitude(amplitude);
            terrain.Rebuild();
        }
        ImGui::End();

        ImGui::Begin("Scene Controls");
        if (ImGui::SliderFloat("Height", &H, 0.00f, 100.0f, "%.2f", 0))
        {
            terrain.SetHeight(H);
        }
        if (ImGui::SliderFloat("Water Level", &waterLevel, 0.00f, 100.0f, "%.2f", 0))
        {
            terrain.SetWaterLevel(waterLevel);
        }
        if (ImGui::SliderFloat("Snow Level", &snowLevel, 0.00f, 100.0f, "%.2f", 0))
        {
            terrain.SetSnowLevel(snowLevel);
        }
        if (ImGui::Checkbox("Fill", &Fill))
        {
            if (Fill)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
        }
        ImGui::End();

        ImGui::Begin("Lighting Controls");
        if (ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.00f, 1.0f, "%.2f", 0))
        {
            terrain.SetAmbientIntensity(ambientIntensity);
        }
        if (ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor)))
        {
            terrain.SetLightColor(lightColor);
        }
        if (ImGui::SliderFloat("Diffuse Intensity", &diffuseIntensity, 0.00f, 1.0f, "%.2f", 0))
        {
            terrain.SetDiffuseIntensity(diffuseIntensity);
        }
        if (ImGui::SliderFloat("Light Direction X", &lightDirection.x, -1.0f, 1.0f, "%.2f", 0))
        {
            terrain.SetLightDirection(lightDirection);
        }
        if (ImGui::SliderFloat("Light Direction Y", &lightDirection.y, -1.0f, 1.0f, "%.2f", 0))
        {
            terrain.SetLightDirection(lightDirection);
        }
        if (ImGui::SliderFloat("Light Direction Z", &lightDirection.z, -1.0f, 1.0f, "%.2f", 0))
        {
            terrain.SetLightDirection(lightDirection);
        }
        if (ImGui::SliderFloat("Specular Intensity", &specularIntensity, 0.00f, 1.0f, "%.2f", 0))
        {
            terrain.SetSpecularIntensity(specularIntensity);
        }
        if (ImGui::SliderFloat("Shininess", &shininess, 0.00f, 100.0f, "%.2f", 0))
        {
            terrain.SetShininess(shininess);
        }

        ImGui::End();

        // set the projection matrix
        glm::mat4 proj = glm::perspective(65.f, 1.f, 0.01f, 1000.f);
        // set the viewing matrix (looking from [0,0,5] to [0,0,0])
        glm::mat4 view = glm::lookAt(glm::vec3(0, 100, 10), glm::vec3(50, 1, 0), glm::vec3(0, 1, 0));
        // get the modeling matrix from the trackball
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(terrainPosX, terrainPosY, terrainPosZ));
        model = glm::rotate(model, glm::radians(terrainRotX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(terrainRotY), glm::vec3(0.0f, 1.0f, 0.0f));
        // premultiply the modelViewProjection matrix
        glm::mat4 modelView = proj * view * model;

        // Render the terrain
        terrain.SetModelViewProjection(modelView);
        terrain.Render();

        // Render the ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
