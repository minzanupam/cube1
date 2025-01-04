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
	float Kc = 1.0;
	float Kl = 0.09;
	float Kq = 0.032;

	vec3 light_direction = normalize(light.position - frag_pos);
	vec3 normal_direction = normalize(frag_nor);
	vec3 view_direction = normalize(camera_pos - frag_pos);
	vec3 reflection_direction = reflect(-light_direction, normal_direction);
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (Kc + (Kl*distance) + Kq * (distance * distance));

	vec3 ambient = attenuation * light.ambient * material.ambient;
	float diff = max(dot(normal_direction, light_direction), 0.0);
	vec3 diffuse = attenuation * light.diffuse * (diff * material.diffuse);
	float spec = pow(max(dot(view_direction, reflection_direction), 0.0), material.shininess);
	vec3 specular = attenuation * light.specular * spec * material.specular;
	float gamma = 1.3;

	color = pow(ambient + diffuse + specular, vec3(1.0/gamma));
}
