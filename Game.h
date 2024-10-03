#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void ImGuiUpdate(float deltaTime);
	void BuildUI(float deltaTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateBuffers();
	void CreateGeometry();


	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//Constant Buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool demoVis = false;
	bool disableAll = false;
	char windowName[128] = "Awesome Window";
	bool clicked = false;
	int clickCounter;
	float colorTint[4] = { 1.0f, 0.5f, 0.5f, 1.0f };
	float offset[3] = { 0.25f, 0.0f, 0.0f };
	

	//Meshes
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> diamond;
	std::shared_ptr<Mesh> hexagon;

	//entities
	std::vector<std::shared_ptr<GameEntity>> entities;

	//transform stuff
	DirectX::XMFLOAT3 mover = { 0.0f, 0.0f, 0.0f };
	float moveFactor = 0.25f;
	DirectX::XMFLOAT3 rotator = { 0.0f, 0.0f, 0.0f };
	float rotateFactor = 1.0f;
	DirectX::XMFLOAT3 scaler = { 1.0f, 1.0f, 1.0f };
	float scaleFactor = 0.6f;

	//camera
	std::shared_ptr<Camera> mainCamera;
	
};

