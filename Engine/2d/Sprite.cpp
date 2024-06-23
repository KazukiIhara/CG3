#include "Sprite.h"
#include "externals/DirectXTex/d3dx12.h"
#include "TextureManager.h"

void cSprite::Initialize(sTransform* transform, Material* material, sTransform* uvTransform)
{
#pragma region 引数からもらったデータをメンバ変数に入れる
	/*NullCheck*/
	assert(transform);
	assert(uvTransform);
	assert(material);

	transform_ = transform;
	uvTransform_ = uvTransform;
	material_ = material;

#pragma endregion

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

}

void cSprite::Update()
{
	/*WVPマトリックスを作る*/
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_->scale, transform_->rotate, transform_->translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(cWinApp::kClientWidth), float(cWinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 WVPMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	*wvpData_ = WVPMatrix;
	materialData_->color = material_->color;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransform = uvTransformMatrix;
}

void cSprite::Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode)
{
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::GetParticlePipelineState(blendMode));//PSOを設定
	/*VBVの設定*/
	cDirectXCommon::GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	/*IBVの設定*/
	cDirectXCommon::GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	/*マテリアルCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	/*wvp用のCBufferの場所を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	/*SRVのDescriptorTableの先頭を設定*/
	cDirectXCommon::GetCommandList()->SetGraphicsRootDescriptorTable(2, cTextureManager::GetTexture()[textureHandle].gpuDescHandleSRV);

	//描画！(DrawCall/ドローコール)
	cDirectXCommon::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void cSprite::CreateVertexResource()
{
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * 6);
}

void cSprite::CreateVretexBufferView()
{
	//頂点バッファビューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(sVertexData) * 4;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cSprite::MapVertexData()
{
	//頂点リソースにデータを書き込む
	vertexData_ = nullptr;

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//1枚目の三角形
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { 1.0f,0.0f };

	/*使わない(多分ほんとは2D用に別の処理をちゃんと作る)*/
	for (uint32_t i = 0; i < 3; i++)
	{
		vertexData_[i].normal = { 0.0f,-1.0f,0.0f };
	}
}

void cSprite::CreateIndexResource()
{
	indexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(uint32_t) * 6);
}

void cSprite::CreateIndexBufferView()
{
	/*リソースの先頭アドレスから使う*/
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	/*使用するリソースのサイズはインデックス6つ分*/
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	/*インデックスはuint32_tとする*/
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void cSprite::MapIndexResource()
{
	/*インデックスリソースにデータを書き込む*/
	indexData_ = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;	indexData_[1] = 1;	indexData_[2] = 2;
	indexData_[3] = 1;	indexData_[4] = 3;	indexData_[5] = 2;
}

void cSprite::CreateMaterialResource()
{
	/*マテリアル用のリソースを作る*/
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cSprite::MapMaterialData()
{
	materialData_ = nullptr;
	/*書き込むためのアドレスを取得*/
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	/*マテリアルデータを書き込む*/
	materialData_->color = material_->color;
	materialData_->enbleLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
}

void cSprite::CreateWVPResource()
{
	/*WVP用のリソースを作る*/
	transformationResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(TransformationMatrix));
}

void cSprite::MapWVPData()
{
	/*データを書き込む*/
	wvpData_ = nullptr;
	/*データを書き込むためのアドレスを取得*/
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	/*単位行列を書き込んでおく*/
	*wvpData_ = MakeIdentity4x4();
}

Microsoft::WRL::ComPtr<ID3D12Resource> cSprite::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
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

