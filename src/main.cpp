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
#define SHADER_ERROR_LOG_LEN 1024

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

unsigned int load_shader(const char *path, GLenum type) {
	unsigned int shader;
	int status, len;
	char log[SHADER_ERROR_LOG_LEN];
	std::ifstream fs_shader(path);
	std::stringstream ss_shader;
	ss_shader << fs_shader.rdbuf();
	std::string s_shader = ss_shader.str();
	const char *c_shader = s_shader.c_str();
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &c_shader, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(shader, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
	}
	return shader;
}

int main() {
	unsigned int VAO_asset, VBO_asset;
	unsigned int VAO_lightcube, VBO_lightcube, EBO_lightcube;
	unsigned int vertexShader, fragmentShader, program;
	unsigned int u_Model, u_View, u_Projection;
	unsigned int u_cameraPos;
	unsigned int u_MaterialAmbient, u_MaterialDiffuse, u_MaterialSpecular,
		u_MaterialShininess;
	unsigned int u_LightPosition, u_LightAmbient, u_LightDiffuse,
		u_LightSpecular;

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

	glEnable(GL_DEPTH_TEST);

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
		-0.5f, -0.5f, 0.5f, // Vertex 0
		0.5f, -0.5f, 0.5f,	// Vertex 1
		0.5f, 0.5f, 0.5f,	// Vertex 2
		-0.5f, 0.5f, 0.5f,	// Vertex 3

		// Back face
		-0.5f, -0.5f, -0.5f, // Vertex 4
		0.5f, -0.5f, -0.5f,	 // Vertex 5
		0.5f, 0.5f, -0.5f,	 // Vertex 6
		-0.5f, 0.5f, -0.5f,	 // Vertex 7
	};

	unsigned int indices_lightcube[] = {
		// Front face
		0, 1, 2, // Triangle 1
		2, 3, 0, // Triangle 2

		// Back face
		4, 5, 6, // Triangle 1
		6, 7, 4, // Triangle 2

		// Top face
		3, 2, 6, // Triangle 1
		6, 7, 3, // Triangle 2

		// Bottom face
		0, 1, 5, // Triangle 1
		5, 4, 0, // Triangle 2

		// Left face
		0, 3, 7, // Triangle 1
		7, 4, 0, // Triangle 2

		// Right face
		1, 2, 6, // Triangle 1
		6, 5, 1	 // Triangle 2
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

	vertexShader =
		load_shader("../src/shaders/vertex_phong.glsl", GL_VERTEX_SHADER);
	fragmentShader =
		load_shader("../src/shaders/fragment_phong.glsl", GL_FRAGMENT_SHADER);

	int status, len;
	char log[SHADER_ERROR_LOG_LEN];

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

	unsigned int vertexShader_lightcube =
		load_shader("../src/shaders/vertex_lightcube.glsl", GL_VERTEX_SHADER);
	unsigned int fragmentShader_lightcube = load_shader(
		"../src/shaders/fragment_lightcube.glsl", GL_FRAGMENT_SHADER);

	unsigned int program_lightcube = glCreateProgram();
	glAttachShader(program_lightcube, vertexShader_lightcube);
	glAttachShader(program_lightcube, fragmentShader_lightcube);
	glLinkProgram(program_lightcube);
	glGetProgramiv(program_lightcube, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(program, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
	}
	glDeleteShader(vertexShader_lightcube);
	glDeleteShader(fragmentShader_lightcube);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	u_Model = glGetUniformLocation(program, "model");
	u_View = glGetUniformLocation(program, "view");
	u_Projection = glGetUniformLocation(program, "projection");
	u_cameraPos = glGetUniformLocation(program, "camera_pos");
	u_MaterialAmbient = glGetUniformLocation(program, "material.ambient");
	u_MaterialDiffuse = glGetUniformLocation(program, "material.diffuse");
	u_MaterialSpecular = glGetUniformLocation(program, "material.specular");
	u_MaterialShininess = glGetUniformLocation(program, "material.shininess");
	u_LightPosition = glGetUniformLocation(program, "light.position");
	u_LightAmbient = glGetUniformLocation(program, "light.ambient");
	u_LightDiffuse = glGetUniformLocation(program, "light.diffuse");
	u_LightSpecular = glGetUniformLocation(program, "light.specular");

	unsigned int u_LightPosition_lightcube =
		glGetUniformLocation(program_lightcube, "light.position");
	unsigned int u_LightAmbient_lightcube =
		glGetUniformLocation(program_lightcube, "light.ambient");
	unsigned int u_LightDiffuse_lightcube =
		glGetUniformLocation(program_lightcube, "light.diffuse");
	unsigned int u_LightSpecular_lightcube =
		glGetUniformLocation(program_lightcube, "light.specular");

	glm::vec3 camera_eye = glm::vec3(0.0f, 0.0f, 8.0f);
	glm::vec3 camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	float camera_fov = 45.0f;
	glm::vec3 lightcube_pos = glm::vec3(5.0f, 5.0f, 3.5f);

	float copper[10] = {// ambient 3, diffuse 3, specular 3, shininess 1
						0.19125, 0.0735,   0.0225,	 0.7038,   0.27048,
						0.0828,	 0.256777, 0.137622, 0.086014, 0.1};

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.04313725, 0.1803921, 0.1607843, 1.0);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
		view =
			glm::lookAt(camera_eye, camera_center, glm::vec3(0.0f, 1.0f, 0.0f));
		projection =
			glm::perspective(glm::radians(camera_fov),
							 (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);

		glUseProgram(program);
		glBindVertexArray(VAO_asset);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_asset);
		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
		glUniform3fv(u_cameraPos, 1, glm::value_ptr(camera_eye));
		glUniform3fv(
			u_MaterialAmbient, 1,
			glm::value_ptr(glm::vec3(copper[0], copper[1], copper[2])));
		glUniform3fv(
			u_MaterialDiffuse, 1,
			glm::value_ptr(glm::vec3(copper[3], copper[4], copper[5])));
		glUniform3fv(
			u_MaterialSpecular, 1,
			glm::value_ptr(glm::vec3(copper[6], copper[7], copper[8])));
		glUniform1f(u_MaterialShininess, 128.0f * copper[9]);
		glUniform3fv(u_LightPosition, 1, glm::value_ptr(lightcube_pos));
		glUniform3fv(u_LightAmbient, 1,
					 glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
		glUniform3fv(u_LightDiffuse, 1,
					 glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
		glUniform3fv(u_LightSpecular, 1,
					 glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
							  NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
							  (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, 3 * asset_triangle_count);

		glUseProgram(program_lightcube);
		glBindVertexArray(VAO_lightcube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_lightcube);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightcube);
		model = glm::translate(glm::mat4(1.0f), lightcube_pos);
		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
		glUniform3fv(u_LightPosition_lightcube, 1,
					 glm::value_ptr(lightcube_pos));
		glUniform3fv(u_LightAmbient_lightcube, 1,
					 glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
		glUniform3fv(u_LightDiffuse_lightcube, 1,
					 glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
		glUniform3fv(u_LightSpecular_lightcube, 1,
					 glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

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
				ImGui::Text("lightcube position");
				ImGui::DragFloat("lc-x", &lightcube_pos.x, 0.05f);
				ImGui::DragFloat("lc-y", &lightcube_pos.y, 0.05f);
				ImGui::DragFloat("lc-z", &lightcube_pos.z, 0.05f);
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
