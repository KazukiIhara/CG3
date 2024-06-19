#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <cassert>

#include "WinApp.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

/*リークチェッカー*/
class D3DResourceLeakChecker
{
public:
	static D3DResourceLeakChecker* GetInstance();
private:
	D3DResourceLeakChecker() = default;
	~D3DResourceLeakChecker();
	D3DResourceLeakChecker(const D3DResourceLeakChecker&) = delete;
	const D3DResourceLeakChecker& operator=(const D3DResourceLeakChecker&) = delete;
};

class cDirectXCommon
{
public: /*メンバ関数*/

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static cDirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t backBufferWidth = cWinApp::kClientWidth,
		int32_t backBufferHeight = cWinApp::kClientHeight, bool enableDebugLayer = true);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw(float clearColor[]);

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 深度クリア
	/// </summary>
	void ClearDepthView();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget(float clearColor[]);

	/// <summary>
	/// レンダーターゲットのセット
	/// </summary>
	void SetRenderTargets();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns></returns>
	static ID3D12Device* GetDevice()
	{
		return GetInstance()->device_.Get();
	}

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns></returns>
	static ID3D12GraphicsCommandList* GetCommandList()
	{
		return GetInstance()->commandList_.Get();
	}

	/// <summary>
	/// コマンドキューの取得
	/// </summary>
	/// <returns></returns>
	static ID3D12CommandQueue* GetCommandQueue()
	{
		return GetInstance()->commandQueue_.Get();
	}

	static ID3D12CommandAllocator* GetCommandAllocator()
	{
		return GetInstance()->commandAllocator_.Get();
	}

	/// <summary>
	/// スワップチェインディスクリプターを取得
	/// </summary>
	/// <returns></returns>
	static DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()
	{
		return GetInstance()->swapChainDesc_;
	}

	/// <summary>
	/// rtvDescを取得
	/// </summary>
	static D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc()
	{
		return GetInstance()->rtvDesc_;
	}

	/// <summary>
	/// srvディスクリプタヒープを取得
	/// </summary>
	/// <returns></returns>
	static ID3D12DescriptorHeap* GetSRVDescriptorHeap()
	{
		return GetInstance()->srvDescriptorHeap_.Get();
	}

	static uint32_t GetDescriptorSizeSRV()
	{
		return GetInstance()->descriptorSizeSRV_;
	}

	/*CPUの特定のインデックスのハンドルを取得*/
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	/*GPUの特定のインデックスのハンドルを取得*/
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

public:


private: /*メンバ関数*/
	cDirectXCommon() = default;
	~cDirectXCommon() = default;
	cDirectXCommon(const cDirectXCommon&) = delete;
	const cDirectXCommon& operator=(const cDirectXCommon&) = delete;

	/// <summary>
	/// DXGIデバイスの生成と初期化
	/// </summary>
	void InitializeDXGIDevice(bool enableDebugLayer);

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンを生成する
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲットビューを生成する
	/// </summary>
	void CreateRenderTargetView();

	/// <summary>
	/// デプスステンシルビューを生成する
	/// </summary>
	void CreateDepthStencilView();

	/// <summary>
	/// フェンスを生成する
	/// </summary>
	void CreateFence();


	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	/// <summary>
	/// ディスクリプタヒープの作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);



private: /*メンバ変数*/

	/*SUCCEEDEDでエラー判別君*/
	HRESULT hr_ = S_FALSE;

	/*DXGIファクトリー*/
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory_ = nullptr;
	/*使用するアダプタ用の変数*/
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	/*Device*/
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	/*コマンドキュー*/
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	/*コマンドアロケータ*/
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	/*コマンドリスト*/
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	/*スワップチェーン*/
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	/*DepthStencilResource*/
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	/*ディスクリプタヒープ*/
	/*DSV用*/
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	/*RTV用*/
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	/*SRV用*/
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	/*スワップチェーンリソース*/
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };

	/*フェンス*/
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	/*RTVを2つ作るのでディスクリプタを2つ用意*/
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2] = { 0 };

	/*DSVのディスクリプタ*/
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_;

	/*バリア*/
	D3D12_RESOURCE_BARRIER barrier_{};

	/*バックバッファインデックス*/
	UINT backBufferIndex_ = 0;

	/*フェンスバリュー*/
	UINT64 fenceValue_ = 0;

	/*DescriptorSize取得*/
	uint32_t descriptorSizeSRV_ = 0u;
	uint32_t descriptorSizeRTV_ = 0u;
	uint32_t descriptorSizeDSV_ = 0u;

};