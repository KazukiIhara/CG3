#pragma once
#include "Lazieal.h"
#include <memory>
#include "DirectXCommon.h"
#include "PipelineStateObject.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class cPrimitiveSystem {
public:

	void Initialize(sTransform* transform, Matrix4x4* viewProjection, sTransform* uvTransform);
	void Update();
	void Draw(cPipelineStateObject::Blendmode blendMode);

	/*Objファイルからデータを読み取る関数*/
	void LoadObjFile(const std::string& filename, const std::string& directoryPath = "Game/Resources");
	void LoadModelFileWithAssimp(const std::string& filename, const std::string& directoryPath = "Game/Resources");
private:
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

	/*マテリアルデータをmtlファイルから作成する*/
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	/*バッファリソースを作成する*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	Node ReadNode(aiNode* node);

private:/*メンバ変数*/

#pragma region モデル
	/*モデルデータを受け取る箱*/
	ModelData modelData;

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
	/*WVP用のリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_;
	/*トランスフォームデータを受け取る箱*/
	sTransform* transform_;
	/*ビュープロジェクションを受け取る箱*/
	Matrix4x4* viewProjection_;
#pragma endregion

};

