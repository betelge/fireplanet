// @jasminko

#include "shader.h"

Shader::Shader(std::string vertexSource, std::string fragmentSource)
{
	vertexSource_ = vertexSource;
	fragmentSource_ = fragmentSource;
}

Shader::~Shader()
{
	// Clean up

	if (handle_)
		glDeleteProgram(handle_);

	if (vertexShader_)
		glDeleteShader(vertexShader_);

	if (fragmentShader_)
		glDeleteShader(fragmentShader_);
}

void Shader::initialize()
{
	// Generate shader handles
	vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);

	// Pass GLSL source to shader objects
	const char *vertex_c_str = vertexSource_.c_str();
	const char *fragment_c_str = fragmentSource_.c_str();
	const int vertexSourceLength = vertexSource_.length();
	const int fragmentSourceLength = fragmentSource_.length();
	glShaderSource(vertexShader_, 1, &vertex_c_str, &vertexSourceLength);
	glShaderSource(fragmentShader_, 1, &fragment_c_str, &fragmentSourceLength);

	// Compile shader objects
	glCompileShader(vertexShader_);
	glCompileShader(fragmentShader_);

	// Check for compilation errors
	{
		bool success;
		success = checkShader(vertexShader_);

		if (!success)
			return;

		success = checkShader(fragmentShader_);

		if (!success)
			return;
	}

	// Generate a program object handle
	handle_ = glCreateProgram();

	// Attach shaders to program
	glAttachShader(handle_, vertexShader_);
	glAttachShader(handle_, fragmentShader_);

	// Link program
	glLinkProgram(handle_);

	// Check for program linking errors
	bool success = checkProgram(handle_);

	issuccessfullyLinked_ = success;
}

bool Shader::checkShader(GLuint shaderObject)
{
	GLint compiled;
	glGetObjectParameterivARB(shaderObject, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint logLength;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);

		GLchar* log = (GLchar*)malloc(logLength);
		glGetShaderInfoLog(shaderObject, logLength, &logLength, log);
		errorLog_.assign(log, logLength);

		return false;
	}

	return true;
}

bool Shader::checkProgram(GLuint programObject)
{
	GLint linked;
	glGetObjectParameterivARB(programObject, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint logLength;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &logLength);

		GLchar* log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(programObject, logLength, &logLength, log);
		errorLog_.assign(log, logLength);

		return false;
	}

	return true;
}

GLuint Shader::getHandle()
{
	return handle_;
}

bool Shader::issuccessfullyLinked()
{
	return issuccessfullyLinked_;
}

std::string Shader::getErrorLog()
{
	return errorLog_;
}