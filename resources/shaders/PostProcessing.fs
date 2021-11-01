#version 440

/*************************************/
/*** Per Fragment Input Attributes ***/
/*************************************/

in vec4 eyePosition;
in vec3 eyeNormal;
in vec3 color;
in vec2 texCoord;

/********************************/
/*** Per Fragment Subroutines ***/
/********************************/

subroutine vec4 PostProcessingModeType();

/****************************************/
/*** Per Fragment Subroutine Uniforms ***/
/****************************************/

layout (location = 0) subroutine uniform PostProcessingModeType postProcessingMode;

/*************************************/
/*** Per Fragment Texture Samplers ***/
/*************************************/

layout (binding = 0) uniform sampler2D textureSampler;
layout (binding = 1) uniform sampler2D luminanceSampler;

/***************************************/
/*** Per Fragment Uniform Attributes ***/
/***************************************/
uniform float LuminanceThreshold;
uniform float EdgeThreshold;
uniform int   GaussianKernelSize;
uniform float GaussianWeights15x15[8] = float[](0.185497470489039, 0.166947723440135, 0.121416526138280, 0.070826306913997, 0.032689064729537, 0.011674665974835, 0.003113244259956, 0.000583733298742);
uniform float GaussianWeights13x13[7] = float[](0.196482550151140, 0.174651155689903, 0.122255808982932, 0.066684986717963, 0.027785411132485, 0.008549357271534, 0.001832005129614);
uniform float GaussianWeights11x11[6] = float[](0.209856915739269, 0.183624801271860, 0.122416534181240, 0.061208267090620, 0.022257551669316, 0.005564387917329);
uniform float GaussianWeights9x9[5]   = float[](0.227027027027027, 0.194594594594595, 0.121621621621622, 0.054054054054054, 0.016216216216216);
uniform float GaussianWeights7x7[4]   = float[](0.251497005988024, 0.209580838323353, 0.119760479041916, 0.044910179640719);       
uniform float GaussianWeights5x5[3]   = float[](0.294117647058824, 0.235294117647059, 0.117647058823529);      
uniform float GaussianWeights3x3[2]   = float[](0.400000000000000, 0.300000000000000);               

/*****************************************/
/*** Per Fragment Named Uniform Blocks ***/
/*****************************************/

layout (std140, binding = 1) uniform LightInfo
{
	vec4 La;		// Ambient  light intensity
	vec4 Ld;		// Diffuse  light intensity
	vec4 Ls;		// Specular light intensity
	vec4 Le;		// Emission light intensity
	vec4 Position;  // Light position in eye coordinates
} Light;

layout (std140, binding = 2) uniform MaterialInfo
{
	vec4 Ka;		 // Ambient  material reflectivity
	vec4 Kd;		 // Diffuse  material reflectivity
	vec4 Ks;		 // Specular material reflectivity
	vec4 Ke;		 // Emission material reflectivity
	float Shininess; // Specular shininess factor
} Material;

/**********************************/
/*** Per Fragment Output Values ***/
/**********************************/

layout (location = 0) out vec4 out_color;



/**********************/
/*** Implementation ***/
/**********************/

/* 
 * Calculate the relative luminance of an RGB color vector
 *
 * Reference: https://en.wikipedia.org/wiki/Relative_luminance
 */
float Luminence(vec3 color)
{
	return dot(vec3(0.2126, 0.7152, 0.0722), color);
}

layout (index = 0) subroutine(PostProcessingModeType)
vec4 TextureFetch()
{
	return texelFetch(textureSampler, ivec2(gl_FragCoord.xy), 0);
}

layout (index = 1) subroutine(PostProcessingModeType)
vec4 EdgeDetectionSobel()
{
	// Implementation is based on the GLSL cookbook, v2.
	// https://github.com/daw42/glslcookbook/blob/master/chapter05/shader/edge.fs

	ivec2 pix = ivec2(gl_FragCoord.xy);

	float s11 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,0)).rgb);
    float s00 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,1)).rgb);
    float s10 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)).rgb);
    float s20 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,-1)).rgb);
    float s01 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,1)).rgb);
    float s21 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)).rgb);
    float s02 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,1)).rgb);
    float s12 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,0)).rgb);
    float s22 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,-1)).rgb);

    float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
    float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

    float g = sqrt(sx * sx + sy * sy);
	
    if( g > EdgeThreshold )
	{
		// Edge pixel!
		// Strong edges will have a darker color (1.0 - g)
		// return vec4(1.0 - g, 1.0 - g, 1.0 - g, 1.0);
		return vec4(1.0, 1.0, 1.0, 1.0);
	}
    else
	{
		// No edge pixel!
		// White transparent color
        return vec4(1.0, 1.0, 1.0, 0.0);
	}
}

