#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"

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
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	bool demoVis = false;
	bool disableAll = false;
	char windowName[128] = "Awesome Window";
	bool clicked = false;
	int clickCounter;
	float colorTint[4] = { 1.0f, 0.5f, 0.5f, 1.0f };
	float offset[3] = { 0.25f, 0.0f, 0.0f };
	

	//Meshes
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> singleQuad;
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> torus; 
	std::shared_ptr<Mesh> sphere2;
	std::shared_ptr<Mesh> cylinder2;
	std::shared_ptr<Mesh> helix2;
	std::shared_ptr<Mesh> quad2;
	std::shared_ptr<Mesh> singleQuad2;
	std::shared_ptr<Mesh> cube2;
	std::shared_ptr<Mesh> torus2; 
	std::shared_ptr<Mesh> sphere3;
	std::shared_ptr<Mesh> cylinder3;
	std::shared_ptr<Mesh> helix3;
	std::shared_ptr<Mesh> quad3;
	std::shared_ptr<Mesh> singleQuad3;
	std::shared_ptr<Mesh> cube3;
	std::shared_ptr<Mesh> torus3;

	//shaders
	std::shared_ptr<SimplePixelShader> UVShader;
	std::shared_ptr<SimplePixelShader> normalShader;
	std::shared_ptr<SimplePixelShader> fancyShader;

	//materials
	std::shared_ptr<Material> mat0White;
	std::shared_ptr<Material> mat1Red;
	std::shared_ptr<Material> mat2Blue;
	std::shared_ptr<Material> mat3Yellow;
	std::shared_ptr<Material> UVMat;
	std::shared_ptr<Material> normalMat;
	std::shared_ptr<Material> fancyMat;

	//entities
	std::vector<std::shared_ptr<GameEntity>> entities;

	//transform stuff
	DirectX::XMFLOAT3 mover = { 0.0f, 0.0f, 0.0f };
	float moveFactor = 0.25f;
	DirectX::XMFLOAT3 rotator = { 0.0f, 0.0f, 0.0f };
	float rotateFactor = 1.0f;
	DirectX::XMFLOAT3 scaler = { 1.0f, 1.0f, 1.0f };
	float scaleFactor = 0.6f;

	//cameras
	std::vector<std::shared_ptr<Camera>> cameras;
	int activeCamera = 0;

	//lights
	DirectX::XMFLOAT3 ambientColor = { 0.1, 0.1, 0.25 };
	Light Light1 = {};
	Light Light2 = {};
	Light Light3 = {};
	Light PointLight1 = {};
	Light PointLight2 = {};
	
};

