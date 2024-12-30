#version 330 core

in vec3 nor;

out vec3 color;

uniform float ambient_light;

void main() {
	color = nor * ambient_light;
}
