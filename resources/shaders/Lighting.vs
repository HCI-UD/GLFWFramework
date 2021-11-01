#version 440

/***********************************/
/*** Per Vertex Input Attributes ***/
/***********************************/

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

/*************************************/
/*** Per Vertex Uniform Attributes ***/
/*************************************/

uniform mat4 modelMatrix;

/***************************************/
/*** Per Vertex Named Uniform Blocks ***/
/***************************************/

layout (std140, binding = 0) uniform Matrices
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
};

/************************************/
/*** Per Vertex Output Attributes ***/
/************************************/

out vec4 eyePosition;
out vec3 eyeNormal;
out vec3 color;



/****************************************/
/*********** Implementation *************/
/****************************************/
void main()
{
	// Calculate normal matrix
	mat3 normalMatrix = mat3(viewMatrix * modelMatrix);

	// Compute vertex position and normal in eye space
	eyePosition = viewMatrix * modelMatrix * vec4(in_position, 1.0);
	eyeNormal = normalize(normalMatrix * in_normal);

	// Pass through color
	color = in_color;

	// Calculate position
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
}