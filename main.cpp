#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <clocale>
#include <iostream>
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

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor,1.0);\n"
"}\n\0";

unsigned int VBO, VAO;
unsigned int shaderProgram;

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
	glDeleteProgram(shaderProgram);


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
	// ������ɫ��
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// �����ж�
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		wprintf(L"������ɫ������ʧ�ܣ�\n");
	}

	// Ƭ����ɫ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// �����ж�
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		wprintf(L"Ƭ����ɫ������ʧ�ܣ�\n");
	}

	// ����
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// �����ж�
	glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		wprintf(L"Ƭ����ɫ������ʧ�ܣ�\n");
	}

	// ɾ��
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


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
	glUseProgram(shaderProgram);

	float timeValue = glfwGetTime();
	float greenValue = sin(timeValue) / 5.0f + 0.5f;
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

	// ����������
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

}
