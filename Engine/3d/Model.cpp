#include <sstream>
#include <fstream>
#include "Model.h"
#include "TextureManager.h"


void cModel::Initialize(sTransform* transform, Matrix4x4* viewProjection, DirectionalLight* light, sTransform* uvTransform, Vector3* cameraPosition, PointLight* pointLight, SpotLight* spotLight) {
	/*NullCheck*/

	assert(transform);
	assert(uvTransform);
	assert(viewProjection);
	assert(light);
	assert(cameraPosition);
	assert(pointLight);
	assert(spotLight);

	transform_ = transform;
	uvTransform_ = uvTransform;
	viewProjection_ = viewProjection;
	directionalLight_ = light;
	cameraPosition_ = cameraPosition;
	pointLight_ = pointLight;
	spotLight_ = spotLight;

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
	CreateCameraPositionResource();
	MapCameraPositionData();

	CreatePointLightResource();
	MapPointLightData();

	CreateSpotLightResource();
	MapSpotLightData();

}

void cModel::Update() {
	/*WVPマトリックスを作る*/
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);
	Matrix4x4 worldInverseTransposeMatrix = MakeInverseTransposeMatrix(worldMatrix);

	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
	transformationData_->WorldInverseTransepose = worldInverseTransposeMatrix;

	// 色を書き込む
	materialData_->color = modelData.material.color;
	materialData_->shininess = 40.0f;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransformMatrix = uvTransformMatrix;

	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;

	pointLightData_->color = pointLight_->color;
	pointLightData_->intensity = pointLight_->intensity;
	pointLightData_->position = pointLight_->position;
	pointLightData_->radius = pointLight_->radius;
	pointLightData_->decay = pointLight_->decay;

	spotLightData_->color = spotLight_->color;
	spotLightData_->position = spotLight_->position;
	spotLightData_->intensity = spotLight_->intensity;
	spotLightData_->direction = spotLight_->direction;
	spotLightData_->distance = spotLight_->distance;
	spotLightData_->decay = spotLight_->decay;
	spotLightData_->cosAngle = spotLight_->cosAngle;

}

void cModel::Draw(cPipelineStateObject::Blendmode blendMode) {
	//RootSIgnatureを設定。PSOに設定しているけど別途設定が必要
	cDirectXCommon::GetCommandList()->SetGraphicsRootSignature(cPipelineStateObject::Get3DModelRootSignature());
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::Get3DModelPipelineState(blendMode));//PSOを設定
	//VBVを設定
	cDirectXCommon::GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	cDirectXCommon::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/*マテリアルCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	/*wvp用のCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	/*SRVのDescriptorTableの先頭を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootDescriptorTable(2, cTextureManager::GetTexture()[modelData.material.textureHandle].gpuDescHandleSRV);
	/*DirectionalLight*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// cameraPosition
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraPositionResource_->GetGPUVirtualAddress());
	// pointLight
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	// SpotLight
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}

void cModel::LoadObjFile(const std::string& filename, const std::string& directoryPath) {
	//必要な変数の宣言
	std::vector<Vector4> positions;	//位置
	std::vector<Vector3> normals;	//法線
	std::vector<Vector2> texcoords;	//テクスチャ座標
	std::string line;				//ファイルから読んだ1行を格納するもの

	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);	//ファイルを開く
	assert(file.is_open());								//とりあえず開けなかったら止める

	//ファイルを読み、ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	//先頭の識別子を読む

		//頂点情報を読む
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f")	//三角形を作る
		{
			sVertexData triangle[3]{};
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
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
		} else if (identifier == "mtllib") {
			/*MaterialTemplateLibraryファイルの名前を取得する*/
			std::string materialFilename;	//mtlファイルの名前
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
			modelData.material.textureHandle = cTextureManager::Load(modelData.material.textureFilePath);
		}
	}
}

void cModel::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * modelData.vertices.size());
}

void cModel::CreateVretexBufferView() {
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(sVertexData) * modelData.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cModel::MapVertexData() {
	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData.vertices.data(), sizeof(sVertexData) * modelData.vertices.size());
}

void cModel::CreateIndexResource() {

}

void cModel::CreateIndexBufferView() {

}

void cModel::MapIndexResource() {

}

void cModel::CreateMaterialResource() {
	// マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cModel::MapMaterialData() {
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// テクスチャハンドルを取得
	// mtlのデータから色を書き込む
	materialData_->color = modelData.material.color;
	// Lightingを有効にする
	materialData_->enbleLighting = true;
	// uvTransformMatrix
	materialData_->uvTransformMatrix = MakeIdentity4x4();
}

void cModel::CreateWVPResource() {
	// WVP用のリソースを作る
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix));
}

void cModel::MapWVPData() {
	/*データを書き込む*/
	transformationData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	/*単位行列を書き込んでおく*/
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
}

void cModel::CreateDirectionalLightResource() {
	//平行光源用のResourceを作成する
	directionalLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(DirectionalLight));
}

void cModel::MapDirectionalLightData() {
	//データを書き込む
	directionalLightData_ = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//ライトのデータを書き込む
	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;
}

void cModel::CreateCameraPositionResource() {
	cameraPositionResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(CameraForGPU));
}

void cModel::MapCameraPositionData() {
	cameraPositionData_ = nullptr;
	cameraPositionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	cameraPositionData_->worldPosition.x = cameraPosition_->x;
	cameraPositionData_->worldPosition.y = cameraPosition_->y;
	cameraPositionData_->worldPosition.z = cameraPosition_->z;
}

void cModel::CreatePointLightResource() {
	pointLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(PointLight));
}

void cModel::MapPointLightData() {
	// データを書き込む
	pointLightData_ = nullptr;
	// アドレス取得
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = pointLight_->color;
	pointLightData_->intensity = pointLight_->intensity;
	pointLightData_->position = pointLight_->position;
	pointLightData_->radius = pointLight_->radius;
	pointLightData_->decay = pointLight_->decay;
}

void cModel::CreateSpotLightResource() {
	spotLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(SpotLight));
}

void cModel::MapSpotLightData() {
	spotLightData_ = nullptr;

	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->color = spotLight_->color;
	spotLightData_->position = spotLight_->position;
	spotLightData_->intensity = spotLight_->intensity;
	spotLightData_->direction = spotLight_->direction;
	spotLightData_->distance = spotLight_->distance;
	spotLightData_->decay = spotLight_->decay;
	spotLightData_->cosAngle = spotLight_->cosAngle;
}

MaterialData cModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData; //構築するMaterialData
	std::string line;	//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなかったら止める
	std::string newMtl;

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		} else if (identifier == "Kd") {
			Vector4 color;
			s >> color.x >> color.y >> color.z;
			color.w = 1.0f;
			materialData.color = color;
		}
	}

	return materialData;
}


Microsoft::WRL::ComPtr<ID3D12Resource> cModel::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
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

