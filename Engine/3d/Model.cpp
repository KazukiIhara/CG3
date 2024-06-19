#include <sstream>
#include <fstream>
#include "Model.h"
#include "TextureManager.h"

void cModel::Initialize(ModelData* modelData, sTransform* transform, Matrix4x4* viewProjection, DirectionalLight* light, sTransform* uvTransform)
{
	/*NullCheck*/
	assert(modelData);
	assert(transform);
	assert(uvTransform);
	assert(viewProjection);
	assert(light);

	modelData_ = modelData;
	transform_ = transform;
	uvTransform_ = uvTransform;
	viewProjection_ = viewProjection;
	directionalLight_ = light;

#pragma region 頂点データ
	/*頂点リソースの作成*/
	CreateVertexResource();
	/*頂点バッファビューの作成*/
	CreateVretexBufferView();
	/*頂点データの書き込み*/
	MapVertexData();
#pragma endregion

#pragma region インデックスデータ
	/*描画用のインデックスリソースを作成*/
	CreateIndexResource();
	/*インデックスバッファビューの作成*/
	CreateIndexBufferView();
	/*インデックスリソースにデータを書き込む*/
	MapIndexResource();
#pragma endregion

#pragma region マテリアルデータ
	/*マテリアル用のリソース作成*/
	CreateMaterialResource();
	/*マテリアルにデータを書き込む*/
	MapMaterialData();
#pragma endregion

#pragma region 変換データ
	/*wvp用のリソース作成*/
	CreateWVPResource();
	/*データを書き込む*/
	MapWVPData();
#pragma endregion

#pragma region ライト
	/*DirectionalLight用のリソースを作成*/
	CreateDirectionalLightResource();
	/*データを書き込む*/
	MapDirectionalLightData();
#pragma endregion

}

void cModel::Update()
{
	/*WVPマトリックスを作る*/
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);

	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;

	// 色を書き込む
	materialData_->color = modelData_->material.color;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransform = uvTransformMatrix;

	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;

}

void cModel::Draw(uint32_t textureHandle)
{
	//VBVを設定
	cDirectXCommon::GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	cDirectXCommon::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/*マテリアルCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	/*wvp用のCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	/*SRVのDescriptorTableの先頭を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootDescriptorTable(2, cTextureManager::GetTexture()[textureHandle].gpuDescHandleSRV);
	/*DirectionalLight*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawInstanced(UINT(modelData_->vertices.size()), 1, 0, 0);
}

ModelData cModel::LoadObjFile(const std::string& filename, const std::string& directoryPath)
{
	//必要な変数の宣言
	ModelData modelData;	//構築するmodelData
	uint32_t meshIndex = 0;
	bool isFirstMesh = true;
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcoords;	//テクスチャ座標
	std::string line;				//ファイルから読んだ1行を格納するもの

	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());								//とりあえず開けなかったら止める

	//ファイルを読み、ModelDataを構築
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		//頂点情報を読む
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f")	//三角形を作る
		{
			sVertexData triangle[3]{};
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');	//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];

				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib")
		{
			/*MaterialTemplateLibraryファイルの名前を取得する*/
			std::string materialFilename;	//mtlファイルの名前
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

void cModel::CreateVertexResource()
{
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * modelData_->vertices.size());
}

void cModel::CreateVretexBufferView()
{
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(sVertexData) * modelData_->vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cModel::MapVertexData()
{
	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_->vertices.data(), sizeof(sVertexData) * modelData_->vertices.size());
}

void cModel::CreateIndexResource()
{

}

void cModel::CreateIndexBufferView()
{

}

void cModel::MapIndexResource()
{

}

void cModel::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cModel::MapMaterialData()
{
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// mtlのデータから色を書き込む
	materialData_->color = modelData_->material.color;
	// Lightingを有効にする
	materialData_->enbleLighting = true;
	// uvTransform
	materialData_->uvTransform = MakeIdentity4x4();
}

void cModel::CreateWVPResource()
{
	// WVP用のリソースを作る
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix));
}

void cModel::MapWVPData()
{
	/*データを書き込む*/
	transformationData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	/*単位行列を書き込んでおく*/
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
}

void cModel::CreateDirectionalLightResource()
{
	//平行光源用のResourceを作成する
	directionalLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(DirectionalLight));
}

void cModel::MapDirectionalLightData()
{
	//データを書き込む
	directionalLightData_ = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//ライトのデータを書き込む
	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;
}

MaterialData cModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData; //構築するMaterialData
	std::string line;	//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなかったら止める
	std::string newMtl;

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		} else if (identifier == "Kd")
		{
			Vector4 color;
			s >> color.x >> color.y >> color.z;
			color.w = 1.0f;
			materialData.color = color;
		}
	}

	return materialData;
}


Microsoft::WRL::ComPtr<ID3D12Resource> cModel::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	HRESULT hr = S_FALSE;
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う

	//マテリアル用のリソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//バッファリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource>resource = nullptr;
	hr = device->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

