#pragma once

class DirectXEngine;
class WinApp;

class ImGuiManager
{
public:

	~ImGuiManager();

	void Initialize(DirectXEngine* dxEngine, WinApp* winApp);

	void Begin();

	void End();

	void Draw();

private:

	DirectXEngine* dxEngine_ = nullptr;
	
	WinApp* winApp_ = nullptr;
};