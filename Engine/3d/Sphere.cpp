#include "Sphere.h"
#include "PipelineStateObject.h"
#include "externals/DirectXTex/d3dx12.h"
#include "TextureManager.h"
#include <numbers>

void cSphere::Initialize(sTransform* transform, Matrix4x4* viewProjection, Material* material, DirectionalLight* light, sTransform* uvTransform, Vector3* cameraPosition) {
	/*NullCheck*/
	assert(transform);
	assert(uvTransform);
	assert(viewProjection);
	assert(material);
	assert(light);
	assert(cameraPosition);

	transform_ = transform;
	uvTransform_ = uvTransform;
	viewProjection_ = viewProjection;
	material_ = material;
	directionalLight_ = light;
	cameraPosition_ = cameraPosition;

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
	CreateDirectionalLightResource();
	MapDirectionalLightData();
#pragma endregion
	CreateCameraPositionResource();
	MapCameraPositionData();
}

void cSphere::Update() {
	/*WVPマトリックスを作る*/
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);

	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;

	materialData_->color = material_->color;
	materialData_->enbleLighting = material_->enbleLighting;
	materialData_->shininess = material_->shininess;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransformMatrix = uvTransformMatrix;

	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;

	cameraPositionData_->worldPosition.x = cameraPosition_->x;
	cameraPositionData_->worldPosition.y = cameraPosition_->y;
	cameraPositionData_->worldPosition.z = cameraPosition_->z;
}

void cSphere::Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode) {
	//RootSIgnatureを設定。PSOに設定しているけど別途設定が必要
	cDirectXCommon::GetCommandList()->SetGraphicsRootSignature(cPipelineStateObject::Get3DModelRootSignature());
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::Get3DModelPipelineState(blendMode));//PSOを設定
	//VBVを設定
	cDirectXCommon::GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	/*IBVの設定*/
	cDirectXCommon::GetCommandList()->IASetIndexBuffer(&indexBufferView_);
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
	// cameraPosition
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraPositionResource_->GetGPUVirtualAddress());
	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawIndexedInstanced(sphereIndexNum, 1, 0, 0, 0);

}

void cSphere::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * sphereVertexNum);
}

void cSphere::CreateVretexBufferView() {
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = (sizeof(sVertexData) * sphereVertexNum);
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cSphere::MapVertexData() {
	//頂点リソースにデータを書き込む
	vertexData = nullptr;

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));


	//経度分割1つ分の角度
	const float kLonEvery = std::numbers::pi_v<float> *2.0f / float(kSubdivision);
	//緯度分割1つ分の角度
	const float kLatEvery = std::numbers::pi_v<float> / float(kSubdivision);
	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;//Θ
		//経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
			float lon = lonIndex * kLonEvery;//φ
			//頂点にデータを入力する
			//a
			vertexData[start].position.x = std::cosf(lat) * std::cosf(lon);
			vertexData[start].position.y = std::sinf(lat);
			vertexData[start].position.z = std::cosf(lat) * std::sinf(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			//法線情報の追加 
			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;

			//b
			vertexData[start + 1].position.x = std::cosf(lat + kLatEvery) * std::cosf(lon);
			vertexData[start + 1].position.y = std::sinf(lat + kLatEvery);
			vertexData[start + 1].position.z = std::cosf(lat + kLatEvery) * std::sinf(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texcoord = { float(lonIndex) / float(kSubdivision),1.0f - float(latIndex + 1.0f) / float(kSubdivision) };
			//法線情報の追加 
			vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
			vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
			vertexData[start + 1].normal.z = vertexData[start + 1].position.z;

			//c
			vertexData[start + 2].position.x = std::cosf(lat) * std::cosf(lon + kLonEvery);
			vertexData[start + 2].position.y = std::sinf(lat);
			vertexData[start + 2].position.z = std::cosf(lat) * std::sinf(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texcoord = { float(lonIndex + 1.0f) / float(kSubdivision),1.0f - float(latIndex) / float(kSubdivision) };
			//法線情報の追加 
			vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
			vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
			vertexData[start + 2].normal.z = vertexData[start + 2].position.z;


			//d
			vertexData[start + 3].position.x = cosf(lat + kLatEvery) * cosf(lon + kLonEvery);
			vertexData[start + 3].position.y = sinf(lat + kLatEvery);
			vertexData[start + 3].position.z = cosf(lat + kLatEvery) * sinf(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord = { float(lonIndex + 1.0f) / float(kSubdivision),1.0f - float(latIndex + 1.0f) / float(kSubdivision) };
			//法線情報の追加 
			vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
			vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
			vertexData[start + 3].normal.z = vertexData[start + 3].position.z;
		}
	}
}

void cSphere::CreateIndexResource() {
	/*インデックスリソースを作る*/
	indexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(uint32_t) * sphereIndexNum);
}

void cSphere::CreateIndexBufferView() {
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * sphereIndexNum;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void cSphere::MapIndexResource() {
	//データを書き込む
	indexData_ = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		//経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t dataStart = (latIndex * kSubdivision + lonIndex) * 4;
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			indexData_[start] = dataStart;	indexData_[start + 1] = dataStart + 1;	indexData_[start + 2] = dataStart + 2;
			indexData_[start + 3] = dataStart + 1;	indexData_[start + 4] = dataStart + 3;	indexData_[start + 5] = dataStart + 2;
		}
	}
}

void cSphere::CreateMaterialResource() {
	//マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cSphere::MapMaterialData() {
	//マテリアルにデータを書き込む
	materialData_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は白を書き込んでみる
	materialData_->color = material_->color;//RGBA
	//Lightingを有効にする
	materialData_->enbleLighting = material_->enbleLighting;
	/*uvTransformMatrix*/
	materialData_->uvTransformMatrix = MakeIdentity4x4();
	// 光沢度を書き込む
	materialData_->shininess = material_->shininess;
}

void cSphere::CreateWVPResource() {
	/*WVP用のリソースを作る*/
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix));
}

void cSphere::MapWVPData() {
	/*データを書き込む*/
	transformationData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	/*単位行列を書き込んでおく*/
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
}

void cSphere::CreateDirectionalLightResource() {
	//平行光源用のResourceを作成する
	directionalLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(DirectionalLight));
}

void cSphere::MapDirectionalLightData() {
	//データを書き込む
	directionalLightData_ = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//ライトのデータを書き込む
	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;
}

void cSphere::CreateCameraPositionResource() {

	cameraPositionResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(CameraForGPU));
}

void cSphere::MapCameraPositionData() {
	cameraPositionData_ = nullptr;
	cameraPositionResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	cameraPositionData_->worldPosition.x = cameraPosition_->x;
	cameraPositionData_->worldPosition.y = cameraPosition_->y;
	cameraPositionData_->worldPosition.z = cameraPosition_->z;
}

Microsoft::WRL::ComPtr<ID3D12Resource> cSphere::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
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


