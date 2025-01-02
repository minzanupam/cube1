#version 330 core

in vec3 frag_pos;
in vec3 frag_nor;

out vec3 color;

uniform vec3 camera_pos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

void main() {
	vec3 light_direction = normalize(light.position - frag_pos);
	vec3 normal_direction = normalize(frag_nor);
	vec3 view_direction = normalize(camera_pos - frag_pos);

	vec3 halfway = (light_direction + view_direction) / length(light_direction + view_direction);

	vec3 ambient = light.ambient * material.ambient;
	float diff = max(dot(normal_direction, light_direction), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	float spec = pow(max(dot(halfway, normal_direction), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular;
	float gamma = 1.5;

	color = pow(ambient + diffuse + specular, vec3(1.0/gamma));
}
