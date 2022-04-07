
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include "Shader.h"
#include "Camera.h"

using namespace std;

struct ModelTransform 
{

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	void setScale(float s)
	{
		scale.x = s;
		scale.y = s;
		scale.z = s;
	}
};

Camera camera(glm::vec3(0.f, 0.f, -5.f));

struct Color { float r, g, b, a; };

Color background = { 0.5f,0.5f,0.5f,0.f };

void onResize(GLFWwindow * win, int width, int height)
{
	glViewport(0, 0, width, height);
}

ModelTransform polygonTrans = { glm::vec3(0.0f,0.0f,0.0f),
									glm::vec3(0.0f,0.0f,0.0f),
									glm::vec3(1.0f,1.0f,1.0f)
};

float cam_dist = 5.0f;


void processInput(GLFWwindow* win, float dt)
{

	float speed = 1.0f * dt;
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);

	if (glfwGetKey(win, GLFW_KEY_K) == GLFW_PRESS)
		polygonTrans.position.x += speed;
	if (glfwGetKey(win, GLFW_KEY_H) == GLFW_PRESS)
		polygonTrans.position.x -= speed;

	if (glfwGetKey(win, GLFW_KEY_U) == GLFW_PRESS)
		polygonTrans.position.y += speed;
	if (glfwGetKey(win, GLFW_KEY_J) == GLFW_PRESS)
		polygonTrans.position.y -= speed;


	uint32_t dir = 0;

	if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
		dir |= CAM_UP;
	if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
		dir |= CAM_DOWN;
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		dir |= CAM_FORWARD;
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		dir |= CAM_BACKWARD;
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
		dir |= CAM_LEFT;
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
		dir |= CAM_RIGHT;



	double newx = 0.f, newy = 0.f;
	glfwGetCursorPos(win,&newx,&newy);
	static double x = newx, y = newy;
	double xoffset = newx - x;
	double yoffset = newy - y;
	x = newx;
	y = newy;

	camera.Move(dir, dt);
	camera.Rotate(xoffset, -yoffset);

}

void OnScroll(GLFWwindow* win, double x, double y)
{
	camera.ChangeFOV(y);
}

bool wireFrameMode = false;

void UpdatePolygonMode()
{
	if (wireFrameMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void OnKeyAction(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
			wireFrameMode = !wireFrameMode;
			UpdatePolygonMode();
			break;
		}
	}
}


typedef unsigned char byte;

int height = 1280, weight = 720;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(height, weight, "openglwin", NULL, NULL);

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
	glfwSetScrollCallback(win, OnScroll);
	glfwSetKeyCallback(win, OnKeyAction);

	glViewport(0, 0, height, weight);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	UpdatePolygonMode();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	int img_width, img_height, channels;
	byte* data = stbi_load("textures/cat.png", &img_width, &img_height, &channels, 0);



	const int verts = 8;

	float cube[verts * (6+2)] = {
		-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		1,2,3,
		0,4,1,
		1,4,5,
		0,3,7,
		0,7,4,
		1,6,2,
		1,5,6,
		2,7,3,
		2,6,7,
		4,7,5,
		5,7,6
	};

	unsigned int cat_texture;
	glGenTextures(1, &cat_texture);

	glBindTexture(GL_TEXTURE_2D, cat_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



	if (channels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	//glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);








	unsigned int VBO_polygon, VAO_polygon, EBO_polygon;
	glGenBuffers(1, &VBO_polygon);
	glGenBuffers(1, &EBO_polygon);
	glGenVertexArrays(1, &VAO_polygon);

	glBindVertexArray(VAO_polygon);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_polygon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts * 8, cube, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_polygon);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * verts * 36, indices, GL_STATIC_DRAW);

	
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");

	

	ModelTransform cube2 = { glm::vec3(5.0f,0.0f,0.0f),
									glm::vec3(0.0f,0.0f,0.0f),
									glm::vec3(1.0f,1.0f,1.0f)
	};
	

	double oldTime = glfwGetTime();
	double newTime = glfwGetTime();
	double deltaTime = glfwGetTime();
	while (!glfwWindowShouldClose(win))
	{
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;

		processInput(win,deltaTime);
		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		//polygonTrans.rotation.y = glfwGetTime() * 30;
		
		

		polygon_shader->use();



		


		glm::mat4 pv = camera.GetProjectionMatrix() * camera.GetViewMatrix();
		
		
		
		glm::mat4 model = glm::mat4(1.0f);
		polygonTrans.rotation.y = glfwGetTime()*30;
		model = glm::translate(model, polygonTrans.position);
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.x), glm::vec3(1.0f,0.0f,0.0f));
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, polygonTrans.scale);






		glm::mat4 pvm = pv * model;
		polygon_shader->setMatrix4F("pvm", pvm);
		polygon_shader->setBool("wireframe", wireFrameMode);
		
		glBindTexture(GL_TEXTURE_2D, cat_texture);
		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



		model = glm::mat4(1.0f);

		cube2.rotation.x = glfwGetTime() * 50;
		cube2.position.x = sin(glfwGetTime()) + 5.f;
		model = glm::translate(model, cube2.position);
		model = glm::rotate(model, glm::radians(cube2.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cube2.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(cube2.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, cube2.scale);


		pvm = pv * model;
		polygon_shader->setMatrix4F("pvm", pvm);
		polygon_shader->setBool("wireframe", wireFrameMode);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(win);
		glfwPollEvents();

	}

	delete polygon_shader;
	glfwTerminate();
	return 0;

}

