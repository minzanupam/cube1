cmake_minimum_required(VERSION 3.10)

project(Cube)

set(IMGUI_DIR libs/imgui-1.91.1)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw GLEW GL X11 GLU OpenGL)
