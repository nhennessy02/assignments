#include "ShaderIncludes.hlsli"

//Data from the constant buffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPos;
	float3 ambient;
	Light Light1;
	Light Light2;
	Light Light3;
	Light PointLight1;
	Light PointLight2;
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
	
	//uses input.color as the storage for the color + ambient light
	input.color.x = colorTint.x * ambient.x;
	input.color.y = colorTint.y * ambient.y;
	input.color.z = colorTint.z * ambient.z;
	input.color.w = colorTint.w;

	float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	return input.color 
		+ calcLightPhong(Light1, input.worldPosition, input.normal, input.color, specExponent, cameraPos) 
		+ calcLightPhong(Light2, input.worldPosition, input.normal, input.color, specExponent, cameraPos)
		+ calcLightPhong(Light3, input.worldPosition, input.normal, input.color, specExponent, cameraPos)
		+ calcLightPhong(PointLight1, input.worldPosition, input.normal, input.color, specExponent, cameraPos)
		+ calcLightPhong(PointLight2, input.worldPosition, input.normal, input.color, specExponent, cameraPos);
}


