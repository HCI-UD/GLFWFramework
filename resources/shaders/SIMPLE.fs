#version 430

/* Per Fragment Input Attributes */
in vec3 color;

/* Per Fragment Output Values */
layout (location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(color, 1.0);
}