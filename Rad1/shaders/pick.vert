#version 330 core
layout (location = 0) in vec3 inPos;
uniform mat4 pv;
uniform mat4 model;

void main()
{
	gl_Position = pv * model * vec4(inPos, 1.0);
}