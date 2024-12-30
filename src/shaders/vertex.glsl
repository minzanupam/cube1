#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 nor;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// nor = vec3(1.0, 0.5, 0.5);
	nor = aNor;
}
