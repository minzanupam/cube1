#version 330 core

in vec3 frag_pos;
in vec3 frag_nor;

out vec3 color;

uniform float ambient_light;
uniform vec3 object_color;
uniform vec3 light_pos;

void main() {
	vec3 light_direction = normalize(light_pos - frag_pos);
	vec3 normal = normalize(frag_nor);
	float diffuse_light = max(dot(normal, light_direction), 0.0);
	color = (ambient_light + diffuse_light) * object_color;
}
