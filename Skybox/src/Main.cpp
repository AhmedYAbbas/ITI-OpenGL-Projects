#include <vector>

#include <glad/glad.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glm/glm.h>
#include <gtc/matrix_transform.h>
#include <gtx/rotate_vector.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/vector3.h>

#include "DrawClass.h"
#include "Shader.h"

/**********************************Vars******************************/
sf::Clock sfClock;

Shader transformShader, skyboxShader;

DrawClass skyBox;
DrawClass teaPot;

GLuint skyboxTextureId;
GLuint diffuseTextureId;
GLuint specularTextureId;

glm::vec3 lightPosition = glm::vec3(10, 10, 10);
glm::vec3 ambientColor = glm::vec3(0.25, 0.15, 0.1);
glm::vec3 diffuseColor = glm::vec3(0.8, 0.65, 0.2);

glm::vec3 cameraPosition = glm::vec3(0.0f, 4.0f, 5.0f);
glm::vec3 cameraLookAt = glm::vec3(0.0f, 2.0f, 0.0f);
glm::vec3 cameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
/**********************************Vars******************************/

/**********************************Functions Declaration******************************/
void SetupWindow(sf::Window& window);
void ImportMesh(std::string fileName, std::vector<float>& modelVertices,
    std::vector<unsigned int>& meshIndecies);
void ImportImage(std::string imageName, GLuint& TextureId);
void CreateCubeMap();
void Initialize();
void DrawCommands();
void GameplayLoop(sf::Window& window);
/**********************************Functions Declaration******************************/

int main()
{
    // create the window
    sf::Window window;
    SetupWindow(window);

    // load resources, initialize the OpenGL states, ...
    Initialize();

    // run the main loop
    GameplayLoop(window);
}

void SetupWindow(sf::Window& window)
{
    window.create(sf::VideoMode(800, 800), "Skybox", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);
    // activate the window
    window.setActive(true);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);
}

void ImportMesh(std::string fileName, std::vector<float>& modelVertices,
    std::vector<unsigned int>& meshIndecies)
{
    Assimp::Importer importer;

    // Reading the file data
    // Applying Generating smooth normals and making all faces triangles
    if (const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_GenSmoothNormals | aiProcess_Triangulate))
    {
        // For each mesh in the scene
        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];

            // Add its vertices / normals in an array            
            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
            {
                // W is 1.0 for points
                modelVertices.push_back(mesh->mVertices[vertexIndex].x);
                modelVertices.push_back(mesh->mVertices[vertexIndex].y);
                modelVertices.push_back(mesh->mVertices[vertexIndex].z);

                // W is 0.0 for vectors
                modelVertices.push_back(mesh->mNormals[vertexIndex].x);
                modelVertices.push_back(mesh->mNormals[vertexIndex].y);
                modelVertices.push_back(mesh->mNormals[vertexIndex].z);

                // TexCoord
                modelVertices.push_back(mesh->mTextureCoords[0][vertexIndex].x);
                modelVertices.push_back(mesh->mTextureCoords[0][vertexIndex].y);

            }

            // Add face data in an array
            for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
            {
                meshIndecies.push_back(mesh->mFaces[faceIndex].mIndices[0]);
                meshIndecies.push_back(mesh->mFaces[faceIndex].mIndices[1]);
                meshIndecies.push_back(mesh->mFaces[faceIndex].mIndices[2]);
            }
        }
    }
}

void ImportImage(std::string imageName, GLuint& TextureId, int texture)
{
    sf::Image img;
    if (img.loadFromFile(imageName.c_str()))
    {
        img.flipVertically();
        glGenTextures(1, &TextureId);
        glActiveTexture(texture);
        glBindTexture(GL_TEXTURE_2D, TextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}

void CreateCubeMap()
{
    std::string cub_maps[6] = {
        "res/right.jpg",
        "res/left.jpg",
        "res/top.jpg",
        "res/bottom.jpg",
        "res/front.jpg",
        "res/back.jpg"
    };

    glGenTextures(1, &skyboxTextureId);
    sf::Image img;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureId);
    for (GLuint i = 0; i < 6; i++)
    {
        img.loadFromFile(cub_maps[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Initialize()
{
    //Load and compile shader
    transformShader.compile("res/transform.vert", "res/transform.frag");
    skyboxShader.compile("res/skybox.vert", "res/skybox.frag");

    std::vector<float> teapotVertices;
    std::vector<unsigned int> teapotIndecies;
    ImportMesh("res/teapot.obj", teapotVertices, teapotIndecies);
    // Generate mesh out of this array
    teaPot.CreateBuffersIndices(
        &teapotVertices[0], teapotVertices.size() * sizeof(float),
        &teapotIndecies[0], teapotIndecies.size() * sizeof(int),
        teapotIndecies.size()
    );

    std::vector<float> skyboxVertices;
    std::vector<unsigned int> skyboxIndecies;
    ImportMesh("res/skybox.obj", skyboxVertices, skyboxIndecies);
    // Generate mesh out of this array
    skyBox.CreateBuffersIndices(
        &skyboxVertices[0], skyboxVertices.size() * sizeof(float),
        &skyboxIndecies[0], skyboxIndecies.size() * sizeof(int),
        skyboxIndecies.size()
    );

    CreateCubeMap();

    ImportImage("res/diffuse.png", diffuseTextureId, GL_TEXTURE1);
    ImportImage("res/specular.png", specularTextureId, GL_TEXTURE2);
}

void DrawCommands()
{
    // clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw...
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    float clockTime = sfClock.getElapsedTime().asSeconds();
    float raduis = 10;
    cameraPosition.x = cos(clockTime) * raduis;
    cameraPosition.z = sin(clockTime) * raduis;

    //Projection transformation
    glm::mat4 projection = glm::perspective(45.0f,
        1.0f, 0.01f, 1000.0f
    );
    //View Transformation
    glm::mat4 view = glm::lookAt(
        cameraPosition,                     //position camera
        cameraLookAt,                       //point i am looking at
        glm::vec3(0.0f, 1.0f, 0.0f)         //up verctor
    );

    // draw our first triangle
    transformShader.use();
    //Model transformation
    //T R S
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, 0));
    model = glm::scale(model, glm::vec3(1, 1, 1));
    transformShader.setMat4("model", model);
    transformShader.setMat4("view", view);
    transformShader.setMat4("projection", projection);
    transformShader.setVec3("cameraPosition", cameraPosition);
    transformShader.setVec4("lightDirection", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));
    transformShader.setVec4("ambientColor", glm::vec4(0.1f, 0.16f, 0.24f, 1));
    transformShader.setVec4("diffuseColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    transformShader.setVec4("specularColor", glm::vec4(1.0f));
    transformShader.setFloat("ambientCoeff", 1.0f);
    transformShader.setFloat("diffuseCoeff", 1.0f);
    transformShader.setFloat("specularCoeff", 1.0f);

    transformShader.setInt("skybox", 0);
    transformShader.setInt("diffuse", 1);
    transformShader.setInt("specular", 2);

    // Draw tea pot model
    teaPot.DrawIndices();


    skyboxShader.use();
    glm::mat4 skyboxModel = glm::scale(glm::mat4(1.0f), glm::vec3(600, 600, 600));
    skyboxShader.setMat4("model", skyboxModel);
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // Draw tea pot model
    skyBox.DrawIndices();
}

void GameplayLoop(sf::Window& window)
{
    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        //Draw the buffers
        DrawCommands();

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }
}
