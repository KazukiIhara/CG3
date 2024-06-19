#pragma once
#include "DirectXCommon.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <memory>
#include "Lazieal.h"
#include "externals/DirectXTex/DirectXTex.h"

class cTriangle
{
public:
	/*初期化*/
	void Initialize(sTransform* transform, Matrix4x4* viewProjection, Vector4* color);
	/*更新*/
	void Update();
	/*描画*/
	void Draw(uint32_t textureHandle);

private:/*メンバ関数*/

	/*バッファリソースを作成する*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

#pragma region Vertex
	/*頂点リソースの作成*/
	void CreateVertexResource();
	/*頂点バッファビューの作成*/
	void CreateVretexBufferView();
	/*頂点データの書き込み*/
	void MapVertexData();
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

#pragma endregion	
private: /*メンバ変数*/
#pragma region 頂点
	/*頂点リソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	/*VBV*/
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
#pragma endregion

#pragma region マテリアル
	/*マテリアルリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
	/*マテリアルデータ*/
	Material* materialData_ = nullptr;

	/*色情報*/
	Vector4* color_;
#pragma endregion

#pragma region 変換
	/*WVP用のリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* wvpData_;

	/*トランスフォームデータを受け取る箱*/
	sTransform* transform_;

	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjection_;

#pragma endregion

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
};