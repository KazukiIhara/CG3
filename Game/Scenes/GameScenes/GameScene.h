#pragma once
#include "BaseScene.h"
#include <memory>
#include "Lazieal.h"
#include "PipelineStateObject.h"

class cDirectXCommon;
class cImGuiManager;
class cCameraController;

class cModel;
class cTriangle;
class cSprite;
class cSphere;

class cGameScene :public cBaseScene
{
public:
	cGameScene();
	~cGameScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/*シーン切り替え時にリソースをデリートする関数*/
	void ReleasePointer()override;

private:

	const int kThisSceneNo_ = Game;

	/*DirectX*/
	cDirectXCommon* dxCommon = nullptr;
	/*ImGui*/
	cImGuiManager* imgui_ = nullptr;

	cPipelineStateObject::Blendmode blendMode_ = cPipelineStateObject::kBlendModeNormal;

	/*カメラ*/
	cCameraController* mainCamera_{};
	sTransform cameraTransform_;
	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjectionMatrix_{};

	/*光源*/
	DirectionalLight light{};

	/*モデル用変数*/
	/*モデル用のTexture*/
	uint32_t modelTextureHandle_;
	/*モデルのトランスフォーム*/
	sTransform modelTransform_;
	/*モデルのトランスフォーム*/
	sTransform modelUVTransform_;
	/*モデル(本人)*/
	cModel* model_{};
	/*モデルデータ*/
	ModelData modelData_{};
};

