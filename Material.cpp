#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> verShader, std::shared_ptr<SimplePixelShader> pixShader)
{
    this->colorTint = colorTint;
    this->verShader = verShader;
    this->pixShader = pixShader;
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

