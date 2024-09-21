#pragma once
#include <vector>
#include "Vertex.h"
#include "Texture.h"

class Mesh
{
public:
	// ��������
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	// ����
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);
private:
	// ��Ⱦ����
	unsigned int VAO, VBO, EBO;
	// ����
	void setupMesh();
};