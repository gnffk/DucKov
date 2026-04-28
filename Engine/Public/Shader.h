#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Shader final : public Component
{
private:
	Shader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~Shader();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(uint32_t iPassIndex);
	HRESULT Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, uint32_t iNumMatrices);

private:
	ComPtr<ID3DX11Effect>			m_pEffect = { nullptr };

	uint32_t							m_iNumPasses = {};
	vector<ComPtr<ID3D11InputLayout>>	m_InputLayouts;


public:
	static unique_ptr<Shader> Create(ComPtr<ID3D11Device>	pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements);
	virtual shared_ptr<Prototype> Clone(void* pArg) override;

};

NS_END