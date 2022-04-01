
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "Shader.h"

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

	const int verts = 4;

	float polygon[verts * 6] = {
		0.0f, 0.75f, 0.0f,		1.0f, 1.0f,0.0f,
		0.5f, 0.0f, 0.0f,		1.0f, 1.0f,0.0f,
		-0.5f, 0.0f, 0.0f,		0.0f, 0.0f,1.0f,
		0.0f, -0.75f, 0.0f,		0.0f, 0.0f, 1.0f
		
	};

	unsigned int indices[] = {
		0,1,2,
		1,2,3
	};


	unsigned int VBO_polygon, VAO_polygon, EBO_polygon;
	glGenBuffers(1, &VBO_polygon);
	glGenBuffers(1, &EBO_polygon);
	glGenVertexArrays(1, &VAO_polygon);

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts * 6, polygon, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_polygon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * verts * 6, indices, GL_STATIC_DRAW);

	
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);



	Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");

	while (!glfwWindowShouldClose(win))
	{
		processInput(win);
		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT);

		

		polygon_shader->use();
		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(win);
		glfwPollEvents();

	}

	delete polygon_shader;
	glfwTerminate();
	return 0;

}

