#include <sstream>
#include <fstream>
#include "Particle.h"
#include "TextureManager.h"
#include "MathOperator.h"

void cParticle::Initialize(Matrix4x4* viewProjection, sTransform* uvTransform) {

	// 乱数生成器の初期化
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	/*NullCheck*/
	assert(uvTransform);
	assert(viewProjection);

	modelData_.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });
	modelData_.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });

	modelData_.material.color = { 1.0f,1.0f,1.0f,1.0f };

	modelData_.material.enbleLighting = false;

	// パーティクルの生成
	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		particles[index] = MakeNewParticle(randomEngine);
	}

	uvTransform_ = uvTransform;
	viewProjection_ = viewProjection;

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

#pragma region Instancing
	// Instancingリソースを作る
	CreateInstancingResource();
	// Instancingデータを書き込む
	MapInstancingData();

#pragma endregion

	CreateSRV();
}

void cParticle::Update(const Matrix4x4& cameraMatrix) {
	// 描画すべきインスタンス数
	instanceCount_ = 0;

	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		// 生存時間を過ぎていたら更新せず描画対象にしない
		if (particles[index].lifeTime <= particles[index].currentTime) {
			continue;
		}

		// 180度回す回転行列を作成する
		Matrix4x4 backFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);

		// WVPマトリックスを求める
		Matrix4x4 scaleMatrix = MakeScaleMatrix(particles[index].transform.scale);
		Matrix4x4 billboardMatrix = backFrontMatrix * cameraMatrix;
		// 平行移動成分を削除
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;

		Matrix4x4 translateMatrix = MakeTranslateMatrix(particles[index].transform.translate);

		Matrix4x4 worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, *viewProjection_);

		// 移動
		//Move(index);
		// 経過時間を足す
		//particles[index].currentTime += kDeltaTime;
		// 透明度
		float alpha = 1.0f - (particles[index].currentTime / particles[index].lifeTime);

		instancingData_[instanceCount_].WVP = worldViewProjectionMatrix;
		instancingData_[instanceCount_].World = worldMatrix;
		// 色を入力
		instancingData_[instanceCount_].color.x = particles[index].color.x;
		instancingData_[instanceCount_].color.y = particles[index].color.y;
		instancingData_[instanceCount_].color.z = particles[index].color.z;
		instancingData_[instanceCount_].color.w = alpha;

		// 生きているParticleの数を1つカウントする
		instanceCount_++;
	}

	// 色を書き込む
	materialData_->color = modelData_.material.color;

	/*uvTranform用のMatrixを作る*/
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform_->scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform_->rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform_->translate));
	materialData_->uvTransform = uvTransformMatrix;
}

void cParticle::Draw(uint32_t textureHandle, cPipelineStateObject::Blendmode blendMode) {
	//RootSIgnatureを設定。PSOに設定しているけど別途設定が必要
	cDirectXCommon::GetCommandList()->SetGraphicsRootSignature(cPipelineStateObject::GetParticleRootSignature());
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::GetParticlePipelineState(blendMode));//PSOを設定
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
	//描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	cDirectXCommon::GetCommandList()->DrawInstanced(6, instanceCount_, 0, 0);
}

void cParticle::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(sVertexData) * modelData_.vertices.size());
}

void cParticle::CreateVretexBufferView() {
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(sVertexData) * modelData_.vertices.size());
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(sVertexData);
}

void cParticle::MapVertexData() {
	vertexData_ = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(sVertexData) * modelData_.vertices.size());
}

void cParticle::CreateIndexResource() {

}

void cParticle::CreateIndexBufferView() {

}

void cParticle::MapIndexResource() {

}

void cParticle::CreateMaterialResource() {
	// マテリアル用のリソースを作る。
	materialResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(Material));
}

void cParticle::MapMaterialData() {
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// mtlのデータから色を書き込む
	materialData_->color = modelData_.material.color;
	// Lightingを有効にする
	materialData_->enbleLighting = false;
	// uvTransform
	materialData_->uvTransform = MakeIdentity4x4();
}

void cParticle::CreateInstancingResource() {
	// instancing用のリソースを作る
	instancingResource_ = CreateBufferResource(cDirectXCommon::GetDevice(), sizeof(ParticleForGPU) * kNumMaxInstance);
}

void cParticle::MapInstancingData() {
	instancingData_ = nullptr;
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {

		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

cParticle::Particle cParticle::MakeNewParticle(std::mt19937& randomEngine) {
	// 出現位置と移動量の乱数の生成
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	// 色を決める乱数の生成
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	// 生存時間の乱数の生成(1秒から3秒の間生存)
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);

	Particle particle;
	// トランスフォームの設定
	particle.transform.scale = { 1.0f,1.0f,1.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	// 位置と移動量を[-1,1]の範囲でランダムに初期化
	particle.transform.translate = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	// 移動量の設定
	particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	// 色の設定
	particle.color = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine),1.0f };

	// 生存時間の設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	return particle;
}

void cParticle::Move(uint32_t index) {
	particles[index].transform.translate += Multiply(kDeltaTime, particles[index].velocity);
}

void cParticle::CreateSRV() {
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = cDirectXCommon::GetCPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), 1);
	instancingSrvHandleGPU = cDirectXCommon::GetGPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), 1);
	cDirectXCommon::GetDevice()->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU);
}

Microsoft::WRL::ComPtr<ID3D12Resource> cParticle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
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


