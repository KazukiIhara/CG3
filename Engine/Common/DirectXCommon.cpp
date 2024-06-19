#include "DirectXCommon.h"
#include "Log.h"
#include "PipelineStateObject.h"

D3DResourceLeakChecker* D3DResourceLeakChecker::GetInstance()
{
	static D3DResourceLeakChecker instance;
	return &instance;
}

D3DResourceLeakChecker::~D3DResourceLeakChecker()
{
	//リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		cLog::Log("Leak Check Completed\nBye...\n");
	}
}

cDirectXCommon* cDirectXCommon::GetInstance()
{
	static cDirectXCommon instance;
	return &instance;
}

void cDirectXCommon::Initialize(int32_t backBufferWidth, int32_t backBufferHeight, bool enableDebugLayer)
{
	/*DXGIデバイスの初期化*/
	InitializeDXGIDevice(enableDebugLayer);

	/*コマンド関連の初期化*/
	InitializeCommand();

	/*スワップチェーンの生成*/
	CreateSwapChain();

	/*レンダーターゲットの生成*/
	CreateRenderTargetView();

	/*深度バッファの生成*/
	CreateDepthStencilView();

	/*フェンスの生成*/
	CreateFence();

}

void cDirectXCommon::PreDraw(float clearColor[])
{
	/*これから書き込むバックバッファインデックスを取得*/
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	/*TransitionBarrierの設定*/
	/*今回のバリアはTransition*/
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	/*Noneにしておく*/
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	/*バリアを張る対象のリソース。現在のバックバッファに対して行う*/
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex_].Get();
	/*遷移前のResourceState*/
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	/*遷移後のResourceState*/
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	/*TransitionBarrierを張る*/
	commandList_->ResourceBarrier(1, &barrier_);

	/*レンダーターゲット設定*/
	SetRenderTargets();

	/*深度クリア*/
	ClearDepthView();

	/*全画面クリア*/
	ClearRenderTarget(clearColor);

	/*ビューポートの設定*/
	D3D12_VIEWPORT viewport{};
	/*クライアント領域のサイズと一緒にして画面全体に表示*/
	viewport.Width = cWinApp::kClientWidth;
	viewport.Height = cWinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	/*シザー矩形の設定*/
	D3D12_RECT scissorRect{};
	/*基本的にビューポートと同じ矩形が構成されるようにする*/
	scissorRect.left = 0;
	scissorRect.right = cWinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = cWinApp::kClientHeight;


	cDirectXCommon::GetCommandList()->RSSetViewports(1, &viewport);//ViewPort設定
	cDirectXCommon::GetCommandList()->RSSetScissorRects(1, &scissorRect);//Scirssorを設定
	//RootSIgnatureを設定。PSOに設定しているけど別途設定が必要
	cDirectXCommon::GetCommandList()->SetGraphicsRootSignature(cPipelineStateObject::GetRootSignature());
	cDirectXCommon::GetCommandList()->SetPipelineState(cPipelineStateObject::GetPipelineState());//PSOを設定
}

void cDirectXCommon::PostDraw()
{
	/*画面に書く処理はすべて終わり、画面に映すので状態を遷移*/
	/*今回はRenderTargetからPresentにする*/
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	/*TransitionBarrierを張る*/
	commandList_->ResourceBarrier(1, &barrier_);

	/*コマンドのクローズ*/
	commandList_->Close();

	/*GPUにコマンドリストの実行を行わせる*/
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	/*GPUとOSに画面の交換を行うよう通知する*/
	swapChain_->Present(1, 0);


	/*Fenceの値を更新し、GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る*/
	commandQueue_->Signal(fence_.Get(), ++fenceValue_);
	/*Fenceの値が指定したSingnal値にたどり着いているか確認する*/
	/*GetCompletedValueの初期値はFence作成時に渡した初期値*/
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		/*FenceのSignalを待つためのイベントを作成する*/
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		/*指定したSignalにたどり着いていないので、たどりつくまでに待つようにイベントを設定する*/
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		/*イベントを待つ*/
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	/*次のフレーム用のコマンドリストを準備*/
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

void cDirectXCommon::ClearDepthView()
{
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void cDirectXCommon::ClearRenderTarget(float clearColor[])
{
	/*これから書き込むバックバッファインデックスを取得*/
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	/*指定した色で画面全体をクリアする*/
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex_], clearColor, 0, nullptr);
}

void cDirectXCommon::SetRenderTargets()
{
	/*これから書き込むバックバッファインデックスを取得*/
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	/*描画先のRTVとDSVをを設定する*/
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex_], false, &dsvHandle_);
}

