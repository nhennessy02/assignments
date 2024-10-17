#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh,std::shared_ptr<Material> mat)
{
    entityMesh = mesh;
    entityMaterial = mat;
}

GameEntity::~GameEntity()
{
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return entityMesh;
}

Transform* GameEntity::GetTransform()
{
    return &entityTransform;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
    return entityMaterial;
}

void GameEntity::SetMaterial(Material mat)
{
    entityMaterial = std::make_shared<Material>(mat);
}

void GameEntity::Draw()
{
    entityMaterial->GetVertexShader()->SetShader();
    entityMaterial->GetPixelShader()->SetShader();
    entityMesh.get()->Draw();
}
