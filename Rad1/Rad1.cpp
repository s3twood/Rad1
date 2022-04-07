
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


void processInput(GLFWwindow* win)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
		background = { 0.f,1.f,0.f,1.f };
	if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
		background = { 1.f,0.f,0.f,1.f };
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		polygonTrans.position.y += 0.01f;
	if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		polygonTrans.position.y -= 0.01f;
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
		polygonTrans.position.x -= 0.01f;
	if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
		polygonTrans.position.x += 0.01f;

	if (glfwGetKey(win, GLFW_KEY_H) == GLFW_PRESS)
		cam_dist += 0.05f;
	if (glfwGetKey(win, GLFW_KEY_K) == GLFW_PRESS)
		cam_dist -= 0.05f;


	if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
		polygonTrans.rotation.x -= 0.1f;
	if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
		polygonTrans.rotation.x += 0.1f;

	if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)
		polygonTrans.rotation.y -= 0.1f;
	if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
		polygonTrans.rotation.y += 0.1f;

}


typedef unsigned char byte;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(900, 900, "openglwin", NULL, NULL);

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
	glViewport(0, 0, 900, 900);

	glEnable(GL_DEPTH_TEST);

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

	

	while (!glfwWindowShouldClose(win))
	{
		processInput(win);
		glClearColor(background.r, background.g, background.b, background.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		//polygonTrans.rotation.y = glfwGetTime() * 30;
		
		

		polygon_shader->use();


		glm::vec3 pos_vec = glm::vec3(0.0f, 0.0f, cam_dist);
		glm::vec3 target_vec = glm::vec3(0.f, 0.f, 0.0f);
		glm::vec3 up_vec = glm::vec3(0.f, 1.f, 0.0f);

		glm::mat4 camera = glm::lookAt(pos_vec,target_vec,up_vec);
		//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.f);
		glm::mat4 projection = glm::perspective(45.f, 1.0f, 0.01f, 100.0f);




		glm::mat4 model = glm::mat4(1.0f);
		
		model = glm::translate(model, polygonTrans.position);
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.x), glm::vec3(1.0f,0.0f,0.0f));
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(polygonTrans.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, polygonTrans.scale);

		glm::mat4 pvm = projection * camera * model;

		polygon_shader->setMatrix4F("pvm",pvm);
		
		glBindTexture(GL_TEXTURE_2D, cat_texture);
		glBindVertexArray(VAO_polygon);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(win);
		glfwPollEvents();

	}

	delete polygon_shader;
	glfwTerminate();
	return 0;

}

