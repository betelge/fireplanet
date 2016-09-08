#include <fstream>
#include <iostream>

#include <GL/glew.h>
// @jasminko

#include <GL/freeglut.h>

#include "shader.h"
#include "lodepng.h";

#define VERTEX_SOURCE "vertex.glsl"
#define FRAGMENT_SOURCE "fragment.glsl"
#define NOISE_SOURCE "noise3D.glsl"
#define TEXTURE_FILE "../assets/fire_profile.png"

#define WINDOW_SIZE 512

Shader* planetShader = nullptr;
GLuint texture = 0;

std::string loadFile(std::string filename)
{
	std::ifstream file(filename);

	if (!file)
	{
		std::cerr << "Can't load " << filename << std::endl;
		return "";
	}

	std::string content ((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));

	return content;
}

GLuint loadTexture(std::string filename)
{
	//load and decode
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	if (error)
		std::cout << "Texture decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	GLuint textureHandle = 0;
	glGenTextures(1, &textureHandle);

	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLenum err = glGetError();

	return textureHandle;
}

bool init()
{
	// Load shader sources
	std::string vertexSource = loadFile(VERTEX_SOURCE);
	std::string fragmentSource = loadFile(FRAGMENT_SOURCE);
	std::string noiseSource = loadFile(NOISE_SOURCE); // Contains the noise function used in the fragment shader

	// Compile and link shader
	planetShader = new Shader(vertexSource, fragmentSource + noiseSource);
	planetShader->initialize();

	if (!planetShader->issuccessfullyLinked())
	{
		std::cerr << "Shader was not sucessfully compiled and linked" << std::endl;
		std::cerr << "Log:\n" + planetShader->getErrorLog() << std::endl;

		return false;
	}

	// This will be the only shader program in use, so we can activate it during init
	glUseProgram(planetShader->getHandle());

	// Load texture
	texture = loadTexture(TEXTURE_FILE);

	return true;
}

// Fix window size
void resize(int width, int height) {
	glutReshapeWindow(WINDOW_SIZE, WINDOW_SIZE);
}

// display callback
void display() {
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint timeUniform = glGetUniformLocation(planetShader->getHandle(), "time");
	int time = glutGet(GLUT_ELAPSED_TIME); // Gets application time in milliseconds

	// Make time 64 second periodic and normalize it to 1, meaning 0 - 1 is 64 seconds
	float timef = (time % 64000) / (float) 64000;
	glUniform1f(timeUniform, timef);

	// Bind our texture to unit 0 and set sampler uniform accordingly
	const int TEXTURE_UNIT = 0;
	glActiveTexture(GL_TEXTURE0 + TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLuint samplerUniform = glGetUniformLocation(planetShader->getHandle(), "texture");
	glUniform1i(samplerUniform, TEXTURE_UNIT);

	/*
		Draws a full screen triangle.
		No vertex attributes are used. Instead the vertex shader uses gl_VertexID to span the triangle
		and the fragment shader uses texture coordinates to draw a rond shaded planet.
		*/
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	// request version
	glutInitContextVersion(3, 0);

	// double buffered, color, no depth, no alpha
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutCreateWindow("Fireplanet");

	glewExperimental = true; // Needed for newer OpenGL versions
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "glewInit failed" << std::endl;
		return -1;
	}

	glutDisplayFunc(display);
	glutIdleFunc(display); // This forces constant redrawing
	glutReshapeFunc(resize);

	bool success = init();

	if (!success)
	{
		std::cerr << "Program initialization failed" << std::endl;
		return -1;
	}

	glutMainLoop();

	if (planetShader != nullptr)
		delete planetShader;
	
	return 0;
}
