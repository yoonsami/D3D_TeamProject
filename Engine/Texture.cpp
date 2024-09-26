#include "pch.h"
#include "Texture.h"

Texture::Texture()
	:ResourceBase(ResourceType::Texture)
{
}

Texture::~Texture()
{
}

void Texture::Load(const wstring& path)
{
	m_strFilePath = path;
	TexMetadata md;
	_tchar		szExt[MAX_PATH] = TEXT("");

	_wsplitpath_s(path.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	if (false == lstrcmp(szExt, TEXT(".dds")) || false == lstrcmp(szExt, TEXT(".DDS")))
	{
		::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, &md, m_Img);
		::CreateShaderResourceView(DEVICE.Get(), m_Img.GetImages(), m_Img.GetImageCount(), md, m_pShaderResourceView.GetAddressOf());

	}
	else if (false == lstrcmp(szExt, TEXT(".tga")) || false == lstrcmp(szExt, TEXT(".TGA")))
	{
		::LoadFromTGAFile(path.c_str(),TGA_FLAGS_FORCE_SRGB, &md, m_Img);
		::CreateShaderResourceView(DEVICE.Get(), m_Img.GetImages(), m_Img.GetImageCount(), md, m_pShaderResourceView.GetAddressOf());

	}
	else // png, jpg, jpeg, bmp
	{
		::LoadFromWICFile(path.c_str(),WIC_FLAGS_FORCE_RGB, &md, m_Img);
		::CreateWICTextureFromFile(DEVICE.Get(), m_strFilePath.c_str(), nullptr, &m_pShaderResourceView);
	}


	m_Size.x = static_cast<_float>(md.width);
	m_Size.y = static_cast<_float>(md.height);
}

void Texture::Create(DXGI_FORMAT format, _uint width, _uint height, _uint BindFlags, _float4 clearColor)
{
	m_Size.x = _float(width);
	m_Size.y = _float(height);
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof texDesc);
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;

	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = BindFlags;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	(DEVICE->CreateTexture2D(&texDesc, nullptr, m_pTexture.GetAddressOf()));

	CreateFromResource(m_pTexture);
	
}

void Texture::CreateFromResource(ComPtr<ID3D11Texture2D> tex2D)
{
	m_pTexture = tex2D;
	


	tex2D.Get()->GetDesc(&m_TextureDesc);

	m_Size.x = _float(m_TextureDesc.Width);
	m_Size.y = _float(m_TextureDesc.Height);
	if (m_TextureDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof desc);
		desc.Format = m_TextureDesc.Format;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = (DEVICE->CreateDepthStencilView(m_pTexture.Get(), &desc, m_pDSV.GetAddressOf()));
		if (FAILED(hr))
			MSG_BOX("CreateDepthStencilView CreateFromResource FAIL");
		
	}
	else
	{
		if (m_TextureDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof desc);
			desc.Format = m_TextureDesc.Format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;
			

			(DEVICE->CreateRenderTargetView(m_pTexture.Get(), &desc, m_pRTV.GetAddressOf()));
			
		}

		if (m_TextureDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof uavDesc);
			uavDesc.Format = m_TextureDesc.Format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Buffer.NumElements = 1;

			(DEVICE->CreateUnorderedAccessView(m_pTexture.Get(), &uavDesc, m_pUAV.GetAddressOf()));

		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof srvDesc);
		srvDesc.Format = m_TextureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		HRESULT hr = (DEVICE->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pShaderResourceView.GetAddressOf()));

		int a = 0;
	}

}

void Texture::CreateShadowDST(_float _width, _float _height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width =_uint( _width);
	texDesc.Height = _uint(_height);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	(DEVICE->CreateTexture2D(&texDesc, 0, m_pTexture.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	(DEVICE->CreateDepthStencilView(m_pTexture.Get(), &dsvDesc, m_pDSV.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	(DEVICE->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pShaderResourceView.GetAddressOf()));

	m_Size = { _width,_height };
}

