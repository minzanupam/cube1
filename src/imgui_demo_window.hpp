#ifndef _IMGUI_DEMO_WINDOW_HPP
#define _IMGUI_DEMO_WINDOW_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void imgui_demo_window(bool show_demo_window, glm::vec3 &camera_eye,
		       glm::vec3 &camera_center, float &camera_fov,
		       glm::vec3 &lightcube_pos);

#endif
