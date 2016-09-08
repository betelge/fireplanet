// @jasminko

#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>

class Shader
{
public:
	Shader(std::string vertexSource, std::string fragmentSource);
	~Shader();

	void initialize();

	GLuint getHandle();
	bool issuccessfullyLinked();
	std::string getErrorLog();

private:
	std::string vertexSource_;
	std::string fragmentSource_;

	GLuint handle_ = 0;
	GLuint vertexShader_ = 0;
	GLuint fragmentShader_ = 0;
	bool issuccessfullyLinked_ = false;
	std::string errorLog_;

	bool checkShader(GLuint shaderObject);
	bool checkProgram(GLuint programObject);
};

#endif // SHADER_H