D3D12_CPU_DESCRIPTOR_HANDLE cDirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE cDirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void cDirectXCommon::InitializeDXGIDevice([[maybe_unused]] bool enableDebugLayer)
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		/*デバッグコントローラを有効化する*/
		debugController->EnableDebugLayer();
		/*さらにGPU側でもチェックを行うようにする*/
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	/*DXGIファクトリーの生成*/
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	/*初期化の根本的な部分なので、うまくいかないならassert*/
	assert(SUCCEEDED(hr_));

	/*使用するアダプタ(GPU)を決定する*/
	/*良い順にアダプタを積む*/
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		/*アダプターの情報を取得する*/
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		/*取得できぬのならとまれ*/
		assert(SUCCEEDED(hr_));
		/*ソフトウェアアダプタでなければ採用*/
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			/*採用したアダプタの情報をログに出力。*/
			cLog::Log(cLog::ConvertString(std::format(L"Use Adapter : {}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr; /*ソフトウェアアダプタの場合は見なかったことにする*/
	}
	/*適切なアダプタが見つからなかったので起動できない*/
	assert(useAdapter_ != nullptr);

	/*機能レベルとログ出力用の文字列*/
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	/*高い順に生成できるか試していく*/
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		/*採用したアダプターでデバイスを生成*/
		hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		/*指定した機能レベルでデバイスが生成出来たかを確認*/
		if (SUCCEEDED(hr_))
		{
			/*生成できたのでログ出力を行ってループを抜ける*/
			cLog::Log(std::format("featureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	/*デバイスの生成がうまくいかなかったので起動できない*/
	assert(device_ != nullptr);
	/*初期化完了ログ*/
	cLog::Log("Complete Create D3D12DDevice!\n omdetou/(^ v ^)/\n");

	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);



#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		/*やばいエラー時に止まる*/
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		/*エラー時に止まる*/
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		/*警告時に止まる*/
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		/*抑制するメッセージのID*/
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGiデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		/*抑制するレベル*/
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		/*指定したメッセージの表示を抑制する*/
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG

}

void cDirectXCommon::InitializeCommand()
{
	/*コマンドキューを生成する*/
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	/*コマンドキューの生成がうまくいかなかったらassert*/
	assert(SUCCEEDED(hr_));

	/*コマンドアロケータを生成する*/
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	/*コマンドアロケータの生成がうまくいかなかったので起動できない*/
	assert(SUCCEEDED(hr_));

	/*コマンドリストを生成する*/
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	/*コマンドリストの生成がうまくいかなかったので起動できない*/
	assert(SUCCEEDED(hr_));

}

void cDirectXCommon::CreateSwapChain()
{
	swapChainDesc_.Width = cWinApp::kClientWidth;				/*画面の幅、ウィンドウのクライアント領域を同じものにしておく*/
	swapChainDesc_.Height = cWinApp::kClientHeight;				/*画面の高さ、ウィンドウのクライアント領域を同じものしておく*/
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			/*色形式*/
	swapChainDesc_.SampleDesc.Count = 1;						/*マルチサンプルしない*/
	swapChainDesc_.BufferCount = 2;								/*ダブルバッファ*/
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	/*モニタにうつしたら、中身を破棄*/

	/*コマンドキュー、ウィンドウハンドル、設定を渡して生成する*/
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), cWinApp::GetHwnd(), &swapChainDesc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr_));
}

void cDirectXCommon::CreateRenderTargetView()
{
	/*ディスクリプタヒープの生成*/
	/*RTV用のヒープでディスクリプタの数は２。RTVはShader内で触るものではないので、ShaderVisibleはfalse*/
	rtvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはtrue
	srvDescriptorHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	/*スワップチェーンからリソースを引っ張ってくる*/
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	/*うまく取得出来なければ起動できない*/
	assert(SUCCEEDED(hr_));
	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));

	/*RTVの設定*/
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	/*出力結果をSRGBに変換して書き込む*/
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	/*2Dテクスチャとして書き込む*/
	/*ディスクリプタの先頭を取得する*/
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	/*まず1つを作る*/
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	/*2つ目のディスクリプタハンドルを得る*/
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	/*2つ目を作る*/
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

void cDirectXCommon::CreateDepthStencilView()
{
	/*DepthStencilTextureをウィンドウのサイズで作成*/
	depthStencilResource_ = CreateDepthStencilTextureResource(device_.Get(), cWinApp::kClientWidth, cWinApp::kClientHeight);

	/*DSV用のヒープでディスクリプタの数は1*/
	dsvDescriptorHeap_ = CreateDescriptorHeap(cDirectXCommon::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	/*DSVの設定*/
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	/*DSVHeapの先端にDSVをつくる*/
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void cDirectXCommon::CreateFence()
{
	/*初期値0でFenceを作る*/
	fenceValue_ = 0;
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));
}

Microsoft::WRL::ComPtr<ID3D12Resource> cDirectXCommon::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	/*生成するResourceの設定*/
	static D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*利用するHeapの設定*/
	static D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	/*深度値のクリア設定*/
	static D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc,	//Resourcの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度地を書き込む状態にしておく
		&depthClearValue,//Clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}


Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cDirectXCommon::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;	//レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors;						//ダブルバッファ用に2つ、多くても別に構わない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプターヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}
