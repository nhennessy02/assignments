#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh)
{
    entityMesh = mesh;
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

void GameEntity::Draw()
{
    entityMesh.get()->Draw();
}
