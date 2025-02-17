#pragma once
#include "Lazieal.h"
#include <memory>
#include "DirectXCommon.h"
#include "PipelineStateObject.h"
#include <random>
#include <iostream>
#include <list>
#include <algorithm>
#include "Collision.h"

class cParticleSystem {
public:
	struct Particle {
		sTransform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};

	struct ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct Emitter {
		sTransform transform;	// エミッターのTransform
		uint32_t count;			// 発生数
		float frequency;		// 発生頻度
		float frequencyTime;	// 頻度用時刻
	};

	struct AccelerationField {
		Vector3 acceleration;	// 加速度
		cCollision::AABB area;	// 範囲
	};

	void Initialize(Matrix4x4* viewProjection, sTransform* uvTransform);
	void Update(const Matrix4x4& cameraMatrix);
	void Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode);

private:
#pragma region Vertex
	/*頂点リソースの作成*/
	void CreateVertexResource();
	/*頂点バッファビューの作成*/
	void CreateVretexBufferView();
	/*頂点データの書き込み*/
	void MapVertexData();
#pragma endregion

#pragma region Index
	/*描画用のインデックスリソースを作成*/
	void CreateIndexResource();
	/*インデックスバッファビューの作成*/
	void CreateIndexBufferView();
	/*インデックスリソースにデータを書き込む*/
	void MapIndexResource();
#pragma endregion

#pragma region Material
	/*マテリアルリソースの作成*/
	void CreateMaterialResource();
	/*マテリアルデータの書き込み*/
	void MapMaterialData();
#pragma endregion

#pragma region Instancing
	void CreateInstancingResource();
	void MapInstancingData();

#pragma endregion
#pragma region Particle
	// Particleの生成
	Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);

#pragma endregion

	// instancingSrvを作る
	void CreateSRV();

	/*バッファリソースを作成する*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

private:/*メンバ変数*/

	// パーティクルの最大数
	static const uint32_t kNumMaxInstance = 100;

#pragma region モデル
	/*モデルデータを受け取る箱*/
	ModelData modelData_;

#pragma endregion

#pragma region 頂点
	/*頂点リソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	/*頂点データ*/
	sVertexData* vertexData_;
	/*VBV*/
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
#pragma endregion

#pragma region インデックス
	/*インデックスリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	/*インデックスバッファビュー*/
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	/*インデックスデータ*/
	uint32_t* indexData_;
#pragma endregion

#pragma region マテリアル
	/*マテリアルリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	/*マテリアルデータ*/
	Material* materialData_ = nullptr;
	/*uvTransformを受け取る箱*/
	sTransform* uvTransform_;
#pragma endregion

#pragma region 変換
	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjection_;
#pragma endregion

#pragma region Instancing
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	ParticleForGPU* instancingData_ = nullptr;
#pragma endregion

#pragma region Particle

	// パーティクル
	std::list<Particle> particles_;
	// デルタタイムを設定。ひとまず60fps固定
	const float kDeltaTime = 1.0f / 60.0f;

#pragma endregion
#pragma region Emitter
	// パーティクルの発生関数
	std::list<Particle> Emit(const Emitter& emitter, std::mt19937& randomEngine);
	// Emitter
	Emitter emitter_{
	.count = 3,
	.frequency = 0.5f,		// 0.5秒ごとに発生
	.frequencyTime = 0.0f,	// 発生頻度用の時刻、0で初期化
	};
#pragma endregion
#pragma region Field

	AccelerationField accelerationField;

#pragma endregion

	// instance描画する際に使う変数
	uint32_t instanceCount_ = kNumMaxInstance;

	// srvGpuハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	/// 
	/// for Debug
	/// 
	bool isUpdate = false;
	bool isUseBillboard = false;
};