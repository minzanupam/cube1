#include "basic_shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

std::string BasicShader::read_file(const char *path) {
	unsigned int shader;
	int status, len;
	char log[SHADER_ERROR_LOG_LEN];
	std::ifstream fs(path);
	std::stringstream ss;
	ss << fs.rdbuf();
	std::string str = ss.str();
	return str;
}

BasicShader::BasicShader(const char *vertexShaderPath,
			 const char *fragmentShaderPath) {
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string vertexShaderString = read_file(vertexShaderPath);
	std::string fragmentShaderString = read_file(fragmentShaderPath);
	const char *vertexShaderCode = vertexShaderString.c_str();
	const char *fragmentShaderCode = fragmentShaderString.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);

	this->ID = glCreateProgram();

	int status, len;
	char log[SHADER_ERROR_LOG_LEN];
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, SHADER_ERROR_LOG_LEN, &len,
				   log);
		std::cout << log << std::endl;
		std::cout << "**GL Shader Error : vertex shader : "
			  << vertexShaderPath << " **" << std::endl
			  << vertexShaderCode << std::endl;
	} else {
		glAttachShader(this->ID, vertexShader);
	}
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(fragmentShader, SHADER_ERROR_LOG_LEN, &len,
				   log);
		std::cout << log << std::endl;
		std::cout << "**GL Shader Error : fragment shader : "
			  << fragmentShaderPath << " **" << std::endl
			  << fragmentShaderCode << std::endl;
	} else {
		glAttachShader(this->ID, fragmentShader);
	}
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(this->ID, SHADER_ERROR_LOG_LEN, &len, log);
		std::cout << log << std::endl;
		exit(1);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void BasicShader::use() { glUseProgram(this->ID); }
