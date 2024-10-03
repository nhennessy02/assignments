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

DirectX::XMFLOAT3 Transform::GetRight()
{
	//rotate the world's right vector by transform's rotation
	DirectX::XMVECTOR rightVector = DirectX::XMVector3Rotate(DirectX::XMVECTOR{ 1,0,0 }, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	//convert to float3 and return
	DirectX::XMFLOAT3 rightFloat3{ DirectX::XMVectorGetX(rightVector), DirectX::XMVectorGetY(rightVector), DirectX::XMVectorGetZ(rightVector) };
	return rightFloat3;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	//rotate the world's up vector by transform's rotation
	DirectX::XMVECTOR upVector = DirectX::XMVector3Rotate(DirectX::XMVECTOR{ 0,1,0 }, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	//convert to float3 and return
	DirectX::XMFLOAT3 upFloat3{ DirectX::XMVectorGetX(upVector), DirectX::XMVectorGetY(upVector), DirectX::XMVectorGetZ(upVector) };
	return upFloat3;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	//rotate the world's forward vector by transform's rotation
	DirectX::XMVECTOR forwardVector = DirectX::XMVector3Rotate(DirectX::XMVECTOR{ 0,0,1 }, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	//convert to float3 and return
	DirectX::XMFLOAT3 forwardFloat3{ DirectX::XMVectorGetX(forwardVector), DirectX::XMVectorGetY(forwardVector), DirectX::XMVectorGetZ(forwardVector) };
	return forwardFloat3;
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

void Transform::MoveRelative(float x, float y, float z)
{
	//convert to vectors
	DirectX::XMVECTOR movementVector{ x,y,z };
	DirectX::XMVECTOR currentRotation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	//rotate the vector
	DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(movementVector, currentRotation);
	//add it's values to position
	position.x += DirectX::XMVectorGetX(direction);
	position.y += DirectX::XMVectorGetY(direction);
	position.z += DirectX::XMVectorGetZ(direction);
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	//convert to vectors
	DirectX::XMVECTOR movementVector = { offset.x,offset.y,offset.z };
	DirectX::XMVECTOR currentRotation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	//rotate the vector
		DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(movementVector, currentRotation);
	//add it's values to position
	position.x += DirectX::XMVectorGetX(direction);
	position.y += DirectX::XMVectorGetY(direction);
	position.z += DirectX::XMVectorGetZ(direction);
}

void Transform::RecalculateWorldMatrix()
{

	//make the world matrix out of scaling, rotation, and translation
	DirectX::XMMATRIX worldM = ((DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)) * 
		DirectX::XMMatrixTranslation(position.x, position.y, position.z));

	//store it and its inverse transpose
	XMStoreFloat4x4(&world, worldM);
	XMStoreFloat4x4(&worldInverseTranspose,
		XMMatrixInverse(0, XMMatrixTranspose(worldM)));
}
