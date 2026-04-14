#include "Shader.h"

Shader::Shader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Component{ pDevice, pContext }
{
}

Shader::~Shader()
{
}

HRESULT Shader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements)
{
    uint32_t        iFlag = {};

#ifdef _DEBUG
    iFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    ID3DBlob* pData = { nullptr };

    if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
        return E_FAIL;

    ComPtr<ID3DX11EffectTechnique>  pTechnique = m_pEffect->GetTechniqueByIndex(0);

    D3DX11_TECHNIQUE_DESC   TechniqueDesc{};

    pTechnique->GetDesc(&TechniqueDesc);

    m_iNumPasses = TechniqueDesc.Passes;

    for (uint32_t i = 0; i < m_iNumPasses; i++)
    {
        ComPtr<ID3D11InputLayout>       pInputLayout = { nullptr };

        D3DX11_PASS_DESC        PassDesc{};

        ComPtr<ID3DX11EffectPass>   pPass = pTechnique->GetPassByIndex(i);

        pPass->GetDesc(&PassDesc);

        PassDesc.pIAInputSignature;
        PassDesc.IAInputSignatureSize;

        if (FAILED(m_pDevice->CreateInputLayout(pInputElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
            return E_FAIL;

        m_InputLayouts.push_back(pInputLayout);
    }

    return S_OK;
}

HRESULT Shader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT Shader::Begin(uint32_t iPassIndex)
{
    if (iPassIndex >= m_iNumPasses)
        return E_FAIL;

    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex].Get());

    m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext.Get());

    return S_OK;
}

HRESULT Shader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

unique_ptr<Shader> Shader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pInputElements, uint32_t iNumElements)
{
    auto		pInstance = unique_ptr<Shader>(new Shader(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pInputElements, iNumElements)))
    {
        MSG_BOX("Failed to Created : Shader");
        return nullptr;
    }

    return pInstance;
}


shared_ptr<Prototype> Shader::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<Shader>(new Shader(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : Shader");
        return nullptr;
    }

    return pInstance;
}
