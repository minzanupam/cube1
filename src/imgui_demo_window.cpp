#include "imgui_demo_window.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void imgui_demo_window(bool show_demo_window, glm::vec3 camera_eye,
					   glm::vec3 camera_center, float camera_fov,
					   glm::vec3 lightcube_pos) {
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
