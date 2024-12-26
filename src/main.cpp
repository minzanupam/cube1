#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define WIDTH 1280
#define HEIGHT 720

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
								GLenum severity, GLsizei length,
								const GLchar *message, const void *userParam) {
	fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

int main() {
	unsigned int VAO, VBO;
	unsigned int vertexShader, fragmentShader, program;
	if (!glfwInit()) {
		fprintf(stderr, "failed to init glfw\n");
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "cube1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "failed to init glew\n");
		return -1;
	}
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, ///
		0.0f,  1.0f,  0.0f, ///
		1.0f,  -1.0f, 0.0f, ///
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	int status, len;
#define SHADER_ERROR_LOG_LEN 1024
	char log[SHADER_ERROR_LOG_LEN];

	std::ifstream fs_vertexShader("../src/shaders/vertex.glsl");
	std::stringstream ss_vertexShader;
	ss_vertexShader << fs_vertexShader.rdbuf();
	std::string s_vertexShader = ss_vertexShader.str();
	const char *c_vertexShader = s_vertexShader.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &c_vertexShader, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
	}

	std::ifstream fs_fragmentShader("../src/shaders/fragment.glsl");
	std::stringstream ss_fragmentShader;
	ss_fragmentShader << fs_fragmentShader.rdbuf();
	std::string s_fragmentShader = ss_fragmentShader.str();
	const char *c_fragmentShader = s_fragmentShader.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &c_fragmentShader, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(fragmentShader, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(program, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(program);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	return 0;
}
