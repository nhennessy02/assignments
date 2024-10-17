
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;       // for lighting
	float2 uv : TEXCOORD;     // for texture mapping
};

//Data from the constant buffer
cbuffer ExternalData : register(b0)
{
	float screenWidth;
	float screenHeight;
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
	//object color changes based on screen position, based on screen size

	float scaledx = (input.screenPosition.x / screenWidth);
	float scaledy = (input.screenPosition.y / screenHeight);

	return float4(scaledx, 0, scaledy, 1);
}