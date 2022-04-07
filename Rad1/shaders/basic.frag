#version 330 core
in vec3 vertColor;
in vec2 texCoords;

out vec4 outColor;

uniform sampler2D outTexture;
uniform bool wireframe;

void main()
{
	if (wireframe)
		outColor = vec4(vertColor, 1.0f);
	else
		outColor = texture(outTexture, texCoords) * vec4(vertColor,1.0f);
	
}