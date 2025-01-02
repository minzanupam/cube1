#version 330 core

out vec3 color;

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;


void main() {
	color = light.ambient + light.diffuse + light.specular;
}
