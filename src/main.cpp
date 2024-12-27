#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

static void glfw_error_callback(int error, const char *description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
	unsigned int VAO, VBO;
	unsigned int vertexShader, fragmentShader, program;
	unsigned int u_Model, u_View, u_Projection;
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "failed to init glfw\n");
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "cube1", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // enable vsync, sync framerate to monitor
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |=
		ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char *glsl_version = "#version 330 core";
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	u_Model = glGetUniformLocation(program, "model");
	u_View = glGetUniformLocation(program, "view");
	u_Projection = glGetUniformLocation(program, "projection");

	glUseProgram(program);

	float camera_posz = 1.0f;
	float f32_zero = 0.0f;
	float f32_one = 1.0f;

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
		view = glm::lookAt(glm::vec3(0.0f, 1.0f, camera_posz),
						   glm::vec3(0.0f, 0.0f, 0.0f),
						   glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(
			glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);
		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// ImGui::ShowDemoWindow(&show_demo_window);
		if (show_demo_window) {
			ImGui::Begin("My Demo Window", &show_demo_window);
			ImGui::Text("hello world");
			ImGui::DragFloat("camera pos-z", &camera_posz, 0.005f);
			if (ImGui::Button("close")) {
				show_demo_window = false;
			}
			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
