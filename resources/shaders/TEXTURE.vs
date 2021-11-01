#version 430

/* Per Vertex Input Attributes */
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_textCoords;

/* Per Vertex Uniform Attributes */
uniform mat4 modelMatrix;

/* Named Uniform Blocks */
layout (std140, binding = 0) uniform Matrices
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
};

/* Per Vertex Output Attributes */
out vec2 texCoord;

void main()
{
	texCoord = in_textCoords;
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
}