#ifndef _BASIC_SHADER_HPP
#define _BASIC_SHADER_HPP

#define SHADER_ERROR_LOG_LEN 1024

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class BasicShader {
      private:
	std::string read_file(const char *path);

      public:
	unsigned int ID; // program ID

	BasicShader(const char *vertexShaderPath,
		    const char *fragmentShaderPath);

	void setMat4(const char *name, glm::mat4 value);

	void setVec3(const char *name, glm::vec3 value);

	void setFloat(const char *name, float value);

	void use();
};

#endif
