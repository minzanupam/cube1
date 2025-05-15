#ifndef _BASIC_SHADER_HPP
#define _BASIC_SHADER_HPP

#define SHADER_ERROR_LOG_LEN 1024

#include <string>

class BasicShader {
      private:
	std::string read_file(const char *path);

      public:
	unsigned int ID; // program ID
	BasicShader(const char *vertexShaderPath,
		    const char *fragmentShaderPath);
	void use();
};

#endif
