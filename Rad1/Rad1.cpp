#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assimp/Importer.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"

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

struct Color {
	float r, g, b, a;
};

bool pickClick = false;
bool colorPickingShow = false;

Color background = { 0.f, 0.f, 0.5f, 1.f };


struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

Camera camera(glm::vec3(0.183165, -0.0376139, 0.031249), glm::vec3(0.f, 1.0f, 0.f), 243.051, -20.7);

void OnResize(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
}
vector<Light*> lights;
vector<Model*> objects;

float distP = 2;
void processInput(GLFWwindow* win, double dt)
{
	if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(win, true);
	if (glfwGetKey(win, GLFW_KEY_1) == GLFW_PRESS)
		background = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_2) == GLFW_PRESS)
		background = { 0.0f, 1.0f, 0.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_3) == GLFW_PRESS)
		background = { 0.0f, 0.0f, 1.0f, 1.0f };
	if (glfwGetKey(win, GLFW_KEY_4) == GLFW_PRESS)
		background = { 0.55f, 0.8f, 0.85f, 1.0f };
	
	

	
	

	


	

	uint32_t dir = 0;

	if (glfwGetKey(win, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		dir |= CAM_UP;
	if (glfwGetKey(win, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
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
	glfwGetCursorPos(win, &newx, &newy);
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
	//camera.ChangeFOV(y);
	//std::cout << "Scrolled x: " << x << ", y: " << y << ". FOV = " << camera.Fov << std::endl;
	distP += y/10;
	if (distP < 1) distP = 1;
}

bool wireframeMode = false;
Model* pickObject = 0;

void UpdatePolygoneMode()
{
	if (wireframeMode)
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
			wireframeMode = !wireframeMode;
			UpdatePolygoneMode();
			break;
		case GLFW_KEY_F:
			for (auto light : lights)
				if (light->name == "FlashLight")
					light->turnChange();
			break;
		case GLFW_KEY_P:
			cout <<
				"glm::vec3(" << camera.Position.x <<
				"f, " << camera.Position.y <<
				"f," << camera.Position.z << "f)," <<
				endl <<
				"glm::vec3(" << camera.Front.x <<
				"f, " << camera.Front.y <<
				"f, " << camera.Front.z << "f)," << endl;
			break;
		case GLFW_KEY_M:
			if (pickObject == 0) break;
			cout <<
				"glm::vec3(" << pickObject->translate.x <<
				"f, " << pickObject->translate.y <<
				"f," << pickObject->translate.z << "f), // pos" <<
				endl  <<

				"glm::vec3(" << pickObject->rotate.x <<
				"f, " << pickObject->rotate.y <<
				"f," << pickObject->rotate.z << "f), // rotate" <<
				endl <<

				"glm::vec3(" << pickObject->scale.x <<
				"f, " << pickObject->scale.y <<
				"f," << pickObject->scale.z << "f), // scale" <<
				endl;
			break;
		case GLFW_KEY_E:
			pickClick = true;
			break;

		case GLFW_KEY_Z:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x + 3, pickObject->rotate.y, pickObject->rotate.z);
			break;
		case GLFW_KEY_X:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x - 3, pickObject->rotate.y, pickObject->rotate.z);
			break;

		case GLFW_KEY_C:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x, pickObject->rotate.y+3, pickObject->rotate.z);
			break;
		case GLFW_KEY_V:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x, pickObject->rotate.y-3, pickObject->rotate.z);
			break;

		case GLFW_KEY_B:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x, pickObject->rotate.y, pickObject->rotate.z+3);
			break;
		case GLFW_KEY_N:
			if (pickObject == 0) break;
			pickObject->setRotate(pickObject->rotate.x, pickObject->rotate.y, pickObject->rotate.z-3);
			break;

		case GLFW_KEY_KP_ADD:
			if (pickObject == 0) break;
			pickObject->setScale(pickObject->scale.x + 0.01);
			break;

		case GLFW_KEY_KP_SUBTRACT:
			if (pickObject == 0) break;
			pickObject->setScale(pickObject->scale.x - 0.01);
			break;





		case GLFW_KEY_9:
			colorPickingShow = !colorPickingShow;
			break;
		}
	}
}

typedef unsigned char byte;

Light* flashLight, * redLamp, * blueLamp, * sunLight, *LampLight;


Model* getPickedObject()
{
	vector< unsigned char > pixels(1 * 1 * 4);
	glReadPixels
	(
		1280 / 2, 720 / 2,
		1, 1,
		GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]
	);

	for (auto var : objects)
	{
		if (var->id == 0) continue;
		if (pixels[0] == var->r() && pixels[1] == var->g() && pixels[2] == var->b())
			return var;
	}

	return 0;
}


