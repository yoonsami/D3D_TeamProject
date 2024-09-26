#include "pch.h"
#include "Graphics.h"

#define FORMATTYPE DXGI_FORMAT_R16G16B16A16_FLOAT

HRESULT Graphics::Initialize(GRAPHICDESC desc)
{
	if (FAILED(Ready_DeviceAndSwapChain(desc.hWnd, desc.eWinMode, desc.iSizeX, desc.iSizeY)))
	{
		MSG_BOX("Failed to Ready_DeviceAndSwapChain : Graphics ");
		return E_FAIL;
	}

	Set_ViewPort(desc.iSizeX, desc.iSizeY);
	
	Create_RTGroup();

	return S_OK;
}

HRESULT Graphics::Render_Begin()
{
	if (!m_pContext)
		return E_FAIL;

	Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets();

	for (_uchar i = 0; i < _uchar(RENDER_TARGET_GROUP_TYPE::END); ++i)
	{
		Get_RTGroup(RENDER_TARGET_GROUP_TYPE(i))->ClearRenderTargetView();
	}

	m_Viewport.RSSetViewport();

	return S_OK;
}

HRESULT Graphics::Render_End()
{
	if (!m_pSwapChain)
		return E_FAIL;

	HRESULT hr = m_pSwapChain->Present(1, 0);
	m_pContext->ClearState();

	return hr;
}

HRESULT Graphics::Ready_DeviceAndSwapChain(HWND hWnd, GRAPHICDESC::WINMODE eWinmode, _uint iWinCX, _uint iWinCY)
{
	DXGI_SWAP_CHAIN_DESC    SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.BufferDesc.Width = iWinCX;
	SwapChainDesc.BufferDesc.Height = iWinCY;
	/*스왑하는 형태*/
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	SwapChainDesc.BufferDesc.Format = FORMATTYPE;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.Windowed = eWinmode;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION, &SwapChainDesc, m_pSwapChain.GetAddressOf(), m_pDevice.GetAddressOf(), nullptr, m_pContext.GetAddressOf())))
	{
		MSG_BOX("Failed to D3D11CreateDeviceAndSwapChain");
		return E_FAIL;
	}

	return S_OK;
}

void Graphics::Set_ViewPort(_uint iWinCX, _uint iWinCY)
{
	m_Viewport.Set(_float(iWinCX), _float(iWinCY));
}

