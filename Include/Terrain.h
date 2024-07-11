#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/ext.hpp"

#include "shaders.h"

struct Vertices
{
    glm::vec3 vertex;
    glm::vec3 normals = glm::vec3(0.0f);
};

class Terrain
{
private:
    // width and height of the terrain
    int width, height;
    // height map
    std::vector<std::vector<float>> heightMap;
    // octaves of the perlin noise
    int octaves;
    // Perlin noise parameters
    float frequency, amplitude;
    // vertex array object, vertex buffer object and index buffer object
    GLuint VAO, VBO, EBO;
    // shader program
    GLuint shaderProgram;
    // modelViewProjection uniform location
    GLint modelViewProjectionLoc, mountainLoc, waterLoc, snowLoc, ambientIntensityLoc, lightColorLoc, lightDirectionLoc, diffuseIntensityLoc, modelLoc, specularIntensityLoc, shininessLoc, cameraPosLoc;
    // vector of vertices
    // std::vector<glm::vec3> vertices;
    std::vector<Vertices> vertices;
    // vector of indices
    std::vector<unsigned int> indices;
    // vector of normals
    // std::vector<glm::vec3> normals;
    // Build scene
    void BuildScene();
    // Compile the shaders
    void CompileShaders();
    // Initialize the vertices
    void InitVertices();
    // Initialize the indices
    void InitIndices();
    // Initialize Normals
    void InitNormals();
    // Initialize the normals
    std::vector<glm::vec3> normals;
    // Model view projection matrix
    glm::mat4 modelViewProjection;
    // Max height
    float maxHeigth, minHeight;
    // Generate the height map
    void GenerateHeightMap();
    // Height
    float H;
    // Water Level
    float waterLevel;
    // Snow Level
    float snowLevel;
    // Ambient Intensity
    GLfloat ambientIntensity;
    // Light Color
    glm::vec3 lightColor;
    // Directional Light
    glm::vec3 lightDirection;
    // Diffuse Intensity
    GLfloat diffuseIntensity;
    // Model
    glm::mat4 model;
    // Specular Intensity
    GLfloat specularIntensity;
    // Shininess
    GLfloat shininess;
    // Camera Position
    glm::vec3 cameraPos;

public:
    // Constructor
    Terrain(int width, int height, int octaves, float frequency, float amplitude);
    // Destructor
    ~Terrain();
    // Generate the terrain
    void Render();
    // Set the model view projection matrix
    void SetModelViewProjection(glm::mat4 modelViewProjection);
    // Rebuid the terrain
    void Rebuild();
    // Change octaves
    void ChangeOctaves(int octaves);
    // Change frequency
    void ChangeFrequency(float frequency);
    // Change amplitude
    void ChangeAmplitude(float amplitude);
    // Set the height
    void SetHeight(float H);
    // Set Water Level
    void SetWaterLevel(float waterLevel);
    // Set snow level
    void SetSnowLevel(float snowLevel);
    // Set Ambient Intensity
    void SetAmbientIntensity(float ambientIntensity);
    // Set Light Color
    void SetLightColor(glm::vec3 lightColor);
    // Set Diffuse Intensity
    void SetDiffuseIntensity(float diffuseIntensity);
    // Set Light Direction
    void SetLightDirection(glm::vec3 lightDirection);
    // Set the model
    void SetModel(glm::mat4 model);
    // Set the specular intensity
    void SetSpecularIntensity(float specularIntensity);
    // Set the shininess
    void SetShininess(float shininess);
    // Set the camera position
    void SetCameraPos(glm::vec3 cameraPos);
};

#endif // TERRAIN_H