#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "string"

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
};