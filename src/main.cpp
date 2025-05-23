#include "basic_shader.hpp"
#include "imgui.h"
#include "imgui_demo_window.hpp"
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
#define SHADOW_WIDTH 1280
#define SHADOW_HEIGHT 720

struct UMaterial {
	unsigned int ambient;
	unsigned int diffuse;
	unsigned int specular;
	unsigned int shininess;
};

struct ULight {
	unsigned int position;
	unsigned int ambient;
	unsigned int diffuse;
	unsigned int specular;
};

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

void configurePhongShader(BasicShader *shader, glm::mat4 model, glm::mat4 view,
			  glm::mat4 projection, glm::vec3 camera_eye,
			  glm::vec3 lightcube_pos, float material[10]) {
	// MVP
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	// camera
	shader->setVec3("camera_pos", camera_eye);

	// material
	shader->setVec3("material.ambient",
			glm::vec3(material[0], material[1], material[2]));
	shader->setVec3("material.diffuse",
			glm::vec3(material[3], material[4], material[5]));
	shader->setVec3("material.specular",
			glm::vec3(material[6], material[7], material[8]));
	shader->setFloat("material.shininess", 128.0f * material[9]);

	// light
	shader->setVec3("light.position", lightcube_pos);
	shader->setVec3("light.ambient", glm::vec3(0.6f, 0.6f, 0.6f));
	shader->setVec3("light.diffuse", glm::vec3(0.9f, 0.9f, 0.9f));
	shader->setVec3("light.specular", glm::vec3(4.0f, 4.0f, 4.0f));
}

void configureLightcubeShader(BasicShader *shader, glm::mat4 model,
			      glm::mat4 view, glm::mat4 projection,
			      glm::vec3 lightcube_pos) {
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	// Value for these are essentially copies of the values used
	// in the light asset.
	shader->setVec3("light.position", lightcube_pos);
	shader->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

int main() {
	unsigned int VAO_asset, VBO_asset;
	unsigned int VAO_lightcube, VBO_lightcube, EBO_lightcube;
	unsigned int VAO_ground, VBO_ground, EBO_ground;
	unsigned int vertexShader, fragmentShader, program;
	unsigned int u_Model, u_View, u_Projection;
	unsigned int u_cameraPos;

	struct UMaterial u_Material;
	struct ULight u_Light;

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "failed to init glfw\n");
		return -1;
	}
	GLFWwindow *window =
	    glfwCreateWindow(WIDTH, HEIGHT, "cube1", NULL, NULL);
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
	float vertices_cube[] = {
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

	unsigned int indices_cube[] = {
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
	    6, 5, 1  // Triangle 2
	};

	float vertices_cube2[] = {
	    // front face
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Vertex 0
	    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // Vertex 1
	    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	  // Vertex 2
	    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	  // Vertex 2
	    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // Vertex 3
	    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Vertex 0

	    // back face
	    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // Vertex 4
	    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // Vertex 5
	    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,   // Vertex 6
	    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,   // Vertex 6
	    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // Vertex 7
	    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // Vertex 4

	    // top face
	    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 3
	    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,	  // Vertex 2
	    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 6
	    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 6
	    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 7
	    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 3

	    // bottom face
	    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,  // Vertex 0
	    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,   // Vertex 1
	    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  // Vertex 5
	    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,  // Vertex 5
	    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, // Vertex 4
	    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,  // Vertex 0

	    // left face
	    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  // Vertex 0
	    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,   // Vertex 3
	    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  // Vertex 7
	    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  // Vertex 7
	    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // Vertex 4
	    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  // Vertex 0

	    // Right face
	    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 1
	    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,	  // Vertex 2
	    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 6
	    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 6
	    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Vertex 5
	    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 1
	};

	glGenVertexArrays(1, &VAO_lightcube);
	glBindVertexArray(VAO_lightcube);
	glGenBuffers(1, &VBO_lightcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_lightcube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube,
		     GL_STATIC_DRAW);
	glGenBuffers(1, &EBO_lightcube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightcube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_cube),
		     indices_cube, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO_ground);
	glBindVertexArray(VAO_ground);
	glGenBuffers(1, &VBO_ground);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ground);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube2), vertices_cube2,
		     GL_STATIC_DRAW);

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

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	glm::vec3 camera_eye = glm::vec3(0.0f, 4.0f, 8.0f);
	glm::vec3 camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	float camera_fov = 45.0f;
	glm::vec3 lightcube_pos = glm::vec3(2.0f, 2.0f, 3.5f);

	BasicShader *shader_phong =
	    new BasicShader("../src/shaders/vertex_phong.glsl",
			    "../src/shaders/fragment_blinn_phong.glsl");

	BasicShader *shader_lightcube =
	    new BasicShader("../src/shaders/vertex_lightcube.glsl",
			    "../src/shaders/fragment_lightcube.glsl");

	float copper[10] = {// ambient 3, diffuse 3, specular 3, shininess 1
			    0.19125, 0.0735,   0.0225,	 0.7038,   0.27048,
			    0.0828,  0.256777, 0.137622, 0.086014, 0.1};

	float white_plastic[] = {
	    // ambient 3, diffuse 3, specular 3, shininess 1
	    0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, 0.25};

	BasicShader *shader_depthmap =
	    new BasicShader("../src/shaders/vertex_simple_depth.glsl",
			    "../src/shaders/fragment_empty.glsl");

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.04313725, 0.1803921, 0.1607843, 1.0);

		// kettle
		model = glm::translate(glm::mat4(1.0f),
				       glm::vec3(0.0f, -1.0f, -1.0f));
		view = glm::lookAt(camera_eye, camera_center,
				   glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(camera_fov),
					      (float)WIDTH / (float)HEIGHT,
					      0.01f, 100.0f);

		shader_phong->use();
		configurePhongShader(shader_phong, model, view, projection,
				     camera_eye, lightcube_pos, copper);

		glBindVertexArray(VAO_asset);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_asset);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
				      6 * sizeof(float), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
				      6 * sizeof(float),
				      (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 3 * asset_triangle_count);

		// ground

		model =
		    glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 0.1f, 10.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
		configurePhongShader(shader_phong, model, view, projection,
				     camera_eye, lightcube_pos, white_plastic);

		glBindVertexArray(VAO_ground);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_ground);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
				      6 * sizeof(float), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
				      6 * sizeof(float),
				      (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::translate(glm::mat4(1.0f), lightcube_pos);
		shader_lightcube->use();
		configureLightcubeShader(shader_lightcube, model, view,
					 projection, lightcube_pos);

		glBindVertexArray(VAO_lightcube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_lightcube);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightcube);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
				      3 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// ImGui::ShowDemoWindow(&show_demo_window);
		if (show_demo_window) {
			imgui_demo_window(show_demo_window, camera_eye,
					  camera_center, camera_fov,
					  lightcube_pos);
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
