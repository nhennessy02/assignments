#include "ShaderIncludes.hlsli"

//Data from the constant buffer
cbuffer ExternalData : register(b0)
{
	float4x4 world;
	float4x4 worldInvTranspose;
	float4x4 view;
	float4x4 projection;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// Multiply the three matrices together first
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	//do the same for world position
	output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;

	// Pass the data through
	output.uv = input.uv;

	output.normal = mul((float3x3)worldInvTranspose, input.normal); //cast the normal to a 3x3

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}