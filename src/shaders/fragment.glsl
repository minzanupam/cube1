#version 330 core

in vec3 frag_pos;
in vec3 frag_nor;

out vec3 color;

uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform vec3 light_color;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

void main() {
	vec3 light_direction = normalize(light_pos - frag_pos);
	vec3 normal = normalize(frag_nor);
	vec3 view_direction = normalize(camera_pos - frag_pos);
	vec3 reflection_direction = reflect(-light_direction, normal);

	vec3 ambient = light_color * material.ambient;
	float diff = max(dot(normal, light_direction), 0.0);
	vec3 diffuse = light_color * (diff * material.diffuse);
	float spec = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
	vec3 specular = light_color * spec * material.specular;

	color = ambient + diffuse + specular;
}
