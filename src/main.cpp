#include "utils.h"
#include "Camera.h"
#include "Cloth.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <fstream>

bool pause;
bool wireMode;
bool cullingMode;

bool LeftDown, RightDown;
int MouseX, MouseY;

glm::vec3 wind;

Camera* cam;

// Window Properties
int width;
int height;
const char* windowName;

// Objects to render
static Cloth* cloth;

// Shader Program 
static GLuint shaderProgram;

enum ShaderType { vertex, fragment };

GLuint LoadSingleShader(const char* shaderFilePath, ShaderType type) {
	// Create a shader id
	GLuint shaderID = 0;
	if (type == vertex) 
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	else if (type == fragment) 
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Try to read shader codes from the shader file.
	std::string shaderCode;
	std::ifstream shaderStream(shaderFilePath, std::ios::in);
	if (shaderStream.is_open()) 
	{
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else 
	{
		std::cerr << "Impossible to open " << shaderFilePath << ". "
			<< "Check to make sure the file exists and you passed in the "
			<< "right filepath!"
			<< std::endl;
		return 0;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader.
	std::cerr << "Compiling shader: " << shaderFilePath << std::endl;
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);
	
	// Check Shader.
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage.data());
		std::string msg(shaderErrorMessage.begin(), shaderErrorMessage.end());
		std::cerr << msg << std::endl;
		return 0;
	}
	else 
	{
		if (type == vertex)
			printf("Successfully compiled vertex shader!\n");
		else if (type == fragment) 
			printf("Successfully compiled fragment shader!\n");
	}

	return shaderID;
}

GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath) {
	// Create the vertex shader and fragment shader.
	GLuint vertexShaderID = LoadSingleShader(vertexFilePath, vertex);
	GLuint fragmentShaderID = LoadSingleShader(fragmentFilePath, fragment);

	// Check both shaders.
	if (vertexShaderID == 0 || fragmentShaderID == 0) return 0;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program.
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program.
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, ProgramErrorMessage.data());
		std::string msg(ProgramErrorMessage.begin(), ProgramErrorMessage.end());
		std::cerr << msg << std::endl;
		glDeleteProgram(programID);
		return 0;
	}
	printf("Successfully linked program!\n");
	
	return programID;
}

// Act as Constructors and desctructors 
bool initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	
	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool initializeObjects() {
	cloth = new Cloth(50, 50, glm::vec3(0, 0, 0));																							// Segementation Fault here
	cloth->blowByWind(wind);
	return true;
}

// for the Window
GLFWwindow* createWindow(int width, int height) {
    // Setup window
    if (!glfwInit())
        exit(0);

    // Decide GL+GLSL versions
    const char * glsl_version = setGLSLVersion();

    // Create window with graphics context
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(width, height, "Cloth Simulation Project", NULL, NULL);
    if (window == NULL)
        exit(0);

	// Make the context of the window.
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    // Initialize OpenGL loader
    int status = openGLInit();
    if(!status){
        std::cout << "Initialized OpenGL succesfully " << std::endl;
    }
    std::cout<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

	// set up the camera
	cam = new Camera();
    cam->setAspectRatio(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

	return window;
}

// update and display functions
void updateFrame(GLFWwindow* window) {
    // Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	cloth->display(cam->GetViewProjectMatrix(), shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);

	// Perform any updates as necessary. 
	cam->update();

	if (!pause) {
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
            cloth->update();
	}
}

void keyPressDetect(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {	// Check for a key press.
		switch (key) {

			// cloth control
			case GLFW_KEY_W:
				cloth->translate(glm::vec3(0, 0, -0.05));
				break;

			case GLFW_KEY_S:
				cloth->translate(glm::vec3(0, 0, 0.05));
				break;

			case GLFW_KEY_A:
				cloth->translate(glm::vec3(-0.05, 0, 0));
				break;

			case GLFW_KEY_D:
				cloth->translate(glm::vec3(0.05, 0, 0));
				break;

			case GLFW_KEY_LEFT_SHIFT:
				cloth->translate(glm::vec3(0, 0.05, 0));
				break;

			case GLFW_KEY_LEFT_CONTROL:
				cloth->translate(glm::vec3(0, -0.05, 0));
				break;

			
			// wind control
			case GLFW_KEY_I:
				wind += glm::vec3(0, 0, -1);
				cloth->blowByWind(wind);
				std::cerr << "Wind Velocity: " << "(" <<
					wind.x << ", " <<
					wind.y << ", " <<
					wind.z << ")" << std::endl;
				break;

			case GLFW_KEY_K:
				wind += glm::vec3(0, 0, 1);
				cloth->blowByWind(wind);
				std::cerr << "Wind Velocity: " << "(" <<
					wind.x << ", " <<
					wind.y << ", " <<
					wind.z << ")" << std::endl;
				break;

			case GLFW_KEY_J:
				wind += glm::vec3(-1, 0, 0);
				cloth->blowByWind(wind);
				std::cerr << "Wind Velocity: " << "(" <<
					wind.x << ", " <<
					wind.y << ", " <<
					wind.z << ")" << std::endl;
				break;

			case GLFW_KEY_L:
				wind += glm::vec3(1, 0, 0);
				cloth->blowByWind(wind);
				std::cerr << "Wind Velocity: " << "(" <<
					wind.x << ", " <<
					wind.y << ", " <<
					wind.z << ")" << std::endl;
				break;

			default:
				break;
		}
	}
}

void mouseDetect(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void cursorDetect(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	if (LeftDown) {
		const float rate = 1.0f;
		cam->setRotation(cam->getRotation() + dx * rate);
		cam->setAngle(glm::clamp(cam->getAngle() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(cam->getDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		cam->setDistance(dist);
	}
}

int main(void) {
	// Create the GLFW window.
	GLFWwindow* window = createWindow(800, 600);
	if (!window) exit(EXIT_FAILURE);

	// Set the detectors
	glfwSetKeyCallback(window, keyPressDetect);
	glfwSetMouseButtonCallback(window, mouseDetect);
	glfwSetCursorPosCallback(window, cursorDetect);
    // Enable depth buffering.
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Set clear color to black.
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Initialize the shader program; exit if initialization fails.
	if (!initializeProgram()) exit(EXIT_FAILURE);
	// Initialize objects/pointers for rendering; exit if initialization fails.
	if (!initializeObjects()) exit(EXIT_FAILURE);
	
	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(window))
	{
		// Main render display callback. Rendering of objects is done here.
		updateFrame(window);
	}

	// Destroy the window.
	glfwDestroyWindow(window);
	// Terminate GLFW.
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

