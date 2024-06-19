#include "BaseScene.h"

int cBaseScene::sceneNo = 0;

/// <summary>
/// 仮想デストラクタの定義
/// </summary>
cBaseScene::~cBaseScene() {}

/// <summary>
/// シーン番号のゲッター
/// </summary>
/// <returns></returns>
int cBaseScene::GetSceneNo()
{
	return sceneNo;
}
