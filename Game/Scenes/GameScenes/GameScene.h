#pragma once
#include "BaseScene.h"
#include <memory>
#include "Lazieal.h"
#include "PipelineStateObject.h"

class cDirectXCommon;
class cImGuiManager;
class cCameraController;

class cModel;
class cPrimitiveSystem;
class cTriangle;
class cSprite;
class cSphere;
class cParticleSystem;

class cGameScene :public cBaseScene {
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

	// BlendMode
	cPipelineStateObject::Blendmode blendMode_ = cPipelineStateObject::kBlendModeNone;

	/*カメラ*/
	cCameraController* mainCamera_{};
	sTransform cameraTransform_;
	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjectionMatrix_{};

	/*光源*/
	DirectionalLight directionalLight{};
	PointLight pointLight_{};
	SpotLight spotLight_{};

	sTransform transform_;
	sTransform uvTransform_;


	// モデル
	cPrimitiveSystem* primitive_{};


	// BlendMode
	const char* BlendMode[6] = { "None","Normal","Add","Subtract","Multiply","Screen" };
};

