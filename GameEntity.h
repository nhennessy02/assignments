#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"

class GameEntity
{
public:
	GameEntity() = default;
	GameEntity(Mesh mesh);
	~GameEntity();

	//getters
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();

	//other methods
	void Draw();

private:
	Transform* entityTransform;
	std::shared_ptr<Mesh> entityMesh;
};

