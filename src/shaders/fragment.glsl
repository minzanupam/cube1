#version 330 core

in vec3 frag_pos;
in vec3 frag_nor;

out vec3 color;

uniform float ambient_strength;
uniform vec3 object_color;
uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform vec3 light_color;

void main() {
	vec3 light_direction = normalize(light_pos - frag_pos);
	vec3 normal = normalize(frag_nor);
	vec3 view_direction = normalize(camera_pos - frag_pos);
	vec3 reflection_direction = reflect(-light_direction, normal);
	vec3 ambient = ambient_strength * light_color;
	vec3 specular = (0.5 * pow(max(dot(view_direction, reflection_direction), 0.0), 32)) * light_color;
	vec3 diffuse = max(dot(normal, light_direction), 0.0) * light_color;
	color = (ambient + diffuse + specular) * object_color;
}
