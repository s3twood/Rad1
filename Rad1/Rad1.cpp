
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;


void onResize(GLFWwindow * win, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* win)
{

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

	cout << "123123";

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Error load GLAD" << endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(win, onResize);


	while (!glfwWindowShouldClose(win))
	{
		processInput(win);

		glfwSwapBuffers(win);
		glfwPollEvents();

	}


	glfwTerminate();
	return 0;

}

