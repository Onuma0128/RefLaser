#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")
#include <memory>

#include "Framework.h"
#include "MyGame.h"
#include "D3DResourceLeakChecker.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;

	std::unique_ptr<Framework> game = std::make_unique<MyGame>();

	game->Run();

	return 0;
}