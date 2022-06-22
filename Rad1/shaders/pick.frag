#version 330 core

out vec4 outColor;
uniform int rID;
uniform int gID;
uniform int bID;
uniform float aID;

void main()
{
    outColor = vec4(rID/255.f, gID/255.f,bID/255.f, aID);
}