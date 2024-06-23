#pragma once
#include <array>
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <cstdint>
#include <string>
#include <memory>
#include "DirectXCommon.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

class cTextureManager
{
public:

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
		// シェーダリソースビューのハンドル(CPU)
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
		// 名前
		std::string name;
	};

	// 初期化
	static void Initialize();

	// テクスチャのロード
	static uint32_t Load(const std::string& filePath);

	/*Textureを渡す*/
	static std::vector<Texture>& GetTexture() { return GetInstance()->textures_; }

private:/*メンバ関数*/

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static cTextureManager* GetInstance();

	cTextureManager() = default;
	~cTextureManager() = default;
	cTextureManager(const cTextureManager&) = delete;
	const cTextureManager& operator=(const cTextureManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	void InitializeSystem();

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <returns></returns>
	uint32_t TransferTexture(const std::string& filePath);

	/*バッファリソースを作成する*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	/*テクスチャー転送用のfence作成*/
	void CreateFence();
	/*テクスチャー転送のコマンドを実行する関数*/
	void KickCommand();

	/*Texture読み込み*/
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/*テクスチャリソースを作る*/
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	/*テクスチャデータを送る*/
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

private: /*メンバ変数*/

	// テクスチャコンテナ
	std::vector<Texture> textures_;
	uint32_t textureIndex_ = 0;
	static const uint32_t textureIndexDecrimentSize_ = 2;

	/*フェンス*/
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	UINT fenceValue_ = 0;

};

