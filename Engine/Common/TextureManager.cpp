#include "TextureManager.h"
#include "PipelineStateObject.h"
#include "externals/DirectXTex/d3dx12.h"
#include "Log.h"

cTextureManager* cTextureManager::GetInstance()
{
	static cTextureManager instance;
	return &instance;
}

void cTextureManager::Initialize()
{
	GetInstance()->InitializeSystem();
}

uint32_t cTextureManager::Load(const std::string& filePath)
{
	return GetInstance()->TransferTexture(filePath);
}

void cTextureManager::InitializeSystem()
{
	textureIndex_ = 0;
	CreateFence();
}

uint32_t cTextureManager::TransferTexture(const std::string& filePath)
{
	/*Textureを読んで転送*/
	++textureIndex_;
	/*今回ぶち込むテクスチャーの箱*/
	Texture texture;
	DirectX::ScratchImage mipImage_ = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImage_.GetMetadata();
	texture.resource = CreateTextureResource(cDirectXCommon::GetDevice(), metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(texture.resource.Get(), mipImage_, cDirectXCommon::GetDevice(), cDirectXCommon::GetCommandList());

	/*コマンドのクローズと実行*/
	KickCommand();

	/*metaDataをもとにSRVの設定*/
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	/*SRVを作成するDescriptorHeapの場所を決める*/
	texture.cpuDescHandleSRV = cDirectXCommon::GetCPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), textureIndex_);
	texture.gpuDescHandleSRV = cDirectXCommon::GetGPUDescriptorHandle(cDirectXCommon::GetSRVDescriptorHeap(), cDirectXCommon::GetDescriptorSizeSRV(), textureIndex_);
	/*コンテナに格納*/
	textures_.push_back(texture);
	/*Srvの作成*/
	cDirectXCommon::GetDevice()->CreateShaderResourceView(textures_[textureIndex_ - 1].resource.Get(), &srvDesc, textures_[textureIndex_ - 1].cpuDescHandleSRV);
	return textureIndex_ - 1;
}

Microsoft::WRL::ComPtr<ID3D12Resource> cTextureManager::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
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

void cTextureManager::CreateFence()
{
	HRESULT hr = S_FALSE;
	/*初期値0でFenceを作る*/
	fenceValue_ = 0;
	hr = cDirectXCommon::GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
}

void cTextureManager::KickCommand()
{
	HRESULT hr = S_FALSE;
	/*commandClose*/
	cDirectXCommon::GetCommandList()->Close();

	/*GPUにコマンドリストの実行を行わせる*/
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { cDirectXCommon::GetCommandList() };
	cDirectXCommon::GetCommandQueue()->ExecuteCommandLists(1, commandLists->GetAddressOf());

	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときにFenceの値を指定した値に代入するようにSignalを送る
	cDirectXCommon::GetCommandQueue()->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		/*FenceのSignalを待つためのイベントを作成する*/
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	hr = cDirectXCommon::GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	hr = cDirectXCommon::GetCommandList()->Reset(cDirectXCommon::GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr));

}

DirectX::ScratchImage cTextureManager::LoadTexture(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = cLog::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> cTextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);	//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行きor配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//textureの次元数。普段使っているのは2次元

	//利用するHeapの設定。非常に便利な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの作成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc,//リソースの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//データ転送される設定
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> cTextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

