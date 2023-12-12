#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <clocale>
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "FPSMeter.h"
#include <sys/timeb.h>
#include "timmer.h"
#include <format>
#include "EncodeTool.h"
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void build();
void Render();
unsigned int LoadTexture(const char* path);
GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam);

HWND hwnd;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float cameraSpeed = 0.05f;

Shader* pCubeShader = NULL, * pLightShader = NULL;

double deltaTime = 1.0 / 60.0;
unsigned int VBO, cubeVAO, lightCubeVAO;
unsigned int texture1, texture2;
//unsigned int shaderProgram;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch = 0.0f, yaw = -90.0f;
float lastX = 800.0f / 2.0f, lastY = 600.0f / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

unsigned int diffuseMap, specularMap;

int main()
{
	// 设置控制台的中文显示
	setlocale(LC_ALL, "chs");

	// 初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SRender", NULL, NULL);
	if (window == NULL)
	{
		wprintf(L"创建窗口失败！\n");
		glfwTerminate();
		return -1;
	}
	hwnd = glfwGetWin32Window(window);

	// 显示窗口
	glfwMakeContextCurrent(window);
	// 捕获鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// 回调
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// GLAD初始化
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		wprintf(L"GLAD初始化错误！\n");
		glfwTerminate();
		return -1;
	}

#if _DEBUG
	glGetError();
	// 调试输出
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	//glViewport(0, 0, 800, 600);
	// 深度测试
	glEnable(GL_DEPTH_TEST);
	build();
	diffuseMap = LoadTexture("res\\container2.png");
	specularMap = LoadTexture("res\\container2_specular.png");
	pCubeShader->use();
	pCubeShader->set("material.diffuse", 0);
	pCubeShader->set("material.specular", 1);

	FPSMeter fpsMeter(hwnd, L"SRender");
	timeb tb;
	ftime(&tb);
	double prevTime = tb.time + 1.0 * tb.millitm / 1000;
	double delay = 0;
	while (!glfwWindowShouldClose(window))
	{
		fpsMeter.TickStart();
		double curTime = tb.time + 1.0 * tb.millitm / 1000;
		double elapsedTime = curTime - prevTime;
		prevTime = curTime;
		// 如果游戏处于调试状态就别跳帧了
		if (elapsedTime > 0.25)
			elapsedTime = 0.25;
		delay += elapsedTime;
		while (delay >= deltaTime) {

			delay -= deltaTime;
		}
		processInput(window);
		// 渲染指令
		Render();
		fpsMeter.TickEnd();

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
		double finTime = tb.time + 1.0 * tb.millitm / 1000;
		double processTime = finTime - curTime;
		sleep(std::max((deltaTime - processTime) * 1000, 0.0));
	}

	// Delete
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(pCubeShader->ID);
	glDeleteProgram(pLightShader->ID);
	delete pCubeShader;
	delete pLightShader;
	//glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
}

void build()
{
	pCubeShader = new Shader("shader\\cube.vert", "shader\\cube.frag");
	pLightShader = new Shader("shader\\light.vert", "shader\\light.frag");


	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	// 绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	// 点
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// 灯
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}
float opactity = 0.0f;
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};
void Render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// 激活着色器
	pCubeShader->use();

	pCubeShader->set("viewPos", cameraPos);
	pCubeShader->set("material.shininess", 32.0f);

	// 太阳光
	pCubeShader->set("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	pCubeShader->set("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	pCubeShader->set("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	pCubeShader->set("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	// 点光
	pCubeShader->set("pointLightNum", 4);
	for (unsigned int i = 0; i < 4; i++)
	{
		std::string preString = std::format("pointLights[{}]", i);
		auto w = preString + ".position";
		pCubeShader->set((preString + ".position").c_str(), pointLightPositions[i]);
		pCubeShader->set((preString + ".constant").c_str(), 1.0f);
		pCubeShader->set((preString + ".linear").c_str(), 0.09f);
		pCubeShader->set((preString + ".quadratic").c_str(), 0.032f);
		pCubeShader->set((preString + ".ambient").c_str(), glm::vec3(0.05f, 0.05f, 0.05f));
		pCubeShader->set((preString + ".diffuse").c_str(), glm::vec3(0.8f, 0.8f, 0.8f));
		pCubeShader->set((preString + ".specular").c_str(), glm::vec3(1.0f, 1.0f, 1.0f));
	}
	// 聚光
	pCubeShader->set("spotLight.position", cameraPos);
	pCubeShader->set("spotLight.direction", cameraFront);
	pCubeShader->set("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	pCubeShader->set("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	pCubeShader->set("spotLight.constant", 1.0f);
	pCubeShader->set("spotLight.linear", 0.09f);
	pCubeShader->set("spotLight.quadratic", 0.032f);
	pCubeShader->set("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	pCubeShader->set("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	pCubeShader->set("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));


	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	pCubeShader->set("projection", projection);
	pCubeShader->set("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	pCubeShader->set("model", model);

	//pCubeShader->set("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	glBindVertexArray(cubeVAO);

	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		pCubeShader->set("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// 渲染灯
	pLightShader->use();
	pLightShader->set("projection", projection);
	pLightShader->set("view", view);
	glBindVertexArray(lightCubeVAO);
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f));
		pLightShader->set("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
#if _DEBUG
	glCheckError();
#endif

}
unsigned int LoadTexture(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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
		std::wcout << "读取纹理失败！" << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::wstring msg = s2ws(std::format("{} | {} ({})", error, file, line));
		MessageBox(NULL, msg.c_str(), L"OpenGL Error", MB_OK | MB_ICONEXCLAMATION);
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
	std::wstring msg = s2ws(std::format("---------------\nOpenGL Debug message ({}) : {}\n", id, message));
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		msg += L"Source: API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		msg += L"Source: Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		msg += L"Source: Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		msg += L"Source: Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		msg += L"Source: Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		msg += L"Source: Other";
		break;
	default:
		break;
	}
	msg += L"\n";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		msg += L"Type: Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		msg += L"Type: Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		msg += L"Type: Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		msg += L"Type: Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		msg += L"Type: Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		msg += L"Type: Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		msg += L"Type: Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		msg += L"Type: Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		msg += L"Type: Other";
		break;
	default:
		break;
	}
	msg += L"\n";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		msg += L"Severity: high";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		msg += L"Severity: medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		msg += L"Severity: low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		msg += L"Severity: notification";
		break;
	default:
		break;
	}
	msg += L"\n\n";
	std::cout << ws2s(msg);
}