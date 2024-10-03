#pragma once
#include "Input.h"
#include "Transform.h"

class Camera
{
public:
	//constructors
	Camera() = default;
	Camera(DirectX::XMFLOAT3 initPosition, float fov, float movespeed);
	~Camera();

	//getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	//updaters
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	//extras
	float fov;
	float movespeed;
};

