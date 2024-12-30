#version 330 core

in vec3 nor;

out vec3 color;

uniform float ambient_light;
uniform vec3 object_color;
uniform vec3 light_pos;

void main() {
	color = ambient_light * object_color;
}
