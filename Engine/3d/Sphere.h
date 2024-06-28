#pragma once
#include "DirectXCommon.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <memory>
#include "Lazieal.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "PipelineStateObject.h"

class cSphere {
public:
	void Initialize(sTransform* transform, Matrix4x4* viewProjection, Material* material, DirectionalLight* light, sTransform* uvTransform, Vector3* cameraPosition);
	void Update();
	void Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode);

private:/*メンバ関数*/

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

#pragma region WVP
	/*wvp用のリソース作成*/
	void CreateWVPResource();
	/*データを書き込む*/
	void MapWVPData();
#pragma endregion

#pragma region Light
	void CreateDirectionalLightResource();
	void MapDirectionalLightData();
#pragma endregion	

	void CreateCameraPositionResource();
	void MapCameraPositionData();


	/*バッファリソースを作成する*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

private:/*メンバ変数*/
#pragma region 頂点
	/*頂点リソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	/*頂点データ*/
	sVertexData* vertexData;
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
	/*マテリアル情報を受け取る箱*/
	Material* material_;
	/*uvTransformを受け取る箱*/
	sTransform* uvTransform_;
#pragma endregion

#pragma region 変換
	/*WVP用のリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_;
	/*トランスフォームデータを受け取る箱*/
	sTransform* transform_;
	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjection_;
#pragma endregion

#pragma region ライト
	/*ライトのリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	/*ライトデータ*/
	DirectionalLight* directionalLightData_;
	/*ライトを受け取る箱*/
	DirectionalLight* directionalLight_;
#pragma endregion

	// カメラのポジションのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPositionResource_;

	// ポジションデータ
	CameraForGPU* cameraPositionData_;

	Vector3* cameraPosition_;

private:/*メンバ定数*/

	/*球の分割数*/
	const uint32_t kSubdivision = 16;
	/*球の頂点定数*/
	const uint32_t sphereVertexNum = kSubdivision * kSubdivision * 4;
	/*球のインデックス定数*/
	const uint32_t sphereIndexNum = kSubdivision * kSubdivision * 6;
};

