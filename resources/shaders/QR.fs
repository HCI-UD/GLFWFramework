#version 430

/* Per Fragment Input Attributes */
in vec2 texCoord;

/* Per Fragment Uniform Attributes */
layout (binding = 0) uniform sampler2D textureSampler0;
layout (binding = 1) uniform sampler2D textureSampler1;

uniform bool doCalibration;

/* Per Fragment Output Values */
layout (location = 0) out vec3 out_color;

void main()
{
	vec4 color0 = texture(textureSampler0, texCoord); 		//background
	vec4 color1 = texture(textureSampler1, texCoord);		//marker
	
	if(doCalibration)
		out_color = color1.rgb;
	else
		out_color = color0.rgb;
}