layout (index = 2) subroutine(PostProcessingModeType)
vec4 EdgeDetectionFreiChen()
{
	// Implementation is based on the GLSL cookbook, v2.
	// https://github.com/daw42/glslcookbook/blob/master/chapter05/shader/edge.fs

	ivec2 pix = ivec2(gl_FragCoord.xy);

	float s11 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,0)).rgb);
    float s00 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,1)).rgb);
    float s10 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)).rgb);
    float s20 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(-1,-1)).rgb);
    float s01 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,1)).rgb);
    float s21 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)).rgb);
    float s02 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,1)).rgb);
    float s12 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,0)).rgb);
    float s22 = Luminence(texelFetchOffset(textureSampler, pix, 0, ivec2(1,-1)).rgb);
	
	float g1 = 1.0/(2.0*sqrt(2.0)) * (s00 + sqrt(2) * s10 + s20 - (s02 + sqrt(2) * s12 + s22));
	float g2 = 1.0/(2.0*sqrt(2.0)) * (s00 + sqrt(2) * s01 + s02 - (s20 + sqrt(2) * s21 + s22));
	float g3 = 1.0/(2.0*sqrt(2.0)) * (s01 + sqrt(2) * s20 + s12 - (s10 + sqrt(2) * s02 + s21));
	float g4 = 1.0/(2.0*sqrt(2.0)) * (s21 + sqrt(2) * s00 + s12 - (s10 + sqrt(2) * s22 + s01));
	float g5 = 1.0/2.0 * (s10 + s12 - (s01 + s21));
	float g6 = 1.0/2.0 * (s20 + s02 - (s00 + s22));
	float g7 = 1.0/6.0 * (s10 + s01 + s21 + s12 + 4 * s11 - (2 * (s00 + s20 + s02 + s22)));
	float g8 = 1.0/3.0 * (s00 + s10 + s20 + s01 + s11 + s21 + s02 + s12 + s22);
	
	float M = g1 * g1 + g2 * g2 + g3 * g3 + g4 * g4;
	float S = M + g5 * g5 + g6 * g6 + g7 * g7 + g8 * g8;
	
	float g = sqrt(M / S);
	
	if( g > EdgeThreshold )
	{
		// Edge pixel!
		// Strong edges will have a darker color (1.0 - g)
		// return vec4(1.0 - g, 1.0 - g, 1.0 - g, 1.0);
		return vec4(1.0, 1.0, 1.0, 1.0);
	}
    else
	{
		// No edge pixel!
		// White transparent color
        return vec4(1.0, 1.0, 1.0, 0.0);
	}
}

layout (index = 3) subroutine(PostProcessingModeType)
vec4 GaussianBlurVertical()
{
	// Vertical gaussian blurring
	// Implementation is based on the GLSL cookbook, v2.
	// Reference: https://github.com/daw42/glslcookbook/blob/master/chapter05/shader/blur.fs
	// Predefined gaussian weights are used, according to: http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

	ivec2 pix = ivec2(gl_FragCoord.xy);
	
	vec4 sum;
	switch(GaussianKernelSize)
	{
		case(3):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights3x3[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights3x3[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights3x3[1];
			break;
		}
		
		case(5):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights5x5[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights5x5[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights5x5[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights5x5[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights5x5[2];
			break;
		}
		
		case(7):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights7x7[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights7x7[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights7x7[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights7x7[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights7x7[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,3))  * GaussianWeights7x7[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-3)) * GaussianWeights7x7[3];
			break;
		}
		
		case(9):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights9x9[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights9x9[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights9x9[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights9x9[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights9x9[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,3))  * GaussianWeights9x9[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-3)) * GaussianWeights9x9[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,4))  * GaussianWeights9x9[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-4)) * GaussianWeights9x9[4];
			break;
		}
		
		case(11):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights11x11[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights11x11[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights11x11[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights11x11[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights11x11[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,3))  * GaussianWeights11x11[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-3)) * GaussianWeights11x11[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,4))  * GaussianWeights11x11[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-4)) * GaussianWeights11x11[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,5))  * GaussianWeights11x11[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-5)) * GaussianWeights11x11[5];
			break;
		}
		
		case(13):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights13x13[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights13x13[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights13x13[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights13x13[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights13x13[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,3))  * GaussianWeights13x13[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-3)) * GaussianWeights13x13[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,4))  * GaussianWeights13x13[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-4)) * GaussianWeights13x13[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,5))  * GaussianWeights13x13[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-5)) * GaussianWeights13x13[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,6))  * GaussianWeights13x13[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-6)) * GaussianWeights13x13[6];
			break;
		}
		
		case(15):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights15x15[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,1))  * GaussianWeights15x15[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-1)) * GaussianWeights15x15[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,2))  * GaussianWeights15x15[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-2)) * GaussianWeights15x15[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,3))  * GaussianWeights15x15[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-3)) * GaussianWeights15x15[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,4))  * GaussianWeights15x15[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-4)) * GaussianWeights15x15[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,5))  * GaussianWeights15x15[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-5)) * GaussianWeights15x15[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,6))  * GaussianWeights15x15[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-6)) * GaussianWeights15x15[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,7))  * GaussianWeights15x15[7];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(0,-7)) * GaussianWeights15x15[7];
			break;
		}
		
		default:
		{
			break;
		}
	}
	
    return sum;
}

