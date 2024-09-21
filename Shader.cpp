#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "EncodeTool.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	Load(vertexPath, fragmentPath);
	Build();
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::set(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set(const std::string& name, const glm::vec3& val) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &val[0]);
}

void Shader::Load(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	std::ifstream vsFile;
	std::ifstream fsFile;
	std::stringstream vsStream, fsStream;
	try
	{
		vsFile.open(vertexPath, std::ifstream::in);
		fsFile.open(fragmentPath, std::ifstream::in);
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();
		vsFile.close();
		fsFile.close();
	}
	catch (std::ifstream::failure e)
	{
		wprintf(L"¶ÁÈ¡äÖÈ¾Æ÷´úÂë´íÎó\n");
		throw;
	}
	vertexShaderSource = vsStream.str();
	fragmentShaderSource = fsStream.str();
}

static void checkCompileErrors(GLuint shader, const std::string& type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
	}
	if (!success) {
		std::wcout << L"±àÒë´íÎó£º" << s2ws(type) << std::endl;
		std::wcout << L"ÐÅÏ¢£º" << s2ws(infoLog) << std::endl;
		exit(-1);
	}
	
}

void Shader::Build()
{
	const char* vertexShaderSource = this->vertexShaderSource.c_str();
	const char* fragmentShaderSource = this->fragmentShaderSource.c_str();
	// ¶¥µã×ÅÉ«Æ÷
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");

	// Æ¬¶Î×ÅÉ«Æ÷
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkCompileErrors(fragmentShader, "FRAGMENT");

	// Á´½Ó
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// É¾³ý
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}
