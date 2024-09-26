#include "pch.h"
#include "RenderTargetGroup.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> DepthStencilTexture)
{
	m_eGroupType = groupType;
	m_RenderTargets = rtVec;
	m_iRenderTargetCount = static_cast<_uint>(rtVec.size());
	m_pDepthStencilTexture = DepthStencilTexture;
}

void RenderTargetGroup::OMSetRenderTarget(_uint count, _uint index)
{
	vector<ID3D11RenderTargetView*> targets;
	for (_uint i = index; i < index + count; ++i)
	{
		targets.push_back(m_RenderTargets[i].target->Get_RTV().Get());
	}
	CONTEXT->OMSetRenderTargets(count, targets.data(), m_pDepthStencilTexture->Get_DST().Get());

	D3D11_TEXTURE2D_DESC desc;
	m_pDepthStencilTexture->Get_Texture2D().Get()->GetDesc(&desc);

	GRAPHICS.Get_ViewPort().Set(_float(desc.Width), _float(desc.Height));
	GRAPHICS.Get_ViewPort().RSSetViewport();
}

void RenderTargetGroup::OMSetRenderTargets()
{
	D3D11_VIEWPORT VP = D3D11_VIEWPORT{ 0,0,m_pDepthStencilTexture->Get_Size().x,m_pDepthStencilTexture->Get_Size().y,0.f,1.f };
	D3D11_RECT rect = D3D11_RECT{ 0,0,static_cast<LONG>(m_pDepthStencilTexture->Get_Size().x),static_cast<LONG>(m_pDepthStencilTexture->Get_Size().y) };
	CONTEXT->RSSetViewports(1, &VP);
	CONTEXT->RSSetScissorRects(1, &rect);

	vector<ID3D11RenderTargetView*> targets;
	for (_uint i = 0; i < m_iRenderTargetCount; ++i)
	{
		if (m_RenderTargets[i].target)
			targets.push_back(m_RenderTargets[i].target->Get_RTV().Get());
		else
			targets.push_back(nullptr);
	}
	CONTEXT->OMSetRenderTargets(m_iRenderTargetCount, targets.data(), m_pDepthStencilTexture->Get_DST().Get());
}

void RenderTargetGroup::ClearRenderTargetView(_uint index)
{
	CONTEXT->ClearRenderTargetView(m_RenderTargets[index].target->Get_RTV().Get(), (_float*)&m_RenderTargets[index].clearColor);

	CONTEXT->ClearDepthStencilView(m_pDepthStencilTexture->Get_DST().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	for (_uint i = 0; i < m_iRenderTargetCount; ++i)
	{
		if(m_RenderTargets[i].target)
		CONTEXT->ClearRenderTargetView(m_RenderTargets[i].target->Get_RTV().Get(), (_float*)&m_RenderTargets[i].clearColor);
	}

	CONTEXT->ClearDepthStencilView(m_pDepthStencilTexture->Get_DST().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void RenderTargetGroup::UnBindSRV()
{
	vector<ID3D11RenderTargetView*> targets;
	for (_uint i = 0; i < m_iRenderTargetCount; ++i)
	{
		ID3D11RenderTargetView* tmp = nullptr;
		targets.push_back(tmp);
	}
	CONTEXT->OMSetRenderTargets(m_iRenderTargetCount, targets.data(), nullptr);
}