layout (index = 4) subroutine(PostProcessingModeType)
vec4 GaussianBlurHorizontal()
{
	// Horizontal gaussian blurring
	// Implementation is based on the GLSL cookbook, v2.
	// Reference: https://github.com/daw42/glslcookbook/blob/master/chapter05/shader/blur.fs
	// Predefined gaussian weights are used, according to: http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	
	ivec2 pix = ivec2(gl_FragCoord.xy);
	
	vec4 sum;
	switch(GaussianKernelSize)
	{
		case(3):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights3x3[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights3x3[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights3x3[1];
			break;
		}
		
		case(5):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights5x5[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights5x5[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights5x5[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights5x5[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights5x5[2];
			break;
		}
		
		case(7):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights7x7[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights7x7[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights7x7[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights7x7[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights7x7[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(3,0))  * GaussianWeights7x7[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-3,0)) * GaussianWeights7x7[3];
			break;
		}
		
		case(9):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights9x9[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights9x9[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights9x9[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights9x9[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights9x9[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(3,0))  * GaussianWeights9x9[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-3,0)) * GaussianWeights9x9[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(4,0))  * GaussianWeights9x9[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-4,0)) * GaussianWeights9x9[4];
			break;
		}
		
		case(11):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights11x11[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights11x11[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights11x11[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights11x11[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights11x11[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(3,0))  * GaussianWeights11x11[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-3,0)) * GaussianWeights11x11[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(4,0))  * GaussianWeights11x11[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-4,0)) * GaussianWeights11x11[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(5,0))  * GaussianWeights11x11[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-5,0)) * GaussianWeights11x11[5];
			break;
		}
		
		case(13):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights13x13[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights13x13[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights13x13[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights13x13[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights13x13[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(3,0))  * GaussianWeights13x13[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-3,0)) * GaussianWeights13x13[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(4,0))  * GaussianWeights13x13[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-4,0)) * GaussianWeights13x13[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(5,0))  * GaussianWeights13x13[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-5,0)) * GaussianWeights13x13[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(6,0))  * GaussianWeights13x13[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-6,0)) * GaussianWeights13x13[6];
			break;
		}
		
		case(15):
		{
			sum = sum + texelFetch(textureSampler, pix, 0) * GaussianWeights15x15[0];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(1,0))  * GaussianWeights15x15[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-1,0)) * GaussianWeights15x15[1];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(2,0))  * GaussianWeights15x15[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-2,0)) * GaussianWeights15x15[2];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(3,0))  * GaussianWeights15x15[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-3,0)) * GaussianWeights15x15[3];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(4,0))  * GaussianWeights15x15[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-4,0)) * GaussianWeights15x15[4];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(5,0))  * GaussianWeights15x15[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-5,0)) * GaussianWeights15x15[5];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(6,0))  * GaussianWeights15x15[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-6,0)) * GaussianWeights15x15[6];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(7,0))  * GaussianWeights15x15[7];
			sum = sum + texelFetchOffset(textureSampler, pix, 0, ivec2(-7,0)) * GaussianWeights15x15[7];
			break;
		}
		
		default:
		{
			break;
		}
	}
	
    return sum;
}

layout (index = 5) subroutine(PostProcessingModeType)
vec4 ThresholdedLuminance()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);
	vec3 color = texelFetch(textureSampler, pix, 0).rgb;
	float lum = Luminence(color);
	
	if( lum > LuminanceThreshold )
	{
		// Edge pixel!
		// Strong edges will have a darker color (1.0 - g)
		// return vec4(1.0 - g, 1.0 - g, 1.0 - g, 1.0);
		return vec4(color, 1.0);
	}
    else
	{
		// No edge pixel!
		// White transparent color
        return vec4(1.0, 1.0, 1.0, 0.0);
	}
}

layout (index = 6) subroutine(PostProcessingModeType)
vec4 AdditiveBlending()
{
	return (texelFetch(textureSampler, ivec2(gl_FragCoord.xy), 0) + texelFetch(luminanceSampler, ivec2(gl_FragCoord.xy), 0));
}

void main()
{
	out_color = postProcessingMode();
}