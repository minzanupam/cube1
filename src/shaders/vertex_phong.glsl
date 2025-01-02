#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 frag_nor;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	frag_pos = vec3(model * vec4(aPos, 1.0));
	frag_nor = aNor;
}
