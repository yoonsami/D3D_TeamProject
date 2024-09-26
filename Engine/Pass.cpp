#include "pch.h"
#include "Pass.h"

void Pass::Draw(UINT vertexCount, UINT startVertexLocation)
{
	BeginDraw();
	{
		CONTEXT->Draw(vertexCount, startVertexLocation);
	}
	EndDraw();
}

void Pass::DrawIndexed(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
{
	BeginDraw();
	{
			CONTEXT->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}
	EndDraw();
}

void Pass::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		CONTEXT->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}
	EndDraw();
}

void Pass::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
{
	BeginDraw();
	{
		CONTEXT->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startIndexLocation);
	}
	EndDraw();
}

void Pass::BeginDraw()
{
	pass->ComputeStateBlockMask(&stateblockMask);

	CONTEXT->IASetInputLayout(inputLayout.Get());
	pass->Apply(0, CONTEXT.Get());
}

void Pass::EndDraw()
{
	if (stateblockMask.RSRasterizerState == 1)
		CONTEXT->RSSetState(stateBlock->RSRasterizerState.Get());

	if (stateblockMask.OMDepthStencilState == 1)
		CONTEXT->OMSetDepthStencilState(stateBlock->OMDepthStencilState.Get(), stateBlock->OMStencilRef);

	if (stateblockMask.OMBlendState == 1)
		CONTEXT->OMSetBlendState(stateBlock->OMBlendState.Get(), stateBlock->OMBlendFactor, stateBlock->OMSampleMask);

	

	CONTEXT->HSSetShader(NULL, NULL, 0);
	CONTEXT->DSSetShader(NULL, NULL, 0);
	CONTEXT->GSSetShader(NULL, NULL, 0);
}

void Pass::Dispatch(UINT x, UINT y, UINT z)
{
	pass->Apply(0, CONTEXT.Get());
	CONTEXT->Dispatch(x, y, z);

	ID3D11ShaderResourceView* null[1] = { 0 };
	CONTEXT->CSSetShaderResources(0, 1, null);

	ID3D11UnorderedAccessView* nullUav[1] = { 0 };
	CONTEXT->CSSetUnorderedAccessViews(0, 1, nullUav, NULL);

	CONTEXT->CSSetShader(NULL, NULL, 0);
}