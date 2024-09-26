#include "GameEntity.h"

GameEntity::GameEntity(Mesh mesh)
{
    //give the shared pointer the mesh
    Mesh* meshpointer = &mesh;
    entityMesh.reset(meshpointer);
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
    return entityTransform;
}

void GameEntity::Draw()
{
    entityMesh.get()->Draw();
}
