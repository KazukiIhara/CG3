#pragma once

#include <memory>
class cBaseScene;
class cWinApp;
class cDirectXCommon;
class cPipelineStateObject;
class cImGuiManager;

class cSceneManager
{
public:
	cSceneManager();
	~cSceneManager();

	/*シーンチェンジ関数*/
	void SceneChange();
	/*ゲームループを呼び出す関数*/
	int Run();

private:

	cWinApp* win_ = nullptr;
	cDirectXCommon* dxCommon_ = nullptr;
	cPipelineStateObject* pso_ = nullptr;
	cImGuiManager* imgui_ = nullptr;

	/*シーンを保持するメンバ関数*/
	std::unique_ptr<cBaseScene> currentScene_[1];

	/*どのステージを呼び出すかを管理する変数*/
	int currentSceneNo_ = 0;	/*現在のシーン*/
	int prevSceneNo_ = 0;		/*前のシーン*/

};