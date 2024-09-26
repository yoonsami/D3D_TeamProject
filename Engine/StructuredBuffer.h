#pragma once
class StructuredBuffer
{
public:
	StructuredBuffer(void* inputData, _uint inputStride, _uint inputCount, _uint outputStride = 0, _uint outputCount = 0);
	~StructuredBuffer();

public:
	ComPtr<ID3D11ShaderResourceView> Get_SRV() { return m_pSRV; }
	ComPtr<ID3D11UnorderedAccessView> Get_UAV() { return m_pUAV; }
	_uint Get_InputByteWidth() { return m_iInputStride * m_iInputCount; }
	_uint Get_OutputByteWidth() { return m_iOutputStride * m_iOutputCount; }


public:
	void Create_Buffer();
	void Copy_ToInput(void* data);
	void Copy_FromOutput(void* data);
	_uint Get_InputCount() { return m_iInputCount; }

private:
	void Create_Input();
	void Create_SRV();
	void Create_Output();
	void Create_UAV();
	void Create_Result();

private:
	ComPtr<ID3D11Buffer> m_pInput;
	ComPtr<ID3D11ShaderResourceView> m_pSRV;
	ComPtr<ID3D11Buffer> m_pOutput;
	ComPtr<ID3D11UnorderedAccessView> m_pUAV;
	ComPtr<ID3D11Buffer> m_pResult;

private:
	void* m_pInputData = nullptr;
	_uint m_iInputStride = 0;
	_uint m_iInputCount = 0;
	_uint m_iOutputStride = 0;
	_uint m_iOutputCount = 0;
};

