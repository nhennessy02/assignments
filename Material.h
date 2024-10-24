#pragma once

#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include "SimpleShader.h"

class Material
{
public:
	Material(DirectX::XMFLOAT4 colorTint,float roughness, std::shared_ptr<SimpleVertexShader> verShader, std::shared_ptr<SimplePixelShader> pixShader);
	~Material();

	//getters
	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();

	//setters
	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetVertexShader(SimpleVertexShader verShader);
	void SetPixelShader(SimplePixelShader pixShader);

private:
	DirectX::XMFLOAT4 colorTint;
	float roughness;
	std::shared_ptr<SimpleVertexShader> verShader;
	std::shared_ptr<SimplePixelShader> pixShader;
};

