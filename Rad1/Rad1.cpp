
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>


using namespace std;


struct Color { float r, g, b, a; };

Color background = { 1.f,0.f,0.f,1.f };

void onResize(GLFWwindow * win, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* win)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
		background = { 0.f,1.f,0.f,1.f };
	if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
		background = { 1.f,0.f,0.f,1.f };

}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(500, 500, "openglwin", NULL, NULL);

	if (win == NULL)
	{
		cout << "Error create window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Error load GLAD" << endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(win, onResize);

	const int verts = 6;

	float polygon[verts*(3+4)] = {
		-0.5f, 0.0f, 0.0f,		1.0f, 0.0f,0.0f,1.0f,
		0.0f, 0.75f, 0.0f,		0.0f, 1.0f,0.0f,1.0f,
		0.5f, 0.0f, 0.0f,		0.0f, 0.0f,1.0f,1.0f,
		0.5f, 0.0f, 0.0f,		1.0f, 0.0f,0.0f,1.0f,
		0.0f, -0.75f, 0.0f,		0.0f, 1.0f,0.0f,1.0f,
		-0.5f, 0.0f, 0.0f,		0.0f, 0.0f,1.0f,1.0f
	};

	unsigned int VBO_polygon;
	glGenBuffers(1, &VBO_polygon);

	unsigned VAO_polygon;
	glGenVertexArrays(1, &VAO_polygon);

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts * (3+4), polygon, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);




	/*char* shaderText = new char[1000], str[200];
	ifstream sfile;
	sfile.open("basic.vs");
	while (!sfile.eof())
	{
		str[0] = '\0';
		sfile >> str;
		if (str[0] != '\0')
			strcat_s(shaderText, 200, str);
	}

	sfile.close();*/

	const char* shaderTextVertex = "#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec4 inColor;\n"
		"out vec4 fragColor;\n"
		"void main()\n"
		"{gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f); \n"
		"fragColor = inColor; }";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &shaderTextVertex, NULL);
	glCompileShader(vertexShader);

	char resultInfo[1000];

	int res;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &res);
	if (res == 0)
	{
		glGetShaderInfoLog(vertexShader, 1000, NULL, resultInfo);
		cout << "Shader compilation error: " << resultInfo << endl;
		glfwTerminate();
		return -1;
	}
	//
	/*shaderText[0] = 0;
	sfile.open("basic.fs");
	while (!sfile.eof())
	{
		str[0] = '\0';
		sfile >> str;
		if (str[0] != '\0')
			strcat_s(shaderText, 200, str);
	}

	sfile.close();*/

	const char* shaderTextFragment = "#version 330 core\n"
		"in vec4 fragColor;\n"
		"out vec4 outColor;\n"
		"void main()\n"
		"{outColor = fragColor;}";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (char** const)(&shaderTextFragment), NULL);
	glCompileShader(fragmentShader);

	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &res);
	if (res == 0)
	{
		glGetShaderInfoLog(fragmentShader, 1000, NULL, resultInfo);
		cout << "Shader compilation error: " << resultInfo << endl;
		glfwTerminate();
		return -1;
	}


	unsigned int shaderProg;
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertexShader);
	glAttachShader(shaderProg, fragmentShader);
	glLinkProgram(shaderProg);


	glGetProgramiv(shaderProg, GL_LINK_STATUS, &res);
	if (res == 0)
	{
		glGetProgramInfoLog(shaderProg, 1000, NULL, resultInfo);
		cout << "Shader linker error: " << resultInfo << endl;
		glfwTerminate();
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	


	while (!glfwWindowShouldClose(win))
	{
		processInput(win);
		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
		

		glUseProgram(shaderProg);
		glBindVertexArray(VAO_polygon);
		glDrawArrays(GL_TRIANGLES, 0, verts);

		glfwSwapBuffers(win);
		glfwPollEvents();

	}


	glfwTerminate();
	return 0;

}

