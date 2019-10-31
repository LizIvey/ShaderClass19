#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 Color;

uniform mat4 FullTransformMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0); //take 3D position and stretch it into 4D
	gl_Position = FullTransformMatrix * v;
	theColor = Color;
};