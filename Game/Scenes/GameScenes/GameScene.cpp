#include "GameScene.h"

#include "DirectXCommon.h"

#include "CameraController.h"
#include "TextureManager.h"

#include "Model.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Sprite.h"
#include "ParticleSystem.h"
#include "ImGuiManager.h"


cGameScene::cGameScene() {
	/*DirectX*/
	dxCommon = cDirectXCommon::GetInstance();
	/*ImGui*/
	imgui_ = cImGuiManager::GetInstance();
}

cGameScene::~cGameScene() {
	if (sceneNo == kThisSceneNo_) {
		/*メインカメラ開放*/
		delete mainCamera_;
		delete particle_;
	}
}
void cGameScene::Initialize() {
	// テクスチャマネージャー初期化
	cTextureManager::Initialize();
	/*カメラ作成*/
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.3f,1.0f,0.0f},{0.0f,12.0f,10.0f} };
	mainCamera_ = new cCameraController();
	mainCamera_->Initialize(&cameraTransform_);

	/*ゲームシーンのビューマトリックスにポインタを渡す*/
	viewProjectionMatrix_ = mainCamera_->GetViewProjectionMatrix();

	/*ライト*/
	light.color = { 1.0f,1.0f,1.0f,1.0f };
	light.direction = { 0.0f,-1.0f,0.0f };
	light.intensity = 1.0f;

	/*Particleの作成*/
	particleUVTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	particle_ = new cParticleSystem();
	particle_->Initialize(viewProjectionMatrix_, &particleUVTransform_);
	particleTextureHandle_ = cTextureManager::Load("Game/Resources/circle.png");

}

void cGameScene::Update() {
	//////////////////////////////////////
	/*ImGuiの開始処理*/
	imgui_->BeginFrame();
	//////////////////////////////////////
	/// ImGuiの処理ココから

#pragma region ImGui
	ImGui::Begin("Config");

	if (ImGui::TreeNodeEx("Model", ImGuiTreeNodeFlags_DefaultOpen)) {

		static int currentBlendModeImGui = 2;
		ImGui::Combo("Texture", &currentBlendModeImGui, BlendMode, IM_ARRAYSIZE(BlendMode));

		switch (currentBlendModeImGui) {
		case 0:
			blendMode_ = cPipelineStateObject::kBlendModeNone;
			break;
		case 1:
			blendMode_ = cPipelineStateObject::kBlendModeNormal;
			break;
		case 2:
			blendMode_ = cPipelineStateObject::kBlendModeAdd;
			break;
		case 3:
			blendMode_ = cPipelineStateObject::kBlendModeSubtract;
			break;
		case 4:
			blendMode_ = cPipelineStateObject::kBlendModeMultiply;
			break;
		case 5:
			blendMode_ = cPipelineStateObject::kBlendModeScreen;
			break;
		}

		ImGui::DragFloat2("uvTranslate", &particleUVTransform_.translate.x, 0.01f);
		ImGui::DragFloat2("uvScale", &particleUVTransform_.scale.x, 0.01f);
		ImGui::SliderAngle("uvTranslate", &particleUVTransform_.rotate.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit3("Color", &light.color.x);
		ImGui::DragFloat3("Direction", &light.direction.x, 0.002f);
		ImGui::DragFloat("Intensity", &light.intensity, 0.01f);
		light.direction = Normalize(light.direction);
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.002f);
		ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.002f);
		ImGui::DragFloat3("Translate", &cameraTransform_.translate.x, 0.01f);

		ImGui::TreePop();
	}

#pragma endregion

	///
	/// ImGuiの処理ここまで
	/// 
	/////////////////////////////////

	/*カメラのアップデート*/
	mainCamera_->Update();

	particle_->Update(*mainCamera_->GetWorldMatrix());

	/*更新処理の最後にImGuiの内部コマンドを生成*/
	imgui_->EndFrame();
}

void cGameScene::Draw() {
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	/////////////////////////////////
	/*描画前処理*/ dxCommon->PreDraw(clearColor);
	/*ImGuiの描画前処理*/
	imgui_->PreDraw();
	/////////////////////////////////
	///
	/// 描画処理ここから
	/// 

	particle_->Draw(particleTextureHandle_, blendMode_);

	///
	/// 描画処理ここまで
	/// 
	/////////////////////////////////
	/*ImGuiは最前面のため一番最後*/
	imgui_->Draw();
	/*描画後処理*/
	dxCommon->PostDraw();
}

void cGameScene::ReleasePointer() {
	/*メインカメラ開放*/
	delete mainCamera_;

}
