#include "Camera.h"
#include "Input.h"

Camera::Camera(DirectX::XMFLOAT3 initPosition, float fov, float movespeed)
{
	cameraTransform.SetPosition(initPosition);
	this->fov = fov;
	this->movespeed = movespeed;

	//INITIALIZE MATRICES HERE
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.05f, 900.0f));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 posStore = transform.GetPosition();
	DirectX::XMFLOAT3 forStore = transform.GetForward();
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&posStore), DirectX::XMLoadFloat3(&forStore), DirectX::XMVECTOR{ 0,1,0 }));
}

void Camera::Update(float dt)
{
	if(Input::KeyDown('W'))
		transform.MoveRelative(DirectX::XMFLOAT3{ movespeed * dt,0,0 });
	if(Input::KeyDown('S'))
		transform.MoveRelative(DirectX::XMFLOAT3{ -movespeed * dt,0,0 });

	UpdateViewMatrix();

}
