#version 430

/* Per Fragment Input Attributes */
in vec2 texCoord;

/* Per Fragment Uniform Attributes */
layout (binding = 0) uniform sampler2D textureSamplerColorFrame;
layout (binding = 1) uniform sampler2D textureSamplerBackground;

/* SSBOs */
struct SSBOBlock 
{
  ivec4 position;
};

layout(std430, binding = 0) buffer LowestFacePointsBuffer 
{
    ivec4[] positions;
} LowestFacePoints;

/* Per Fragment Output Values */
layout (location = 0) out vec3 out_color;

void main()
{
	const int textureHeight = 1080;
	const int faceSize = 256;

	vec4 colorColorFrame = texture(textureSamplerColorFrame, texCoord);
	vec4 colorBackground = texture(textureSamplerBackground, texCoord);
	
	out_color = colorColorFrame.rgb;
	
	for (int i = 0; i < LowestFacePoints.positions.length(); i++) 
	{
		if ((gl_FragCoord.x > LowestFacePoints.positions[i].x - faceSize / 2) && (gl_FragCoord.x < LowestFacePoints.positions[i].x + faceSize / 2) && (gl_FragCoord.y > textureHeight - LowestFacePoints.positions[i].y) && (gl_FragCoord.y < textureHeight - LowestFacePoints.positions[i].y + faceSize))
		{
			out_color = colorBackground.rgb;
		}
	}
}