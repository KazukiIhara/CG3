#include "SceneManager.h"

#include "BaseScene.h"
#include "GameScene.h"

#include "Log.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "PipelineStateObject.h"


#include "ImGuiManager.h"

cSceneManager::cSceneManager()
{
	/*ログ*/
	cLog::Log("ReHello,DiretX!!\n");
	/*ゲームウィンドウの作成*/
	win_ = cWinApp::GetInstance();
	win_->CreateGameWindow();

	/*DirectX初期化*/
	dxCommon_ = cDirectXCommon::GetInstance();
	dxCommon_->Initialize();
	/*PSO設定*/
	pso_ = cPipelineStateObject::GetInstance();
	pso_->Initialize();

	/*ImGui*/
	imgui_ = cImGuiManager::GetInstance();
	imgui_->Initialize();

	/*シーンの設定*/
	/*初期シーン*/
	currentSceneNo_ = Game;
	/*ひとつ前のシーン*/
	prevSceneNo_ = Game;

	currentScene_[0] = std::make_unique<cGameScene>();

	currentScene_[currentSceneNo_]->Initialize();
}

cSceneManager::~cSceneManager()
{
}

void cSceneManager::SceneChange()
{
	/*シーンのチェック*/
	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = currentScene_[currentSceneNo_]->GetSceneNo();

	/*シーンの変更チェック*/
	if (prevSceneNo_ != currentSceneNo_)
	{
		currentScene_[prevSceneNo_]->ReleasePointer();
		currentScene_[currentSceneNo_]->Initialize();
	}

}

int cSceneManager::Run()
{
	/*ウィンドウのxボタンが押されるまでループ*/
	while (!win_->ProcessMessage())
	{
		SceneChange();

		currentScene_[currentSceneNo_]->Update();

		currentScene_[currentSceneNo_]->Draw();
	}

	/*ImGui終了処理*/
	imgui_->Finalize();

	/*windowの破棄*/
	win_->TerminateGameWindow();
	/*終了ログ*/
	cLog::Log("Bye,DircetX!!\n");

	return 0;
}
