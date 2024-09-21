#pragma once
#include <vector>
#include "Vertex.h"
#include "Texture.h"

class Mesh
{
public:
	// 网格数据
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	// 函数
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader);
private:
	// 渲染数据
	unsigned int VAO, VBO, EBO;
	// 函数
	void setupMesh();
};