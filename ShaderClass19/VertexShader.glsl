#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 Color;

uniform vec3 Pos;
uniform float yflip;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position + Pos, 1.0);
	theColor = Color;
};