// @jasminko

#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>

// Encapsulates an OpenGL shader program object
class Shader
{
public:
	Shader(std::string vertexSource, std::string fragmentSource);
	~Shader();

	// Compiles and links everything. Has to be called before shader program use.
	void initialize();

	GLuint getHandle();
	bool issuccessfullyLinked();
	std::string getErrorLog();

private:
	std::string vertexSource_;
	std::string fragmentSource_;

	GLuint handle_ = 0; // Program object handle
	GLuint vertexShader_ = 0; // Vertex shader handle
	GLuint fragmentShader_ = 0; // Fragment shader object handle

	bool issuccessfullyLinked_ = false;
	std::string errorLog_;

	// Checks for successful compilation of shaders and linking of programs
	// Populates error log if needed
	bool checkShader(GLuint shaderObject);
	bool checkProgram(GLuint programObject);
};

#endif // SHADER_H