#include "Terrain.h"

Terrain::Terrain(int width, int height, int octaves, float frequency, float amplitude)
{
    this->width = width;
    this->height = height;
    this->octaves = octaves;
    this->maxHeigth = -1000000000.0f;
    this->minHeight = 1000000000.0f;
    this->H = 10.0f;
    this->waterLevel = 10.0f;
    this->snowLevel = 40.0f;
    this->ambientIntensity = 0.1f;
    this->lightColor = glm::vec3(0.0f, 1.0f, 1.0f);
    this->diffuseIntensity = 0.5f;
    this->lightDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    this->shininess = 32.0f;
    this->specularIntensity = 1.0f;
    this->cameraPos = glm::vec3(0, 100, 10);

    // Perlin noise parameters
    float persistence = 0.5f;
    GLfloat xDiv = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat yDiv = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat xMul = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat yMul = (GLfloat)(rand() % 350) / 1750.0f;

    // Generate the height map
    this->heightMap = std::vector<std::vector<float>>(width, std::vector<float>(height));
    //  Ref: https://www.youtube.com/watch?v=A8x39t635sI
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            float total = 0.0f;
            this->amplitude = amplitude;
            this->frequency = frequency;
            float maxValue = 0.0f;
            for (int i = 0; i < octaves; i++)
            {
                total += glm::perlin(glm::vec2(x / xDiv * xMul * this->frequency, y / yDiv * yMul * this->frequency)) * this->amplitude;
                maxValue += amplitude;
                this->amplitude *= persistence;
                this->frequency *= 2.0f;
            }
            float noiseValue = total / maxValue;
            GLfloat temp = ((noiseValue + 1.0f) * 0.5f - 0.13f) * 1.35f;
            this->heightMap[x][y] = temp;

            if (temp > this->maxHeigth)
                this->maxHeigth = temp;

            if (temp < this->minHeight)
                this->minHeight = temp;
        }
    }

    BuildScene();
    CompileShaders();
}

Terrain::~Terrain()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Terrain::Render()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glUniformMatrix4fv(modelViewProjectionLoc, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
    glUniform1f(mountainLoc, H);
    glUniform1f(waterLoc, waterLevel);
    glUniform1f(snowLoc, snowLevel);
    glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);
    glUniform1f(ambientIntensityLoc, ambientIntensity);
    glUniform3f(lightDirectionLoc, lightDirection.x, lightDirection.y, lightDirection.z);
    glUniform1f(diffuseIntensityLoc, diffuseIntensity);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(specularIntensityLoc, specularIntensity);
    glUniform1f(shininessLoc, shininess);
    glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
    // glDrawArrays(GL_POINTS, 0, vertices.size());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Terrain::SetModelViewProjection(glm::mat4 modelViewProjection)
{
    this->modelViewProjection = modelViewProjection;
}

void Terrain::Rebuild()
{
    GenerateHeightMap();
    BuildScene();
    CompileShaders();
}

void Terrain::ChangeOctaves(int octaves)
{
    this->octaves = octaves;
}

void Terrain::ChangeFrequency(float frequency)
{
    this->frequency = frequency;
}

void Terrain::ChangeAmplitude(float amplitude)
{
    this->amplitude = amplitude;
}

void Terrain::BuildScene()
{
    InitVertices();

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    InitIndices();

    InitNormals();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Enable the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (const void *)(0 * sizeof(GLfloat)));

    // Enable the vertex attribute array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (const void *)(3 * sizeof(GLfloat)));

    // Enable the vertex attribute array
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
}

void Terrain::InitVertices()
{
    vertices.resize(width * height);
    int index = 0;

    for (unsigned int i = 0; i < width; i++)
    {
        for (unsigned int j = 0; j < height; j++)
        {
            vertices[index].vertex = glm::vec3(i, heightMap[i][j], j);
            index++;
        }
    }
}

void Terrain::InitIndices()
{
    // Ref: https://www.youtube.com/watch?v=xoqESu9iOUE&list=PLA0dXqQjCx0S9qG5dWLsheiCJV-_eLUM0&index=2

    int NumOfQuads = (width - 1) * (height - 1);
    indices.resize(NumOfQuads * 6);
    int index = 0;

    for (unsigned int j = 0; j < width - 1; j++)
    {
        for (unsigned int i = 0; i < height - 1; i++)
        {
            unsigned int bottomLeft = j * height + i;
            unsigned int topLeft = (j + 1) * height + i;
            unsigned int topRight = (j + 1) * height + i + 1;
            unsigned int bottomRight = j * height + i + 1;

            // First triangle
            indices[index++] = bottomLeft;
            indices[index++] = topLeft;
            indices[index++] = topRight;

            // We use index++ to increment the index by 1 after using it

            // Second triangle
            indices[index++] = topRight;
            indices[index++] = bottomRight;
            indices[index++] = bottomLeft;
        }
    }
}

