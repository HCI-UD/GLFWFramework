#version 440 core

/*************************************/
/*** Per Fragment Input Attributes ***/
/*************************************/

in vec4 eyePosition;
in vec3 eyeNormal;

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{
	color = vec4(1,1,0,1) ;
}