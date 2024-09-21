#include <iostream>
#include <filesystem>
#include "Model.h"
#include "EncodeTool.h"
#include "stb_image.h"

static unsigned int textureFromFile(const std::string& filename, const std::string& directory, bool gamma = false);

Model::Model(const std::string& path)
{
	loadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (size_t i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate);

	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::wcout << L"ERROR::ASSIMP::" << s2ws(importer.GetErrorString()) << std::endl;
		return;
	}
	std::filesystem::path dir = std::filesystem::path(path).parent_path();
	directory = dir.string();

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// 处理节点所有的网格(如果有)
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(aimesh, scene));
	}
	// 遍历子节点
	for (size_t i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// 处理顶点位置、法线和纹理坐标
		glm::vec3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.Position = vec;
		vec.x = mesh->mNormals[i].x;
		vec.y = mesh->mNormals[i].y;
		vec.z = mesh->mNormals[i].z;
		vertex.Normal = vec;
		if (mesh->mTextureCoords[0]) // 有纹理坐标
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}
	// 处理索引
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture>&& diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture>&& specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string name = str.C_Str();
		bool skip = false;
		for (size_t j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = textureFromFile(name, directory);
			texture.type = typeName;
			texture.path = name;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

static unsigned int textureFromFile(const std::string& filename, const std::string& directory, bool gamma)
{
	std::filesystem::path path = std::filesystem::path(directory) / filename;

	//texture
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrComponents, 0);
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
		std::wcout << L"Texture failed to load at path: " << path.wstring() << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}