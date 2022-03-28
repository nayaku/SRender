#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
class Shader
{
public:
	unsigned int ID;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void use();
	void set(const std::string& name, bool value) const;
	void set(const std::string& name, int value) const;
	void set(const std::string& name, float value) const;
	void set(const std::string& name, const glm::mat4& mat) const;
	void set(const std::string& name, const glm::vec3& val) const;
private:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	void Load(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Build();
};
