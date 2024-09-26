#include "Transform.h"

//destructor
Transform::~Transform()
{
}

//Sets position of transform
void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position.x = position.x;
	this->position.y = position.y;
	this->position.z = position.z;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	this->rotation.x = rotation.x;
	this->rotation.y = rotation.y;
	this->rotation.z = rotation.z;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	RecalculateWorldMatrix();
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTranspose;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	this->rotation.x += rotation.x;
	this->rotation.y += rotation.y;
	this->rotation.z += rotation.z;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
}

void Transform::RecalculateWorldMatrix()
{
	//make the world matrix out of scaling, rotation, and translation
	DirectX::XMMATRIX worldM = (DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * 
		DirectX::XMMatrixTranslation(position.x, position.y, position.z));

	//store it and its inverse transpose
	XMStoreFloat4x4(&world, worldM);
	XMStoreFloat4x4(&worldInverseTranspose,
		XMMatrixInverse(0, XMMatrixTranspose(worldM)));
}
