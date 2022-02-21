#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <clocale>
#include <iostream>
#include "Shader.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void build();
void Render();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float vertices[] = {
	// λ��              // ��ɫ
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
};
Shader* pOurShader = NULL;

unsigned int VBO, VAO;
//unsigned int shaderProgram;

int main()
{
	// ���ÿ���̨��������ʾ
	setlocale(LC_ALL, "chs");

	// ��ʼ��glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// ��������
	GLFWwindow* window = glfwCreateWindow(800, 600, "SRender", NULL, NULL);
	if (window == NULL)
	{
		wprintf(L"��������ʧ�ܣ�\n");
		glfwTerminate();
		return -1;
	}

	// ��ʾ����
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// GLAD��ʼ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		wprintf(L"GLAD��ʼ������\n");
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);

	build();


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// ��Ⱦָ��
		Render();
		// ��鲢�����¼�����������
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Delete
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(pOurShader->ID);
	delete pOurShader;
	//glDeleteProgram(shaderProgram);


	glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void build()
{
	//Shader ourShader("shader.vert", "shader.frag");

	pOurShader = new Shader("shader\\shader.vert", "shader\\shader.frag");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// ��
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
void Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// ������ɫ��
	//glUseProgram(shaderProgram);
	pOurShader->use();

	// ����������
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

}
