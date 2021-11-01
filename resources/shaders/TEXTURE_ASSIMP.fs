#version 440 core

/*************************************/
/*** Per Fragment Input Attributes ***/
/*************************************/

in vec4 eyePosition;
in vec3 eyeNormal;

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;






/********************************/
/*** Per Fragment Subroutines ***/
/********************************/

subroutine vec3 LightingModelType(vec4 eyePosition, vec3 eyeNormal);

/****************************************/
/*** Per Fragment Subroutine Uniforms ***/
/****************************************/

layout (location = 0) subroutine uniform LightingModelType lightingModel;

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





/****************************************/
/*********** Implementation *************/
/****************************************/

layout (index = 0) subroutine(LightingModelType)
vec3 PhongLightingModel(vec4 eyePosition, vec3 eyeNormal)
{
	// Vector towards point light source
	vec3 s = normalize(vec3(Light.Position - eyePosition));
	
	// Vector towards the viewer
	vec3 v = normalize(-eyePosition.xyz);
	
	// Vector of perfect reflection
	vec3 r = reflect(-s, eyeNormal);
	
	// Calcualte dot product between s and surface normal (eyeNormal) for convenience
	float sDotN = max(dot(s, eyeNormal), 0.0);	
	
	// Calculate Ambient, Diffuse and Specular light components
	vec3 Ia = Light.La.rgb * Material.Ka.rgb;
	vec3 Id = Light.Ld.rgb * Material.Kd.rgb * sDotN;
	vec3 Is = vec3(0.0);
	if (sDotN > 0.0)
	{
		Is = Light.Ls.rgb * Material.Ks.rgb * pow(max(dot(r, v), 0.0), Material.Shininess);
	}
	
	return Ia + Id + Is;
}

layout (index = 1) subroutine(LightingModelType)
vec3 DiffuseOnlyLightingModel(vec4 eyePosition, vec3 eyeNormal)
{
	// Vector towards point light source
	vec3 s = normalize(vec3(Light.Position - eyePosition));
	
	// Calcualte dot product between s and surface normal (eyeNormal) for convenience
	float sDotN = max(dot(s, eyeNormal), 0.0);	
	
	return Light.Ld.rgb * Material.Kd.rgb * sDotN;
}

void main()
{
	color = vec4(texture(texture_diffuse1, TexCoords)) ;//+ vec4(lightingModel(eyePosition, eyeNormal), 1.0);
}