int main()
{
#pragma region WINDOW INITIALIZATION
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* win = glfwCreateWindow(1280, 720, "OpenGL Window", NULL, NULL);
	if (win == NULL)
	{
		std::cout << "Error. Couldn't create window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error. Couldn't load GLAD!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(win, OnResize);
	glfwSetScrollCallback(win, OnScroll);
	glfwSetKeyCallback(win, OnKeyAction);

	glViewport(0, 0, 1280, 720);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	UpdatePolygoneMode();
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);

#pragma endregion

	

	





#pragma endregion

	Shader* polygon_shader = new Shader("shaders\\basic.vert", "shaders\\basic.frag");
	Shader* light_shader = new Shader("shaders\\light.vert", "shaders\\light.frag");


	Shader* backpack_shader = new Shader("shaders\\backpack.vert", "shaders\\backpack.frag");
	Shader* dust2_shader = new Shader("shaders\\dust2.vert", "shaders\\dust2.frag");
	Shader* sniper_shader = new Shader("shaders\\dust2.vert", "shaders\\dust2.frag");
	Shader* balon_shader = new Shader("shaders\\dust2.vert", "shaders\\dust2.frag");
	Shader* tree_shader = new Shader("shaders\\dust2.vert", "shaders\\dust2.frag");

	Model dust2("models/dust2/source/de_dust2.obj", dust2_shader,
		glm::vec3(0.0f, 0.0f, 0.0f), // pos
		glm::vec3(270.0f, 0.0f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f),
		true, false, false);
	objects.push_back(&dust2);

	Model sniper("models/Sniper/KSR-29 sniper rifle new_obj.obj", sniper_shader,
		glm::vec3(7.03181f, 0.0187588f, 2.25845f), // pos
		glm::vec3(270.f, 0.f, 0.f), // rotate
		glm::vec3(0.0999998f, 0.0999998f, 0.0999998f), // scale
		true, false, true);

	objects.push_back(&sniper);

	Model backpack("models/backpack/backpack.obj", backpack_shader,
		glm::vec3(7.50288f, 0.370695f, 3.43412f), // pos
		glm::vec3(271.f, -91.f, -87.f), // rotate
		glm::vec3(0.2f, 0.2f, 0.2f), // scale
		false, false, true);
	
	objects.push_back(&backpack);
	
	Model backpack2("models/backpack/backpack.obj", backpack_shader,
		glm::vec3(5.26887f, 0.13478f, 3.18514f), // pos
		glm::vec3(270.f, 0.f, 0.f), // rotate
		glm::vec3(0.15f, 0.15f, 0.15f), // scale
		
		false, false, true);

	objects.push_back(&backpack2);

	Model balon("models/Balon/Helium and Oxygen tank set.obj", balon_shader,
		glm::vec3(6.74596f, 0.0723032f, 2.90167f), // pos
		glm::vec3(360.f, 0.f, 0.f), // rotate
		glm::vec3(0.0999998f, 0.0999998f, 0.0999998f), // scale
		false, false, true);

	objects.push_back(&balon);

	//Model tree("models/low_poly_tree/Lowpoly_tree_sample.obj", tree_shader,
	//	glm::vec3(6.74596f, 0.0723032f, 2.90167f), // pos
	//	glm::vec3(360.f, 0.f, 0.f), // rotate
	//	glm::vec3(1.f, 1.f, 1.f), // scale
	//	false, false, true);
	//
	//objects.push_back(&tree);

	double oldTime = glfwGetTime(), newTime, deltaTime;

#pragma region LIGHT INITIALIZATION


	int active_lights = 0;

	sunLight = new Light("Sun", true);
	sunLight->initLikeDirectionalLight(
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0f,0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f));
	lights.push_back(sunLight);


	flashLight = new Light("FlashLight", true);
	flashLight->initLikeSpotLight(
		glm::vec3(4.18867f, 5.04311f, 1.95246f),
		glm::vec3(0.000197824f, -0.999848f, 0.0174513f),
		glm::radians(10.f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.7f, 0.7f, 0.6f),
		glm::vec3(0.8f, 0.8f, 0.6f),
		1.0f, 0.01f, 0.09f);
	lights.push_back(flashLight);


	LampLight = new Light("LampLight", true);
	LampLight->initLikeSpotLight(
		glm::vec3(4.18867f, 5.04311f, 1.95246f),
		glm::vec3(0.000197824f, -0.999848f, 0.0174513f),
		glm::radians(30.f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.7f, 0.7f, 0.6f),
		glm::vec3(0.8f, 0.8f, 0.6f),
		1.0f, 0.00f, 0.09f);
	lights.push_back(LampLight);



#pragma endregion
		
		glm::vec3 savePos;
		
	while (!glfwWindowShouldClose(win))
	{
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		oldTime = newTime;

		glClearColor(background.r, background.g, background.b, background.a);
		processInput(win, deltaTime);


		flashLight->position = camera.Position;
		flashLight->direction = camera.Front;

		

		

		glm::mat4 p = camera.GetProjectionMatrix();
		glm::mat4 v = camera.GetViewMatrix();
		glm::mat4 pv = p * v;
		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


if (pickClick)
{
	if (pickObject == 0)
	{
		for (auto var : objects)
		{
			var->updatePickShader(camera.Position, pv);
			var->pickDraw();
		}

		pickObject = getPickedObject();
	}
	else
		pickObject = 0;
	
	pickClick = false;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
		
if (pickObject != 0)
{
	glm::vec3 dist = glm::vec3(camera.Front.x * distP, camera.Front.y * distP, camera.Front.z * distP);
	pickObject->setPosition(camera.Position + dist);
}
	
		
for (auto var : objects)
{
	if (colorPickingShow)
	{
		var->updatePickShader(camera.Position, pv);
		var->pickDraw();
	}
	else
	{
		if (pickObject != 0 && pickObject->id == var->id)
		{
			var->updatePickShader(camera.Position, pv, 0.2f, 255, 255, 255);
			var->pickDraw();
		}
		else
		{
			var->updateShader(camera.Position, pv, lights);
			var->Draw();
		}
	}
}
glfwSwapBuffers(win);
		glfwPollEvents();
		
	}

	delete polygon_shader;
	delete light_shader;
	delete backpack_shader;
	delete dust2_shader;
	delete sniper_shader;
	delete balon_shader;
	delete tree_shader;

	glfwTerminate();
	return 0;
}