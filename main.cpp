#include <Windows.h>
#include "DirectXCommon.h"
#include "SceneManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	/*リークチェッカC++環境ならSingltonもチェックしてくれるはず*/
	D3DResourceLeakChecker* leakCheck = nullptr;
	leakCheck = D3DResourceLeakChecker::GetInstance();

	/*ゲーム*/
	std::unique_ptr<cSceneManager> game = std::make_unique<cSceneManager>();

	/*実行*/
	game->Run();

	CoUninitialize();

	/*終わり*/
	return 0;
}