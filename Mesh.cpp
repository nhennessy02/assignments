#include "Mesh.h"
#include <vector>
#include <fstream>

Mesh::Mesh(Vertex* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indicesCount)
{
	// transfer the numbers to the mesh's values
	this->vertexCount = (unsigned int)vertexCount;
	this->indicesCount = (unsigned int)indicesCount;

	// Create the vertex buffer
	{
		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		// Storing the data
		D3D11_SUBRESOURCE_DATA initialVertexData = {};
		initialVertexData.pSysMem = vertices;

		// Actually make the buffer
		Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Create the index buffer
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indicesCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		// Storing the data
		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indices;

		// Actually make the buffer
		Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
	}
}

Mesh::Mesh(const char* modelFile)
{

		// Author: Chris Cascioli
		// Purpose: Basic .OBJ 3D model loading, supporting positions, uvs and normals
		// 
		// - You are allowed to directly copy/paste this into your code base
		//   for assignments, given that you clearly cite that this is not
		//   code of your own design.
		//
		// - NOTE: You'll need to #include <fstream>


		// File input object
		std::ifstream obj(modelFile);

		// Check for successful open
		if (!obj.is_open())
			return;

		// Variables used while reading the file
		std::vector<DirectX::XMFLOAT3> positions;	// Positions from the file
		std::vector<DirectX::XMFLOAT3> normals;		// Normals from the file
		std::vector<DirectX::XMFLOAT2> uvs;		// UVs from the file
		std::vector<Vertex> verts;		// Verts we're assembling
		std::vector<UINT> indices;		// Indices of these verts
		int vertCounter = 0;			// Count of vertices
		int indexCounter = 0;			// Count of indices
		char chars[100];			// String for line reading

		// Still have data left?
		while (obj.good())
		{
			// Get the line (100 characters should be more than enough)
			obj.getline(chars, 100);

			// Check the type of line
			if (chars[0] == 'v' && chars[1] == 'n')
			{
				// Read the 3 numbers directly into an XMFLOAT3
				DirectX::XMFLOAT3 norm;
				sscanf_s(
					chars,
					"vn %f %f %f",
					&norm.x, &norm.y, &norm.z);

				// Add to the list of normals
				normals.push_back(norm);
			}
			else if (chars[0] == 'v' && chars[1] == 't')
			{
				// Read the 2 numbers directly into an XMFLOAT2
				DirectX::XMFLOAT2 uv;
				sscanf_s(
					chars,
					"vt %f %f",
					&uv.x, &uv.y);

				// Add to the list of uv's
				uvs.push_back(uv);
			}
			else if (chars[0] == 'v')
			{
				// Read the 3 numbers directly into an XMFLOAT3
				DirectX::XMFLOAT3 pos;
				sscanf_s(
					chars,
					"v %f %f %f",
					&pos.x, &pos.y, &pos.z);

				// Add to the positions
				positions.push_back(pos);
			}
			else if (chars[0] == 'f')
			{
				// Read the face indices into an array
				// NOTE: This assumes the given obj file contains
				//  vertex positions, uv coordinates AND normals.
				unsigned int i[12];
				int numbersRead = sscanf_s(
					chars,
					"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&i[0], &i[1], &i[2],
					&i[3], &i[4], &i[5],
					&i[6], &i[7], &i[8],
					&i[9], &i[10], &i[11]);

				// If we only got the first number, chances are the OBJ
				// file has no UV coordinates.  This isn't great, but we
				// still want to load the model without crashing, so we
				// need to re-read a different pattern (in which we assume
				// there are no UVs denoted for any of the vertices)
				if (numbersRead == 1)
				{
					// Re-read with a different pattern
					numbersRead = sscanf_s(
						chars,
						"f %d//%d %d//%d %d//%d %d//%d",
						&i[0], &i[2],
						&i[3], &i[5],
						&i[6], &i[8],
						&i[9], &i[11]);

					// The following indices are where the UVs should 
					// have been, so give them a valid value
					i[1] = 1;
					i[4] = 1;
					i[7] = 1;
					i[10] = 1;

					// If we have no UVs, create a single UV coordinate
					// that will be used for all vertices
					if (uvs.size() == 0)
						uvs.push_back(DirectX::XMFLOAT2(0, 0));
				}

				// - Create the verts by looking up
				//    corresponding data from vectors
				// - OBJ File indices are 1-based, so
				//    they need to be adusted
				Vertex v1;
				v1.Position = positions[i[0] - 1];
				v1.UV = uvs[i[1] - 1];
				v1.Normal = normals[i[2] - 1];

				Vertex v2;
				v2.Position = positions[i[3] - 1];
				v2.UV = uvs[i[4] - 1];
				v2.Normal = normals[i[5] - 1];

				Vertex v3;
				v3.Position = positions[i[6] - 1];
				v3.UV = uvs[i[7] - 1];
				v3.Normal = normals[i[8] - 1];

				// The model is most likely in a right-handed space,
				// especially if it came from Maya.  We want to convert
				// to a left-handed space for DirectX.  This means we 
				// need to:
				//  - Invert the Z position
				//  - Invert the normal's Z
				//  - Flip the winding order
				// We also need to flip the UV coordinate since DirectX
				// defines (0,0) as the top left of the texture, and many
				// 3D modeling packages use the bottom left as (0,0)

				// Flip the UV's since they're probably "upside down"
				v1.UV.y = 1.0f - v1.UV.y;
				v2.UV.y = 1.0f - v2.UV.y;
				v3.UV.y = 1.0f - v3.UV.y;

				// Flip Z (LH vs. RH)
				v1.Position.z *= -1.0f;
				v2.Position.z *= -1.0f;
				v3.Position.z *= -1.0f;

				// Flip normal's Z
				v1.Normal.z *= -1.0f;
				v2.Normal.z *= -1.0f;
				v3.Normal.z *= -1.0f;

				// Add the verts to the vector (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v3);
				verts.push_back(v2);
				vertCounter += 3;

				// Add three more indices
				indices.push_back(indexCounter); indexCounter += 1;
				indices.push_back(indexCounter); indexCounter += 1;
				indices.push_back(indexCounter); indexCounter += 1;

				// Was there a 4th face?
				// - 12 numbers read means 4 faces WITH uv's
				// - 8 numbers read means 4 faces WITHOUT uv's
				if (numbersRead == 12 || numbersRead == 8)
				{
					// Make the last vertex
					Vertex v4;
					v4.Position = positions[i[9] - 1];
					v4.UV = uvs[i[10] - 1];
					v4.Normal = normals[i[11] - 1];

					// Flip the UV, Z pos and normal's Z
					v4.UV.y = 1.0f - v4.UV.y;
					v4.Position.z *= -1.0f;
					v4.Normal.z *= -1.0f;

					// Add a whole triangle (flipping the winding order)
					verts.push_back(v1);
					verts.push_back(v4);
					verts.push_back(v3);
					vertCounter += 3;

					// Add three more indices
					indices.push_back(indexCounter); indexCounter += 1;
					indices.push_back(indexCounter); indexCounter += 1;
					indices.push_back(indexCounter); indexCounter += 1;
				}
			}
		}
		// Close the file and create the actual buffers
		obj.close();

		//============= END OF COPIED CODE ===============

		vertexCount = (unsigned int)verts.size();
		indicesCount = (unsigned int)indices.size();

		// Create the vertex buffer
		{
			D3D11_BUFFER_DESC vbd = {};
			vbd.Usage = D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth = sizeof(Vertex) * vertCounter;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			vbd.StructureByteStride = 0;

			// Storing the data
			D3D11_SUBRESOURCE_DATA initialVertexData = {};
			initialVertexData.pSysMem = &verts[0];

			// Actually make the buffer
			Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());
		}

		// Create the index buffer
		{
			D3D11_BUFFER_DESC ibd = {};
			ibd.Usage = D3D11_USAGE_IMMUTABLE;
			ibd.ByteWidth = sizeof(unsigned int) * indexCounter;
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			ibd.MiscFlags = 0;
			ibd.StructureByteStride = 0;

			// Storing the data
			D3D11_SUBRESOURCE_DATA initialIndexData = {};
			initialIndexData.pSysMem = &indices[0];

			// Actually make the buffer
			Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
		}
}

Mesh::~Mesh()
{
	//Empty for now
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indicesCount;
}

int Mesh::GetVertexCount()
{
	return vertexCount;
}


void Mesh::Draw()
{
	// Draw the mesh using its data
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		Graphics::Context->DrawIndexed(
			indicesCount,     // The number of indices to use (we could draw a subset if we wanted)
			0,     // Offset to the first index we want to use
			0);    // Offset to add to each index when looking up vertices
	}
}