void Graphics::Create_RTGroup()
{
	//
	//shared_ptr<Texture> dsTexture = RESOURCES.CreateTexture(L"DepthStencil", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));
	shared_ptr<Texture> dsTexture = RESOURCES.CreateShadowTexture(L"DepthStencil", (m_Viewport.Get_Width()), (m_Viewport.Get_Height()));

	//SwapChain
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);
		for (_uint i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + to_wstring(i);

			ComPtr<ID3D11Texture2D> resource;
			m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&resource));
			rtVec[i].target = RESOURCES.CreateTextureFromResource(name, resource);
			rtVec[i].clearColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
		}
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
	}

	//Shadow
	//{
	//	vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

	//	rtVec[0].target = RESOURCES.CreateTexture(L"ShadowTarget",
	//		DXGI_FORMAT_R32_FLOAT, _int(SHADOWMAP_SIZE), _int(SHADOWMAP_SIZE),
	//		D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET
	//		| D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
	//	rtVec[0].clearColor = _float4(1.f);

	//	//rtVec[0].target = nullptr;

	//	/*shared_ptr<Texture> shadowDepthTexture
	//		= RESOURCES.CreateTexture(L"ShadowDepthStencil",
	//			DXGI_FORMAT_D32_FLOAT, 8192, 8192,
	//			D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(1.f));
	//	rtVec[0].clearColor = _float4(0.f);*/
	//	shared_ptr<Texture> shadowDepthTexture = RESOURCES.CreateShadowTexture(L"ShadowDepthStencil", SHADOWMAP_SIZE, SHADOWMAP_SIZE);


	//	m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
	//	m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
	//}

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		//shared_ptr<Texture> dsTexture0 = RESOURCES.CreateShadowTexture(L"ShadowTarget", SHADOWMAP_SIZE, SHADOWMAP_SIZE);
		rtVec[0].target = nullptr;
		//rtVec[0].target = nullptr;

		rtVec[0].target = RESOURCES.CreateTexture(L"ShadowTarget",
					DXGI_FORMAT_R32_FLOAT, _int(SHADOWMAP_SIZE), _int(SHADOWMAP_SIZE),
					D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET
					| D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));		rtVec[0].clearColor = _float4(0.f);
		shared_ptr<Texture> shadowDepthTexture = RESOURCES.CreateShadowTexture(L"ShadowDepthStencil", SHADOWMAP_SIZE, SHADOWMAP_SIZE);


		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
	}

	//MotionBlur
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_MOTIONBLUR_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"VelocityMapDS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"VelocityMap", DXGI_FORMAT_R32G32B32A32_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f,0.f,0.f,1.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::MOTIONBLUR)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::MOTIONBLUR)]->Create(RENDER_TARGET_GROUP_TYPE::MOTIONBLUR, rtVec, dsTexture0);
	}


	//Deferred
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"PositionTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		rtVec[1].target = RESOURCES.CreateTexture(L"NormalTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[1].clearColor = _float4(0.f);

		rtVec[2].target = RESOURCES.CreateTexture(L"PositionForSSD", DXGI_FORMAT_R32G32B32A32_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[2].clearColor = _float4(0.f);

		rtVec[3].target = RESOURCES.CreateTexture(L"DiffuseTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[3].clearColor = _float4(0.f);

		rtVec[4].target = RESOURCES.CreateTexture(L"SpecularTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[4].clearColor = _float4(0.f);

		rtVec[5].target = RESOURCES.CreateTexture(L"RimTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[5].clearColor = _float4(0.f);

		rtVec[6].target = RESOURCES.CreateTexture(L"BlurTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[6].clearColor = _float4(0.f);

		rtVec[7].target = RESOURCES.CreateTexture(L"EmissiveTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[7].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}

	//BLURSMALLER0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLUR_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLURSMALLER0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLURSMALLER0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURSMALLER0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURSMALLER0)]->Create(RENDER_TARGET_GROUP_TYPE::BLURSMALLER0, rtVec, dsTexture0);
	}

	//BLURSMALLER1
	{
		_float ratio = 0.25f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLUR_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture1 = RESOURCES.CreateTexture(L"BLURSMALLER1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLURSMALLER1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURSMALLER1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURSMALLER1)]->Create(RENDER_TARGET_GROUP_TYPE::BLURSMALLER1, rtVec, dsTexture1);
	}

	//BLURBIGGER0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLUR_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLURBIGGER0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLURBIGGER0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURBIGGER0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURBIGGER0)]->Create(RENDER_TARGET_GROUP_TYPE::BLURBIGGER0, rtVec, dsTexture0);
	}

	//BLURBIGGER1
	{

		vector<RenderTarget> rtVec(RENDER_TARGET_BLUR_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture1 = RESOURCES.CreateTexture(L"BLURBIGGER1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLURBIGGER1", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURBIGGER1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLURBIGGER1)]->Create(RENDER_TARGET_GROUP_TYPE::BLURBIGGER1, rtVec, dsTexture1);
	}

	//Light
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"AmbientLightTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);
		rtVec[1].target = RESOURCES.CreateTexture(L"DiffuseLightTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[1].clearColor = _float4(0.f);
		rtVec[2].target = RESOURCES.CreateTexture(L"SpecularLightTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[2].clearColor = _float4(0.f);
		rtVec[3].target = RESOURCES.CreateTexture(L"EmissiveLightTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[3].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
	}

	//Final
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_FINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"FinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FINAL)]->Create(RENDER_TARGET_GROUP_TYPE::FINAL, rtVec, dsTexture);
	}

	//OutLine
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_OUTLINE_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"OutLineTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::OUTLINE)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::OUTLINE)]->Create(RENDER_TARGET_GROUP_TYPE::OUTLINE, rtVec, dsTexture);
	}
	//OutLineFinal
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_OUTLINEFINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"OutLineFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::OUTLINEFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::OUTLINEFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::OUTLINEFINAL, rtVec, dsTexture);
	}
	//SSAO
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"SSAOTarget", DXGI_FORMAT_R16_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAO)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAO)]->Create(RENDER_TARGET_GROUP_TYPE::SSAO, rtVec, dsTexture);
	}

	//SSAODOWNSCALE0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAODOWNSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAODOWNSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE0, rtVec, dsTexture0);
	}

	//SSAODOWNSCALE1
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAODOWNSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAODOWNSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE1, rtVec, dsTexture0);
	}

	//SSAODOWNSCALE2
	{
		_float ratio = 0.5f * 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAODOWNSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAODOWNSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::SSAODOWNSCALE2, rtVec, dsTexture0);
	}

	//SSAOUPSCALE0
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAOUPSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAOUPSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE0, rtVec, dsTexture0);
	}

	//SSAOUPSCALE1
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAOUPSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAOUPSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE1, rtVec, dsTexture0);
	}
	//SSAOUPSCALE2
	{

		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"SSAOUPSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"SSAOUPSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::SSAOUPSCALE2, rtVec, dsTexture0);
	}

	//MotionBlurFinal
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_MOTIONBLURFINAL_GROUP_MEMBER_COUNT);

		rtVec[0].target = RESOURCES.CreateTexture(L"MotionBlurFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::MOTIONBLURFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::MOTIONBLURFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::MOTIONBLURFINAL, rtVec, dsTexture);
	}

	//BLOOMMAP
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMMAP_GROUP_MEMBER_COUNT);

		rtVec[0].target = RESOURCES.CreateTexture(L"BloomTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMMAP)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMMAP)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMMAP, rtVec, dsTexture);
	}

	//BLOOMDOWNSCALE0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE0, rtVec, dsTexture0);
	}

	//BLOOMDOWNSCALE1
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE1, rtVec, dsTexture0);
	}

	//BLOOMDOWNSCALE2
	{
		_float ratio = 0.5f * 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMDOWNSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE2, rtVec, dsTexture0);
	}

	//BLOOMUPSCALE0
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMUPSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMUPSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE0, rtVec, dsTexture0);
	}

	//BLOOMUPSCALE1
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMUPSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMUPSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE1, rtVec, dsTexture0);
	}
	//BLOOMUPSCALE2
	{

		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"BLOOMUPSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"BLOOMUPSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE2, rtVec, dsTexture0);
	}

	//BLOOMFINAL
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMFINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"BloomFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::BLOOMFINAL, rtVec, dsTexture);
	}

	//DOFMAP
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFMAP)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFMAP)]->Create(RENDER_TARGET_GROUP_TYPE::DOFMAP, rtVec, dsTexture);
	}

	//DOFDOWNSCALE0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFDOWNSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFDOWNSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE0, rtVec, dsTexture0);
	}

	//DOFDOWNSCALE1
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFDOWNSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFDOWNSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE1, rtVec, dsTexture0);
	}

	//DOFDOWNSCALE2
	{
		_float ratio = 0.5f * 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFDOWNSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFDOWNSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE2, rtVec, dsTexture0);
	}

	//DOFUPSCALE0
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFUPSCALE0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFUPSCALE0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE0)]->Create(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE0, rtVec, dsTexture0);
	}

	//DOFUPSCALE1
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFUPSCALE1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFUPSCALE1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE1)]->Create(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE1, rtVec, dsTexture0);
	}
	//DOFUPSCALE2
	{

		vector<RenderTarget> rtVec(RENDER_TARGET_DOFSCALE_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOFUPSCALE2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DOFUPSCALE2", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE2)]->Create(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE2, rtVec, dsTexture0);
	}

	//DOFFINAL
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_DOFFINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"DOFFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::DOFFINAL, rtVec, dsTexture);
	}

	//Distortion
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_DISTORTION_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"DistortionTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);
		rtVec[1].target = RESOURCES.CreateTexture(L"DistortionTarget1", DXGI_FORMAT_R32G32B32A32_FLOAT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[1].clearColor = _float4(0.f);
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DISTORTION)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DISTORTION)]->Create(RENDER_TARGET_GROUP_TYPE::DISTORTION, rtVec, dsTexture);
	}

	//DistortionFinal
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_DISTORTIONFINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"DistortionFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DISTORTIONFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DISTORTIONFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::DISTORTIONFINAL, rtVec, dsTexture);
	}

	//FXAA
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_FXAA_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"FXAATarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FXAA)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FXAA)]->Create(RENDER_TARGET_GROUP_TYPE::FXAA, rtVec, dsTexture);
	}
	//Lens Flare
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LENSFLARE_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"LensFlareTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLARE)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLARE)]->Create(RENDER_TARGET_GROUP_TYPE::LENSFLARE, rtVec, dsTexture);
	}
	
	//LENSFLAREFINAL
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LENSFLAREFINAL_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"LensFlareFinalTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLAREFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLAREFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::LENSFLAREFINAL, rtVec, dsTexture);
	}
	//Aberration
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_ABERAATION_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"AberrationTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::ABERRATION)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::ABERRATION)]->Create(RENDER_TARGET_GROUP_TYPE::ABERRATION, rtVec, dsTexture);
	}
	//RadialBlur
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_ABERAATION_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"RadialBlurTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::RADIALBLUR)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::RADIALBLUR)]->Create(RENDER_TARGET_GROUP_TYPE::RADIALBLUR, rtVec, dsTexture);
	}
	//Vignette
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_VIGNETTE_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"VignetteTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::VIGNETTE)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::VIGNETTE)]->Create(RENDER_TARGET_GROUP_TYPE::VIGNETTE, rtVec, dsTexture);
	}
	//FOG
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_FOG_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"FogTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FOG)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::FOG)]->Create(RENDER_TARGET_GROUP_TYPE::FOG, rtVec, dsTexture);
	}
	//ToneMapping
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_TONEMAPPING_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"ToneMappingTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::TONEMAPPING)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::TONEMAPPING)]->Create(RENDER_TARGET_GROUP_TYPE::TONEMAPPING, rtVec, dsTexture);
	}  

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_TONEMAPPING_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"SUBSCENEFINALTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SUBSCENEFINAL)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::SUBSCENEFINAL)]->Create(RENDER_TARGET_GROUP_TYPE::SUBSCENEFINAL, rtVec, dsTexture);
	}


	//DOWNSAMPLER0
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOWNSAMPLER0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DownSample0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER0)]->Create(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER0, rtVec, dsTexture0);
	}

	//DOWNSAMPLER1
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOWNSAMPLER1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DownSample1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER1)]->Create(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER1, rtVec, dsTexture0);
	}

	//DOWNSAMPLER2
	{
		_float ratio = 0.5f * 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOWNSAMPLER2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DownSample2", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER2)]->Create(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER2, rtVec, dsTexture0);
	}

	//DOWNSAMPLER3
	{
		_float ratio = 0.5f * 0.5f * 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"DOWNSAMPLER3DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"DownSample3", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER3)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER3)]->Create(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER3, rtVec, dsTexture0);
	}
	//UPSAMPLER0
	{
		_float ratio = 1.f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"UPSAMPLER0DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"UpSample0", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER0)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER0)]->Create(RENDER_TARGET_GROUP_TYPE::UPSAMPLER0, rtVec, dsTexture0);
	}
	//UPSAMPLER1
	{
		_float ratio = 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"UPSAMPLER1DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"UpSample1", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER1)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER1)]->Create(RENDER_TARGET_GROUP_TYPE::UPSAMPLER1, rtVec, dsTexture0);
	}
	//UPSAMPLER2
	{
		_float ratio = 0.5f * 0.5f;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"UPSAMPLER2DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"UpSample2", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER2)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER2)]->Create(RENDER_TARGET_GROUP_TYPE::UPSAMPLER2, rtVec, dsTexture0);
	}
	//UPSAMPLER3
	{
		_float ratio = 0.5f * 0.5f * 0.5F;
		vector<RenderTarget> rtVec(RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"UPSAMPLER3DS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"UpSample3", FORMATTYPE, _uint(m_Viewport.Get_Width() * ratio), _uint(m_Viewport.Get_Height() * ratio), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER3)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER3)]->Create(RENDER_TARGET_GROUP_TYPE::UPSAMPLER3, rtVec, dsTexture0);
	}

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_BLOOMMAP_GROUP_MEMBER_COUNT);
		shared_ptr<Texture> dsTexture0 = RESOURCES.CreateTexture(L"AFTER_UIDS", DXGI_FORMAT_D24_UNORM_S8_UINT, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL, _float4(0.f));

		rtVec[0].target = RESOURCES.CreateTexture(L"AFTER_UITarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::AFTER_UI)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::AFTER_UI)]->Create(RENDER_TARGET_GROUP_TYPE::AFTER_UI, rtVec, dsTexture0);
	}
	//ToneMapping
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_TONEMAPPING_GROUP_MEMBER_COUNT);
		rtVec[0].target = RESOURCES.CreateTexture(L"AFTER_UI_ToneMappingTarget", FORMATTYPE, _uint(m_Viewport.Get_Width()), _uint(m_Viewport.Get_Height()), D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, _float4(0.f));
		rtVec[0].clearColor = _float4(0.f);

		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::AFTER_UI_TONEMAPPING)] = make_shared<RenderTargetGroup>();
		m_RTGroup[static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::AFTER_UI_TONEMAPPING)]->Create(RENDER_TARGET_GROUP_TYPE::AFTER_UI_TONEMAPPING, rtVec, dsTexture);
	}




}
