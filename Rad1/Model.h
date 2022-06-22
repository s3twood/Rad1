#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Shader.h"
#include "Light.h"
#include <string>
#include <vector>


using namespace std;


class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    Shader* shader;
    Shader* pickShader;
    glm::vec3 translate;
    glm::vec3 rotate;
    glm::vec3 scale;
    glm::mat4 modelMat4;
    unsigned int id;

    bool gammaCorrection;

    Model(string const& path, Shader* shader, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale, bool isUV_flipped, bool gamma, bool canPick);
    void Draw();
    void pickDraw();
    void updateShader(glm::vec3 cameraPos, glm::mat4 pvMatrix, vector<Light*> &lights);
    void updatePickShader(glm::vec3 cameraPos, glm::mat4 pvMatrix, float alpha = 1.0f, int r = -1, int g = -1, int b = -1);
    int r();
    int g();
    int b();
    void setPosition(float x, float y, float z);
    void setPosition(glm::vec3 pos);
    void setRotate(float x, float y, float z);
    void setScale(float scale);
    void updateModel();
private:
    void loadModel(string const& path, bool isUV_flipped);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    unsigned int getID();
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

#endif