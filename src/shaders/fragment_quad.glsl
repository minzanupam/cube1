#version 330 core

in vec2 TexCoord;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main() {
	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoord)), 1.0);
}
