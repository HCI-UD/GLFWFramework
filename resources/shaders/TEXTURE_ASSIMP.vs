#version 440 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 modelMatrix;
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
out vec2 TexCoords;

void main()
{
// Calculate normal matrix
	mat3 normalMatrix = mat3(viewMatrix * modelMatrix);

	// Compute vertex position and normal in eye space
	eyePosition = viewMatrix * modelMatrix * vec4(position, 1.0);
	eyeNormal = normalize(normalMatrix * normal);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
    TexCoords = texCoords;
}