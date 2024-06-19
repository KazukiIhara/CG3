#pragma once

class cBaseScene
{
protected:
	/*シーン番号を管理する変数*/
	static int sceneNo;
public:

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void ReleasePointer() = 0;

	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~cBaseScene();

	/*シーン番号のゲッター*/
	int GetSceneNo();


};

/*ゲームシーン列挙型*/
enum eScene
{
	Game,
	Production
};