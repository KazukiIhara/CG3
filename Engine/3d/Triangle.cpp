#include "Triangle.h"
#include "PipelineStateObject.h"
#include "externals/DirectXTex/d3dx12.h"
#include "TextureManager.h"

void cTriangle::Initialize(sTransform* transform, Matrix4x4* viewProjection, Vector4* color)
{
#pragma region 引数からもらったデータをメンバ変数に入れる
	/*NullCheck*/
	assert(transform);
	assert(viewProjection);
	assert(color);

	transform_ = transform;
	viewProjection_ = viewProjection;
	color_ = color;

#pragma endregion

#pragma region 頂点データ
	/*頂点リソースの作成*/
	CreateVertexResource();
	/*頂点バッファビューの作成*/
	CreateVretexBufferView();
	/*頂点データの書き込み*/
	MapVertexData();
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

	CreateDirectionalLightResource();
}

void cTriangle::Update()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);

	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldMatrix;
	materialData_->color = *color_;
}

void cTriangle::Draw(uint32_t textureHandle)
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
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void cTriangle::CreateVertexResource()
{
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * 3);
}

void cTriangle::CreateVretexBufferView()
{
	//頂点バッファビューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(sVertexData) * 3;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cTriangle::MapVertexData()
{
	//頂点リソースにデータを書き込む
	sVertexData* vertexData = nullptr;

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	//上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	//右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };

}

void cTriangle::CreateMaterialResource()
{
	/*マテリアル用のリソースを作る*/
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cTriangle::MapMaterialData()
{
	materialData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	/*今回は赤*/
	materialData_->color = *color_;
	materialData_->enbleLighting = false;
}

void cTriangle::CreateWVPResource()
{
	/*WVP用のリソースを作る*/
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix));
}

void cTriangle::MapWVPData()
{
	/*データを書き込む*/
	wvpData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	/*単位行列を書き込んでおく*/
	wvpData_->WVP = MakeIdentity4x4();
	wvpData_->World = MakeIdentity4x4();
}

void cTriangle::CreateDirectionalLightResource()
{
	//平行光源用のResourceを作成する
	directionalLightResource = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(DirectionalLight));
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;
	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//今回は白を書き込む
	directionalLightData->color.x = 1.0f;
	directionalLightData->color.y = 1.0f;
	directionalLightData->color.z = 1.0f;
	directionalLightData->color.w = 1.0f;
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
}

Microsoft::WRL::ComPtr<ID3D12Resource> cTriangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
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
