#pragma once
#include <glad/glad.h>

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
private:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	void Load(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Build();
};
