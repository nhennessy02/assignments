#include "Camera.h"
#include "Input.h"

Camera::Camera(DirectX::XMFLOAT3 initPosition, float fov, float movespeed)
{
	transform.SetPosition(initPosition);
	this->fov = fov;
	this->movespeed = movespeed;
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixIdentity());
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

Transform* Camera::GetTransform()
{
	return &transform;
}

float Camera::GetFOV()
{
	return fov;
}

float Camera::GetMovespeed()
{
	return movespeed;
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
	//keyboard movement
	if (Input::KeyDown('W'))
		transform.MoveRelative(DirectX::XMFLOAT3{ 0,0,movespeed * dt });
	if (Input::KeyDown('S'))
		transform.MoveRelative(DirectX::XMFLOAT3{ 0,0,-movespeed * dt });
	if(Input::KeyDown('D'))
		transform.MoveRelative(DirectX::XMFLOAT3{ movespeed * dt,0,0 });
	if(Input::KeyDown('A'))
		transform.MoveRelative(DirectX::XMFLOAT3{ -movespeed * dt,0,0 });
	if (Input::KeyDown(VK_SPACE))
		transform.MoveRelative(DirectX::XMFLOAT3{ 0,movespeed * dt,0 });
	if (Input::KeyDown('X'))
		transform.MoveRelative(DirectX::XMFLOAT3{ 0,-movespeed * dt,0 });

	//mouse movement
	if(Input::MouseLeftDown())
	{
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();
		
		transform.Rotate(DirectX::XMFLOAT3(cursorMovementY*0.025f, cursorMovementX*0.01f, 0));

		if(transform.GetPitchYawRoll().x > DirectX::XM_PIDIV2)
		{
			transform.SetRotation(DirectX::XM_PIDIV2, transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
		}
		if (transform.GetPitchYawRoll().x < -DirectX::XM_PIDIV2)
		{
			transform.SetRotation(-DirectX::XM_PIDIV2, transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
		}
	}


	UpdateViewMatrix();

}
