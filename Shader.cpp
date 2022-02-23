#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	Load(vertexPath, fragmentPath);
	Build();
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

void Shader::set(const std::string& name, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void Shader::Load(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	std::ifstream vsFile;
	std::ifstream fsFile;
	std::stringstream vsStream, fsStream;
	try
	{
		vsFile.open(vertexPath);
		fsFile.open(fragmentPath);
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

void Shader::Build()
{
	const char* vertexShaderSource = this->vertexShaderSource.c_str();
	const char* fragmentShaderSource = this->fragmentShaderSource.c_str();
	// ¶¥µã×ÅÉ«Æ÷
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// ´íÎóÅÐ¶Ï
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		wprintf(L"¶¥µã×ÅÉ«Æ÷±àÒëÊ§°Ü£¡\n");
	}

	// Æ¬¶Î×ÅÉ«Æ÷
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// ´íÎóÅÐ¶Ï
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		wprintf(L"Æ¬¶Î×ÅÉ«Æ÷±àÒëÊ§°Ü£¡\n");
	}

	// Á´½Ó
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	// ´íÎóÅÐ¶Ï
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		wprintf(L"Æ¬¶Î×ÅÉ«Æ÷±àÒëÊ§°Ü£¡\n");
	}

	// É¾³ý
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}