void Terrain::InitNormals()
{
    int index = 0;

    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        unsigned int index0 = indices[i];
        unsigned int index1 = indices[i + 1];
        unsigned int index2 = indices[i + 2];

        glm::vec3 vec0 = vertices[index1].vertex - vertices[index0].vertex;
        glm::vec3 vec1 = vertices[index2].vertex - vertices[index0].vertex;

        glm::vec3 normal = glm::cross(vec0, vec1);
        normal = glm::normalize(normal);

        vertices[index0].normals += normal;
        vertices[index1].normals += normal;
        vertices[index2].normals += normal;
    };

    for (unsigned int i = 0; i < vertices.size(); i++)
        vertices[i].normals = glm::normalize(vertices[i].normals);
}

void Terrain::CompileShaders()
{
    std::vector<GLuint> shaderList;

    // load and compile shaders
    shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("../shaders/terrain.vert")));
    shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("../shaders/terrain.frag")));

    // create the shader program and attach the shaders to it
    shaderProgram = CreateProgram(shaderList);

    // delete shaders (they are on the GPU now)
    using namespace std;
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    modelViewProjectionLoc = glGetUniformLocation(shaderProgram, "modelview");
    mountainLoc = glGetUniformLocation(shaderProgram, "mountainHeight");
    waterLoc = glGetUniformLocation(shaderProgram, "waterHeight");
    snowLoc = glGetUniformLocation(shaderProgram, "snowHeight");
    ambientIntensityLoc = glGetUniformLocation(shaderProgram, "directionalLight.ambientIntensity");
    lightColorLoc = glGetUniformLocation(shaderProgram, "directionalLight.color");
    lightDirectionLoc = glGetUniformLocation(shaderProgram, "directionalLight.direction");
    diffuseIntensityLoc = glGetUniformLocation(shaderProgram, "directionalLight.diffuseIntensity");
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    specularIntensityLoc = glGetUniformLocation(shaderProgram, "material.specularIntensity");
    shininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
    cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPosition");
}

void Terrain::GenerateHeightMap()
{
    // Perlin noise parameters
    float persistence = 0.5f;
    GLfloat xDiv = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat yDiv = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat xMul = (GLfloat)(rand() % 350) / 1750.0f;
    GLfloat yMul = (GLfloat)(rand() % 350) / 1750.0f;

    // Generate the height map
    this->heightMap = std::vector<std::vector<float>>(width, std::vector<float>(height));
    //  Ref: https://www.youtube.com/watch?v=A8x39t635sI
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            float total = 0.0f;
            float frequency = this->frequency;
            float amplitude = this->amplitude;
            float maxValue = 0.0f;
            for (int i = 0; i < octaves; i++)
            {
                total += glm::perlin(glm::vec2(x / xDiv * xMul * frequency, y / yDiv * yMul * frequency)) * amplitude;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }
            float noiseValue = total / maxValue;
            GLfloat temp = ((noiseValue + 1.0f) * 0.5f - 0.13f) * 1.35f;
            this->heightMap[x][y] = temp;

            if (temp > this->maxHeigth)
                this->maxHeigth = temp;

            if (temp < this->minHeight)
                this->minHeight = temp;
        }
    }
}

void Terrain::SetHeight(float H)
{
    this->H = H;
}

void Terrain::SetWaterLevel(float waterLevel)
{
    this->waterLevel = waterLevel;
}

void Terrain::SetSnowLevel(float snowLevel)
{
    this->snowLevel = snowLevel;
}

void Terrain::SetAmbientIntensity(float ambientIntensity)
{
    this->ambientIntensity = ambientIntensity;
}

void Terrain::SetLightColor(glm::vec3 lightColor)
{
    this->lightColor = lightColor;
}

void Terrain::SetDiffuseIntensity(float diffuseIntensity)
{
    this->diffuseIntensity = diffuseIntensity;
}

void Terrain::SetLightDirection(glm::vec3 lightDirection)
{
    this->lightDirection = lightDirection;
}

void Terrain::SetModel(glm::mat4 model)
{
    this->model = model;
}

void Terrain::SetSpecularIntensity(float specularIntensity)
{
    this->specularIntensity = specularIntensity;
}

void Terrain::SetShininess(float shininess)
{
    this->shininess = shininess;
}

void Terrain::SetCameraPos(glm::vec3 cameraPos)
{
    this->cameraPos = cameraPos;
}
