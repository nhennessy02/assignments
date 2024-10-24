#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
    // Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition	: POSITION; // XYZ position
    float3 normal			: NORMAL; // for lighting
    float2 uv				: TEXCOORD; // for texture mapping
};

struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color
    float SpotInnerAngle; // Inner cone angle (in radians) � Inside this, full light!
    float SpotOuterAngle; // Outer cone angle (radians) � Outside this, no light!
    float2 Padding; // Purposefully padding to hit the 16-byte boundary
};

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	
    float4 screenPosition	: SV_POSITION;
    float4 color			: COLOR;
    float3 normal			: NORMAL; // for lighting
    float2 uv				: TEXCOORD; // for texture mapping
    float3 worldPosition    : POSITION;
};
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

//returns the DIFFUSE component
float4 calcLightDiffuse(Light currentLight, float3 worldPos, float3 inputNormal, float3 inputColor)
{
    float3 direction;
    if (currentLight.Type = LIGHT_TYPE_POINT) //point light
        direction = normalize(currentLight.Position - worldPos);
    else //directional light
        direction = -direction;

	//from reading
	float3 diffuseTerm =
		saturate(dot(inputNormal, direction)) * // Diffuse intensity, clamped to 0-1
		currentLight.Color * currentLight.Intensity * // Light�s overall color
		inputColor;

	float4 finalColor;
	finalColor.x = inputColor.x + diffuseTerm.x;
	finalColor.y = inputColor.y + diffuseTerm.y;
	finalColor.z = inputColor.z + diffuseTerm.z;
	finalColor.w = 1.0f;

	return finalColor;
}

//returns the SPECULAR component
float4 calcLightSpecular(Light currentLight, float3 worldPos, float3 inputNormal, float3 inputColor, float shininess, float3 cameraPos)
{
    float3 lightRefl = reflect(currentLight.Direction, inputNormal);
    float3 directionToCam = normalize(cameraPos - worldPos);
    
    float RdotV = saturate(dot(lightRefl, -directionToCam));
    
    float3 specularTerm =
    pow(RdotV, shininess) *
    currentLight.Color * currentLight.Intensity *
    inputColor;
    
    float4 finalColor;
    finalColor.x = specularTerm.x;
    finalColor.y = specularTerm.y;
    finalColor.z = specularTerm.z;
    finalColor.w = 1.0f;

    return finalColor;
}

//returns the PHONG component of the equation
float4 calcLightPhong(Light currentLight, float3 worldPos, float3 inputNormal, float3 inputColor, float shininess, float3 cameraPos)
{
    float3 direction;
        direction = currentLight.Direction; //directional light by default
    
    if (currentLight.Type == LIGHT_TYPE_POINT) //point light
    {
        direction = normalize(currentLight.Position - worldPos);
    } 

	//from reading
        float3 diffuseTerm =
		saturate(dot(inputNormal, direction)) * // Diffuse intensity, clamped to 0-1
		currentLight.Color * currentLight.Intensity * // Light�s overall color
		inputColor;
    
        float3 lightRefl = reflect(currentLight.Direction, inputNormal);
        float3 directionToCam = normalize(cameraPos - worldPos);
    
        float RdotV = saturate(dot(lightRefl, -directionToCam));
    
        float3 specularTerm =
        pow(RdotV, shininess) *
        currentLight.Color * currentLight.Intensity *
        inputColor;
    
        float4 phongTerm;
        phongTerm.x = diffuseTerm.x + specularTerm.x;
        phongTerm.y = diffuseTerm.y + specularTerm.y;
        phongTerm.z = diffuseTerm.z + specularTerm.z;
        phongTerm.w = 1.0f;
    
    if (currentLight.Type == LIGHT_TYPE_POINT) //point light attenuation
    {
        phongTerm *= Attenuate(currentLight, worldPos);
    }
    
        return phongTerm;
    }


#endif