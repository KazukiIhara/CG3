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
		delete sphere_;
		delete terrain_;
	}
}
void cGameScene::Initialize() {
	// テクスチャマネージャー初期化
	cTextureManager::Initialize();
	/*カメラ作成*/
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.1f,1.0f,0.0f},{0.0f,5.0f,15.0f} };
	mainCamera_ = new cCameraController();
	mainCamera_->Initialize(&cameraTransform_);

	/*ゲームシーンのビューマトリックスにポインタを渡す*/
	viewProjectionMatrix_ = mainCamera_->GetViewProjectionMatrix();

	/*ライト*/
	directionalLight.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight.direction = { 0.0f,-1.0f,0.0f };
	directionalLight.intensity = 0.0f;

	pointLight_.color = { 1.0f,1.0f,1.0f,1.0f };
	pointLight_.intensity = 1.0f;
	pointLight_.position = { 0.0f,2.0f,0.0f };
	pointLight_.radius = 10.0f;
	pointLight_.decay = 5.0f;

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enbleLighting = true;
	material_.shininess = 40.0f;

	uvTransform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	sphere_ = new cSphere();
	sphere_->Initialize(&transform_, viewProjectionMatrix_, &material_, &directionalLight, &uvTransform_, &cameraTransform_.translate, &pointLight_);

	terrain_ = new cModel();
	terrain_->LoadObjFile("terrain.obj");
	terrain_->Initialize(&transform_, viewProjectionMatrix_, &directionalLight, &uvTransform_, &cameraTransform_.translate, &pointLight_);

	textureHandle_ = cTextureManager::Load("Game/Resources/monsterBall.png");
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

		static int currentBlendModeImGui = 1;
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

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("DirectionalLight", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit3("Color", &directionalLight.color.x);
		ImGui::DragFloat3("Direction", &directionalLight.direction.x, 0.002f);
		ImGui::DragFloat("Intensity", &directionalLight.intensity, 0.01f);
		directionalLight.direction = Normalize(directionalLight.direction);
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("PointLight", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit3("Color", &pointLight_.color.x);
		ImGui::DragFloat3("Position", &pointLight_.position.x, 0.01f);
		ImGui::DragFloat("Intensity", &pointLight_.intensity, 0.01f);
		ImGui::DragFloat("Radius", &pointLight_.radius, 0.01f);
		ImGui::DragFloat("Decay", &pointLight_.decay, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.002f);
		ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.002f);
		ImGui::DragFloat3("Translate", &cameraTransform_.translate.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Sphere", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Scale", &transform_.scale.x, 0.002f);
		ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.002f);
		ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);

		ImGui::TreePop();
	}

#pragma endregion

	///
	/// ImGuiの処理ここまで
	/// 
	/////////////////////////////////

	/*カメラのアップデート*/
	mainCamera_->Update();

	terrain_->Update();
	sphere_->Update();

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

	terrain_->Draw(blendMode_);
	sphere_->Draw(textureHandle_, blendMode_);

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
