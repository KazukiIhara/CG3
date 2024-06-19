#include <sstream>
#include <fstream>
#include "Particle.h"
#include "TextureManager.h"


void cParticle::Initialize(Matrix4x4* viewProjection, DirectionalLight* light, sTransform* uvTransform)
{
	/*NullCheck*/
	assert(uvTransform);
	assert(viewProjection);
	assert(light);

	modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	modelData_.material.color = { 1.0f,1.0f,1.0f,1.0f };

	modelData_.material.enbleLighting = false;

	for (uint32_t index = 0; index < instanceCount_; ++index)
	{
		transform_[index].scale = { 1.0f,1.0f,1.0f };
		transform_[index].rotate = { 0.0f,0.0f,0.0f };
		transform_[index].translate = { index * 0.1f,index * 0.1f,index * 0.1f };
	}

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
	CreateSRV();
}

void cParticle::Update()
{
	/*WVPマトリックスを作る*/

	for (uint32_t index = 0; index < instanceCount_; ++index)
	{
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform_[index].scale, transform_[index].rotate, transform_[index].translate);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);

		transformationData_[index].WVP = worldViewProjectionMatrix;
		transformationData_[index].World = worldMatrix;
	}

	// 色を書き込む
	materialData_->color = modelData_.material.color;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransform = uvTransformMatrix;

	directionalLightData_->color = directionalLight_->color;
	directionalLightData_->direction = directionalLight_->direction;
	directionalLightData_->intensity = directionalLight_->intensity;

}

void cParticle::Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode)
{
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::GetPipelineState(blendMode));//PSOを設定
	//VBVを設定
	cDirectXCommon::GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	cDirectXCommon::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/*マテリアルCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// StructuredBufferのSRVを設定する
	cDirectXCommon::GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU);
	/*SRVのDescriptorTableの先頭を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootDescriptorTable(2, cTextureManager::GetTexture()[textureHandle].gpuDescHandleSRV);
	/*DirectionalLight*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawInstanced(6, instanceCount_, 0, 0);
}

void cParticle::CreateVertexResource()
{
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * modelData_.vertices.size());
}

void cParticle::CreateVretexBufferView()
{
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(sVertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cParticle::MapVertexData()
{
	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(sVertexData) * modelData_.vertices.size());
}

void cParticle::CreateIndexResource()
{

}

void cParticle::CreateIndexBufferView()
{

}

void cParticle::MapIndexResource()
{

}

void cParticle::CreateMaterialResource()
{
	// マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cParticle::MapMaterialData()
{
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// mtlのデータから色を書き込む
	materialData_->color = modelData_.material.color;
	// Lightingを有効にする
	materialData_->enbleLighting = true;
	// uvTransform
	materialData_->uvTransform = MakeIdentity4x4();
}

void cParticle::CreateWVPResource()
{
	// WVP用のリソースを作る
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix) * instanceCount_);
}

void cParticle::MapWVPData()
{
	/*データを書き込む*/
	transformationData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	/*単位行列を書き込んでおく*/
	for (uint32_t index = 0; index < instanceCount_; ++index)
	{
		transformationData_[index].WVP = MakeIdentity4x4();
		transformationData_[index].World = MakeIdentity4x4();
	}
}

void cParticle::CreateDirectionalLightResource()
{
	//平行光源用のResourceを作成する
	directionalLightResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(DirectionalLight));
}

void cParticle::MapDirectionalLightData()
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

void cParticle::CreateSRV()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceCount_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = cDirectXCommon::GetCPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU = cDirectXCommon::GetGPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), 3);
	cDirectXCommon::GetDevice()->CreateShaderResourceView(transformationResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU);
}

Microsoft::WRL::ComPtr<ID3D12Resource> cParticle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
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


