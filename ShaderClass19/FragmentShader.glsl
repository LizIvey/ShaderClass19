#version 430

out vec4 daColor;
uniform vec3 Color;

void main()
{
	daColor = vec4(Color, 1.0);
};