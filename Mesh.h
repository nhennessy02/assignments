#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include "Graphics.h"

class Mesh
{
public:

	//Method declaration
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	char GetName();
	void Draw();

	// Constructor(s)
	Mesh() = default;
	Mesh(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indicesCount, const char* meshName);
	~Mesh();

private:

	// The buffers for this mesh
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// The amount of indices and vertices in the buffers
	unsigned int indicesCount;
	unsigned int vertexCount;

	//Extra data
	const char *meshName;
	
};

