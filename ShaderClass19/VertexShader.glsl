#version 430

in layout(location = 0) vec2 position;
in layout(location = 1) vec3 Color;

uniform vec2 Offset = vec2(-0.4, 0.2);
uniform float yflip;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position + Offset, 0.0, 1.0);
	gl_Position.y = gl_Position.y * yflip;
	theColor = Color;
};