#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <DirectXMath.h>
#include <memory>
#include <math.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBuffers();
	cameras.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3{ 0,0,-10.0 }, XM_PIDIV2, 5.0));
	cameras.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3{ 0,1.0,-1.0 }, XMConvertToRadians(45), 2.0));
	for (auto& c : cameras)
	{
		c->UpdateProjectionMatrix((float)Window::Width() / Window::Height());
	}
	CreateGeometry();
	

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();


	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}

	
}

void Game::CreateBuffers()
{
	//Creating a constant buffer!
	{
		unsigned int size = sizeof(constBufferData);
		size = (size + 15) / 16 * 16; //creates multiple of 16 based on size of struct

		D3D11_BUFFER_DESC cbDesc = {}; //Make buffer empty
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = size; 
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		Graphics::Device->CreateBuffer(&cbDesc, 0, constBuffer.GetAddressOf()); //Actually make the buffer

		//Binding the buffer(s)
		Graphics::Context->VSSetConstantBuffers(
			0, // Which slot (register) to bind the buffer to?
			1, // How many are we setting right now?
			constBuffer.GetAddressOf()); // Array of buffers (or address of just one)
	}
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 magenta = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 cyan = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	
	Vertex mesh1vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.3f, +0.0f), red },
		{ XMFLOAT3(+0.3f, -0.3f, +0.0f), blue },
		{ XMFLOAT3(-0.3f, -0.3f, +0.0f), green },
	};
	unsigned int mesh1indices[] = { 0, 1, 2 };
	triangle = std::make_shared<Mesh>(mesh1vertices,3,mesh1indices,3);

	entities.push_back(std::make_shared<GameEntity>(triangle));
	entities.push_back(std::make_shared<GameEntity>(triangle));

	Vertex mesh2vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.1f, +0.0f), magenta },
		{ XMFLOAT3(+0.05f, +0.0f, +0.0f), cyan },
		{ XMFLOAT3(+0.0f, -0.1f, +0.0f), yellow },
		{ XMFLOAT3(-0.05f, +0.0f, +0.0f), black },
	};
	unsigned int mesh2indices[] = { 0, 1, 3, 1, 2, 3 };
	diamond = std::make_shared<Mesh>(mesh2vertices, 4, mesh2indices, 6);


	entities.push_back(std::make_shared<GameEntity>(diamond));
	entities.push_back(std::make_shared<GameEntity>(diamond));
	entities[2]->GetTransform()->SetPosition(0.5f, 0.6f, 0.0f);
	entities[3]->GetTransform()->SetPosition(0.8f, 0.6f, 0.0f);

	Vertex mesh3vertices[] =
	{
		{ XMFLOAT3(-0.1f, +0.0f, +0.0f), red },
		{ XMFLOAT3(-0.05f, -0.2f, +0.0f), yellow },
		{ XMFLOAT3(+0.0f, +0.2f, +0.0f), red },
		{ XMFLOAT3(+0.05f, -0.2f, +0.0f), yellow },
		{ XMFLOAT3(+0.1f, +0.0f, +0.0f), red },
	};
	unsigned int mesh3indices[] = { 0, 2, 1, 2, 3, 1, 2, 4, 3 };
	hexagon = std::make_shared<Mesh>(mesh3vertices, 5, mesh3indices, 9);

	entities.push_back(std::make_shared<GameEntity>(hexagon));
	entities[4]->GetTransform()->SetPosition(0.6f, -0.7f, 0.0f);

}




// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	for (auto& c : cameras)
	{
		if(c)
			c->UpdateProjectionMatrix((float)Window::Width() / Window::Height());
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	mover.x = mover.x + moveFactor * deltaTime;
	mover.y = mover.y + moveFactor * deltaTime;
	if (mover.x >= 1.0f)
		moveFactor = -moveFactor;
	else if (mover.x <= -1.0f)
		moveFactor = -moveFactor;
	entities[0]->GetTransform()->SetPosition(mover.x,0.0f,0.0f);
	entities[1]->GetTransform()->SetPosition(0.0f, mover.y, 0.0f);


	scaler.x = scaler.x + scaleFactor * deltaTime;
	scaler.y = scaler.y + scaleFactor * deltaTime;
	if (scaler.x >= 3.0f)
		scaleFactor = -scaleFactor;
	else if (scaler.x <= 0.0f)
		scaleFactor = -scaleFactor;
	entities[2]->GetTransform()->SetScale(scaler);
	entities[3]->GetTransform()->SetScale(-scaler.x, -scaler.y, scaler.z);


	rotator.z = rotator.z + rotateFactor * deltaTime;
	if (rotator.z >= 6.28319f)
		rotator.z = 0.0f;
	entities[4]->GetTransform()->SetRotation(rotator);

	cameras[activeCamera]->Update(deltaTime);
	ImGuiUpdate(deltaTime);
	BuildUI(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}

// --------------------------------------------------------
// Gives data to ImGui so that it knows a new frame has begun
// --------------------------------------------------------
void Game::ImGuiUpdate(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
}

// --------------------------------------------------------
// ImGui Customization happens here
// --------------------------------------------------------
void Game::BuildUI(float deltaTime)
{
	ImGui::Begin(windowName);

	if(disableAll)
		ImGui::BeginDisabled();

	//Assignment Required GUI
	ImGui::Text("This is my custom UI! :D");
	ImGui::Text("Current Framerate: %f fps", ImGui::GetIO().Framerate);
	ImGui::Text("Current Window Resolution: %dx%d", Window::Width(), Window::Height());
	ImGui::ColorEdit4("Background Color", &color[0]);

	//Demo Window Toggle
	if(!demoVis){
		if (ImGui::Button("Show ImGui Demo Window"))
			demoVis = true;
	}
	else{
		if (ImGui::Button("Hide ImGui Demo Window"))
			demoVis = false;
		ImGui::ShowDemoWindow();
	}
	
	if(disableAll)
		ImGui::EndDisabled();

	//Mesh UI
	/*if (ImGui::CollapsingHeader("Mesh Information"))
	{
		ImGui::ColorEdit4("Color Tint", colorTint);
		if (ImGui::CollapsingHeader("Mesh: Triangle"))
		{
			ImGui::Text("Triangles: %i", triangle->GetIndexCount() / 3);
			ImGui::Text("Vertices: %i", triangle->GetVertexCount());
			ImGui::Text("Indices: %i", triangle->GetIndexCount());
		}
		if (ImGui::CollapsingHeader("Mesh: Diamond"))
		{
			ImGui::Text("Triangles: %i", diamond->GetIndexCount() / 3);
			ImGui::Text("Vertices: %i", diamond->GetVertexCount());
			ImGui::Text("Indices: %i", diamond->GetIndexCount());
		}
		if (ImGui::CollapsingHeader("Mesh: Hexagon"))
		{
			ImGui::Text("Triangles: %i", hexagon->GetIndexCount() / 3);
			ImGui::Text("Vertices: %i", hexagon->GetVertexCount());
			ImGui::Text("Indices: %i", hexagon->GetIndexCount());
		}
	}*/

	XMFLOAT3 posi;
	XMFLOAT3 rota;
	XMFLOAT3 scal;

	if (ImGui::CollapsingHeader("Scene Entities")) 
	{
		for (int i = 0; i < entities.size(); i++)
		{
			ImGui::PushID(i);
			if(ImGui::TreeNode("","Entity %d", i))
			{
				ImGui::PushID(i);
				posi = entities[i]->GetTransform()->GetPosition();
				if(ImGui::DragFloat3("Position",(&posi.x),0.01f))
				{
					entities[i]->GetTransform()->SetPosition(posi);
				}
				rota = entities[i]->GetTransform()->GetPitchYawRoll();
				if (ImGui::DragFloat3("Rotation", (&rota.x), 0.01f))
				{
					entities[i]->GetTransform()->SetRotation(rota);
				}
				scal = entities[i]->GetTransform()->GetScale();
				if (ImGui::DragFloat3("Scale", (&scal.x), 0.01f))
				{
					entities[i]->GetTransform()->SetScale(scal);
				}
				ImGui::TreePop();
				ImGui::PopID();
			}
			ImGui::PopID();
		}
		
	}

	if(ImGui::CollapsingHeader("Cameras"))
	{
		ImGui::Text("Current Camera: %i",activeCamera+1);
		ImGui::SeparatorText("Camera Position");
		ImGui::BulletText("X : %f", cameras[activeCamera]->GetTransform()->GetPosition().x);
		ImGui::BulletText("Y : %f", cameras[activeCamera]->GetTransform()->GetPosition().y);
		ImGui::BulletText("Z : %f", cameras[activeCamera]->GetTransform()->GetPosition().z);
		ImGui::SeparatorText("Camera Data");
		ImGui::BulletText("FOV : %f", XMConvertToDegrees(cameras[activeCamera]->GetFOV()));
		ImGui::BulletText("Movespeed : %f", cameras[activeCamera]->GetMovespeed());
		
		ImGui::NewLine();
		if(ImGui::Button("<-- Prev Camera"))
		{
			if (activeCamera > 0)
				activeCamera--;
		}
		ImGui::SameLine();
		if(ImGui::Button("Next Camera -->"))
		{
			if(activeCamera < cameras.size()-1)
				activeCamera++;
		}
	}
	
	ImGui::End();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		for(auto& e : entities)
		{
			//update the constant buffer
			constBufferData vsData;
			vsData.colorTint = XMFLOAT4(colorTint[0], colorTint[1], colorTint[2], colorTint[3]);
			vsData.world = e->GetTransform()->GetWorldMatrix();
			//camera stuff
			vsData.view = cameras[activeCamera]->GetViewMatrix();
			vsData.projection = cameras[activeCamera]->GetProjectionMatrix();


			D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
			Graphics::Context->Map(constBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
			memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
			Graphics::Context->Unmap(constBuffer.Get(), 0);

			//draw the shape
			e->Draw();
		}
	}

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



