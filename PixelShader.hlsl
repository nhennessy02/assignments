#include "ShaderIncludes.hlsli"

//Data from the constant buffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPos;
	float3 ambient;
	Light directionalLight1;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	input.color.x = colorTint.x * ambient.x;
	input.color.y = colorTint.y * ambient.y;
	input.color.z = colorTint.z * ambient.z;
	input.color.w = colorTint.w;

	float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	return calcLightDiffuse(directionalLight1, input.worldPosition, input.normal, input.color);
	//return calcLightSpecular(directionalLight1, input.worldPosition, input.normal, input.color, specExponent, cameraPos);
}


