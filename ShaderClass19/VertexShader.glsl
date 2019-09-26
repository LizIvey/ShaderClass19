#version 430

in layout(location = 0) vec2 position;
in layout(location = 1) vec3 Color;

uniform vec2 Pos;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position + Pos, 0.0, 1.0);
	theColor = Color;
};