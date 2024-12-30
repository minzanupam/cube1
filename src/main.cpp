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
	unsigned int VAO_asset, VBO_asset;
	unsigned int VAO_lightcube, VBO_lightcube, EBO_lightcube;
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

	glGenVertexArrays(1, &VAO_asset);
	glGenBuffers(1, &VBO_asset);
	glBindVertexArray(VAO_asset);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_asset);

	FILE *asset_file = fopen("../assets/teapot_bezier0.norm.txt", "r");
	if (asset_file == NULL) {
		fprintf(stderr, "failed to open asset file\n");
		return -1;
	}
	int asset_triangle_count;
	fscanf(asset_file, "%d", &asset_triangle_count);
	float *asset_vertices =
		(float *)malloc(18 * sizeof(float) * asset_triangle_count);
	for (int i = 0; i < asset_triangle_count * 18; i += 6) {
		fscanf(asset_file, "%f %f %f", &asset_vertices[i],
			   &asset_vertices[i + 1], &asset_vertices[i + 2]);
		fscanf(asset_file, "%f %f %f", &asset_vertices[i + 3],
			   &asset_vertices[i + 4], &asset_vertices[i + 5]);
	}
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float) * asset_triangle_count,
				 asset_vertices, GL_STATIC_DRAW);

	float vertices_lightcube[] = {
		// Front face
		0.5f, 0.5f, 0.5f,	// Top right of the front face
		0.5f, -0.5f, 0.5f,	// Bottom right of the front face
		-0.5f, -0.5f, 0.5f, // Bottom left of the front face
		-0.5f, 0.5f, 0.5f,	// Top left of the front face

		// Back face
		0.5f, 0.5f, -0.5f,	 // Top right of the back face
		0.5f, -0.5f, -0.5f,	 // Bottom right of the back face
		-0.5f, -0.5f, -0.5f, // Bottom left of the back face
		-0.5f, 0.5f, -0.5f	 // Top left of the back face
	};
	unsigned int indices_lightcube[] = {
		// Front face
		0, 1, 3, // First triangle
		1, 2, 3, // Second triangle

		// Back face
		4, 5, 7, // First triangle
		5, 6, 7, // Second triangle

		// Top face
		0, 1, 5, // First triangle
		1, 4, 5, // Second triangle

		// Bottom face
		2, 3, 7, // First triangle
		3, 6, 7, // Second triangle

		// Left face
		0, 3, 7, // First triangle
		0, 7, 4, // Second triangle

		// Right face
		1, 2, 6, // First triangle
		2, 5, 6	 // Second triangle
	};
	glGenVertexArrays(1, &VAO_lightcube);
	glBindVertexArray(VAO_lightcube);
	glGenBuffers(1, &VBO_lightcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_lightcube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_lightcube),
				 vertices_lightcube, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO_lightcube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightcube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_lightcube),
				 indices_lightcube, GL_STATIC_DRAW);

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

	glm::vec3 camera_eye = glm::vec3(0.0f, 0.0f, 15.0f);
	glm::vec3 camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	float camera_fov = 45.0f;

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
		view =
			glm::lookAt(camera_eye, camera_center, glm::vec3(0.0f, 1.0f, 0.0f));
		projection =
			glm::perspective(glm::radians(camera_fov),
							 (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);
		glBindVertexArray(VAO_asset);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_asset);
		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
							  NULL);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3 * asset_triangle_count);

		glBindVertexArray(VAO_lightcube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_lightcube);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightcube);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, -3.0f));
		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
							  NULL);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// ImGui::ShowDemoWindow(&show_demo_window);
		if (show_demo_window) {
			ImGui::Begin("Camera", &show_demo_window);
			ImGui::BeginGroup();
			{
				ImGui::Text("eye pos");
				ImGui::DragFloat("ce-x", &camera_eye.x, 0.05f);
				ImGui::DragFloat("ce-y", &camera_eye.y, 0.05f);
				ImGui::DragFloat("ce-z", &camera_eye.z, 0.05f);
			}
			ImGui::EndGroup();
			ImGui::BeginGroup();
			{
				ImGui::Text("center pos");
				ImGui::DragFloat("cc-x", &camera_center.x, 0.05f);
				ImGui::DragFloat("cc-y", &camera_center.y, 0.05f);
				ImGui::DragFloat("cc-z", &camera_center.z, 0.05f);
			}
			ImGui::EndGroup();
			ImGui::BeginGroup();
			{
				ImGui::Text("camera other");
				ImGui::DragFloat("fov", &camera_fov, 0.05f);
			}
			ImGui::EndGroup();
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
