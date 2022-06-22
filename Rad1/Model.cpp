#include "Model.h"

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
using namespace std;

Model::Model(string const& path, Shader* shader, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale, bool isUV_flipped = true, bool gamma = false, bool canPick = false)
{

	this->shader = shader;
	this->translate = translate;
	this->rotate = rotate;
	this->scale = scale;
	this->gammaCorrection = gamma;
	
	if (canPick)
		this->id = getID();
	else this->id = 999999;
	this->pickShader = new Shader("shaders\\pick.vert", "shaders\\pick.frag");



	modelMat4 = glm::mat4(1.0f);
	modelMat4 = glm::translate(modelMat4, translate);
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.x), glm::vec3(1.f, 0.f, 0.f));
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.y), glm::vec3(0.f, 1.f, 0.f));
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.z), glm::vec3(0.f, 0.f, 1.f));
	modelMat4 = glm::scale(modelMat4, scale);


	loadModel(path, isUV_flipped);
}

void Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::pickDraw()
{

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(pickShader);
}

void Model::updateShader(glm::vec3 cameraPos, glm::mat4 pvMatrix, vector<Light*>& lights)
{

	this->shader->use();
	this->shader->setMatrix4F("pv", pvMatrix);
	this->shader->setMatrix4F("model", modelMat4);
	this->shader->setFloat("shininess", 64.0f);
	this->shader->setVec3("viewPos", cameraPos);

	int active_lights = 0;
	for (int i = 0; i < lights.size(); i++)
	{
		active_lights += lights[i]->putInShader(this->shader, active_lights);
	}
	this->shader->setInt("lights_count", active_lights);

}

void Model::updatePickShader(glm::vec3 cameraPos, glm::mat4 pvMatrix, float alpha, int r, int g, int b)
{
	this->pickShader->use();
	this->pickShader->setMatrix4F("pv", pvMatrix);
	this->pickShader->setMatrix4F("model", modelMat4);
	this->pickShader->setVec3("viewPos", cameraPos);

	int Blue = id & 255;
	int Green = (id >> 8) & 255;
	int Red = (id >> 16) & 255;
	
	if (r >= 0 && g >= 0 && b >= 0)
	{
		Blue = b;
		Green = g;
		Red = r;
	}
	
	

	this->pickShader->setInt("rID", Red);
	this->pickShader->setInt("gID", Green);
	this->pickShader->setInt("bID", Blue);
	this->pickShader->setFloat("aID", alpha);
}

int Model::r()
{
	return (id >> 16) & 255;
}

int Model::g()
{
	return (id >> 8) & 255;

}

int Model::b()
{
	return id & 255;
	
}

void Model::setPosition(float x, float y, float z)
{
	this->translate.x = x;
	this->translate.y = y;
	this->translate.z = z;
	updateModel();

}

void Model::setPosition(glm::vec3 pos)
{
	this->translate = pos;
	updateModel();
}

void Model::setRotate(float x, float y, float z)
{
	this->rotate.x = x;
	this->rotate.y = y;
	this->rotate.z = z;
	updateModel();

}

void Model::setScale(float scale)
{
	this->scale.x = scale;
	this->scale.y = scale;
	this->scale.z = scale;
	updateModel();
}

void Model::updateModel()
{
	modelMat4 = glm::mat4(1.0f);
	modelMat4 = glm::translate(modelMat4, translate);
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.x), glm::vec3(1.f, 0.f, 0.f));
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.y), glm::vec3(0.f, 1.f, 0.f));
	modelMat4 = glm::rotate(modelMat4, glm::radians(rotate.z), glm::vec3(0.f, 0.f, 1.f));
	modelMat4 = glm::scale(modelMat4, scale);
}

void Model::loadModel(string const& path, bool isUV_flipped)
{
	Assimp::Importer importer;
	const aiScene* scene;
	if (isUV_flipped)
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	else
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	// 5. opacity maps
	std::vector<Texture> opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
	textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());



	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::getID()
{
	static int id = 100000;
	return id+=100000;
}


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}