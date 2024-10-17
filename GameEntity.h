#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

class GameEntity
{
public:
	GameEntity() = default;
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat);
	~GameEntity();

	//getters
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();
	std::shared_ptr<Material> GetMaterial();

	//setters
	void SetMaterial(Material mat);

	//other
	void Draw();
	

private:
	Transform entityTransform;
	std::shared_ptr<Mesh> entityMesh;
	std::shared_ptr<Material> entityMaterial;
};

