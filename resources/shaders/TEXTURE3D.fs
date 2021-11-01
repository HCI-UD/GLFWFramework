#version 430

/* Per Fragment Input Attributes */
in vec2 texCoord;

/* Per Fragment Uniform Attributes */
uniform float currentSliceTexCoord;

/* Per Fragment Uniform Texture Samplers */
layout (binding = 0) uniform sampler3D textureSampler;

/* Per Fragment Output Values */
layout (location = 0) out vec3 out_color;

void main()
{
	out_color = texture(textureSampler, vec3(texCoord, currentSliceTexCoord)).rgb;
}