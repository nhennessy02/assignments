#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> verShader, std::shared_ptr<SimplePixelShader> pixShader)
{
    this->colorTint = colorTint;
    this->verShader = verShader;
    this->pixShader = pixShader;

    //clamping roughness to 1-0
    if (roughness > 1.0)
        this->roughness = 1.0;
    else if (roughness < 0.0)
        this->roughness = 0.0;
    else
        this->roughness = roughness;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return verShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixShader;
}

float Material::GetRoughness()
{
    return 0.0f;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

void Material::SetVertexShader(SimpleVertexShader verShader)
{
    this->verShader = std::make_shared<SimpleVertexShader>(verShader);
}

void Material::SetPixelShader(SimplePixelShader pixShader)
{
    this->pixShader = std::make_shared<SimplePixelShader>(pixShader);
}

