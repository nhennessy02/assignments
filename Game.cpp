#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Material.h"

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
	cameras.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3{ 0,0,-10.0 }, XM_PIDIV2, 5.0));
	cameras.push_back(std::make_shared<Camera>(DirectX::XMFLOAT3{ 0,1.0,-1.0 }, XMConvertToRadians(45), 2.0));
	for (auto& c : cameras)
	{
		c->UpdateProjectionMatrix((float)Window::Width() / Window::Height());
	}
	
	mat1Red = std::make_shared<Material>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), vertexShader, pixelShader);
	mat2Blue = std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vertexShader, pixelShader);
	mat3Yellow = std::make_shared<Material>(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), vertexShader, pixelShader);

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
	vertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device,
		Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(Graphics::Device,
		Graphics::Context, FixPath(L"PixelShader.cso").c_str());
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	sphere = std::make_shared<Mesh>(*FixPath(L"../../Assets/Models/sphere.obj").c_str());
	entities.push_back(std::make_shared<GameEntity>(sphere,mat1Red));
	entities[0]->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

	//entity declaration example
	//Vertex mesh3vertices[] =
	//{
		//{ XMFLOAT3(-0.1f, +0.0f, +0.0f), red },
		//{ XMFLOAT3(-0.05f, -0.2f, +0.0f), yellow },
		//{ XMFLOAT3(+0.0f, +0.2f, +0.0f), red },
		//{ XMFLOAT3(+0.05f, -0.2f, +0.0f), yellow },
		//{ XMFLOAT3(+0.1f, +0.0f, +0.0f), red },
	//};
	//unsigned int mesh3indices[] = { 0, 2, 1, 2, 3, 1, 2, 4, 3 };
	//hexagon = std::make_shared<Mesh>(mesh3vertices, 5, mesh3indices, 9);

	//entities.push_back(std::make_shared<GameEntity>(hexagon, mat1Red));
	//entities[4]->GetTransform()->SetPosition(0.6f, -0.7f, 0.0f);

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
		//goes through all entities
		for(auto& e : entities)
		{
			//filling external data struct
			std::shared_ptr<SimpleVertexShader> vs = e->GetMaterial()->GetVertexShader();
			vs->SetFloat4("colorTint", e->GetMaterial()->GetColorTint());
			vs->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix()); 
			vs->SetMatrix4x4("view", cameras[activeCamera]->GetViewMatrix());
			vs->SetMatrix4x4("projection", cameras[activeCamera]->GetProjectionMatrix());

			//Copy the data to the GPU
			vs->CopyAllBufferData();

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



