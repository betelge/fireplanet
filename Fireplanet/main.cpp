// @jasminko

#include <fstream>
#include <iostream>

#include <GL/glew.h>

#include <GL/freeglut.h>

#include "shader.h"
#include "lodepng.h"

#define VERTEX_SOURCE "vertex.glsl"
#define FRAGMENT_SOURCE "fragment.glsl"
#define NOISE_SOURCE "noise3D.glsl" // The noise function called from the fragent shader
#define TEXTURE_FILE "../assets/fire_profile.png"

#define WINDOW_SIZE 512

Shader* planetShader = nullptr; // The shader program used to draw the planet
GLuint texture = 0; // The fire texture handle

std::string loadFile(std::string filename)
{
	std::ifstream file(filename);

	if (!file)
	{
		std::cerr << "Can't load " << filename << std::endl;
		return "";
	}

	// Populates a std::string with the file contents
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, // Populates texture in video memory
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set samling filters to smooth
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D); // Generates all higher mipmap levels

	// OpenGL error check for debug build
#ifdef _DEBUG
	GLenum err = glGetError();
#endif

	return textureHandle;
}

bool init()
{
	// Load shader sources
	std::string vertexSource = loadFile(VERTEX_SOURCE);
	std::string fragmentSource = loadFile(FRAGMENT_SOURCE);
	std::string noiseSource = loadFile(NOISE_SOURCE); // Contains the noise function used in the fragment shader

	// Compile and link shader
	planetShader = new Shader(vertexSource, fragmentSource + noiseSource); // Appends noise function
	planetShader->initialize();

	if (!planetShader->issuccessfullyLinked())
	{
		std::cerr << "Shader was not sucessfully compiled and linked" << std::endl;
		std::cerr << "Log:" << std::endl << planetShader->getErrorLog() << std::endl;

		return false;
	}

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

	// Bind shader program. The OpenGL driver should optimize away redundant calls.
	glUseProgram(planetShader->getHandle());

	glClearColor(0, 0, 0, 0); // Black
	glClear(GL_COLOR_BUFFER_BIT); // Clears color, ignores non-existing depth buffer

	// Make time 64 second periodic and normalize it to [0, 1]
	int time = glutGet(GLUT_ELAPSED_TIME); // Gets application time in milliseconds
	float timef = (time % 64000) / (float) 64000;

	// Set time uniform
	GLuint timeUniform = glGetUniformLocation(planetShader->getHandle(), "time");
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
		and the fragment shader uses texture coordinates to draw a round shaded planet.
		*/
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitContextVersion(2, 1); // Request OpenGL 2.1

	// Request a double buffered color frame buffer without depth and alpha
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutCreateWindow("Fireplanet");

	// Initialize GLEW
	glewExperimental = true; // Needed for newer OpenGL versions
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "glewInit failed" << std::endl;
		return -1;
	}

	glutDisplayFunc(display); // Set display callback
	glutIdleFunc(display); // Forces constant redrawing by using display as idle callback
	glutReshapeFunc(resize); // Disables window resizing

	// Initialize everything we need to start the main display loop
	bool success = init();

	if (!success)
	{
		std::cerr << "Program initialization failed" << std::endl;
		return -1;
	}

	// Main GLUT loop. Polls events and draws by calling display()
	glutMainLoop();

	// Clean up
	if (planetShader != nullptr)
		delete planetShader;
	
	return 0;
}
