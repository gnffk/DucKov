#include "Target_Manager.h"
#include "GameInstance.h"
#include "RenderTarget.h"

Target_Manager::Target_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{

}

Target_Manager::~Target_Manager()
{
}

HRESULT Target_Manager::Add_RenderTarget(const _wstring& strTargetTag, uint32_t iWidth, uint32_t iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strTargetTag))
        return E_FAIL;

    auto        pRenderTarget = RenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT Target_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    auto    pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    auto    pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
    {
        list<shared_ptr<RenderTarget>>     MRTList;

        MRTList.push_back(pRenderTarget);

        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        (*pMRTList).push_back(pRenderTarget);


    return S_OK;
}

HRESULT Target_Manager::Begin_MRT(const _wstring& strMRTTag, _bool bUseDepth)
{
    auto pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    ID3D11ShaderResourceView* pNullSRVs[16] = {};
    m_pContext->PSSetShaderResources(0, 16, pNullSRVs);

    m_pBackBufferRTV.Reset();
    m_pOriginalDSV.Reset();

    m_pContext->OMGetRenderTargets(
        1,
        m_pBackBufferRTV.GetAddressOf(),
        m_pOriginalDSV.GetAddressOf()
    );

    // 현재 Viewport 저장
    m_iOriginalViewportCount = 1;
    m_pContext->RSGetViewports(
        &m_iOriginalViewportCount,
        &m_OriginalViewport
    );

    ID3D11RenderTargetView* pRTVs[8] = {};
    uint32_t iNumRenderTargets = 0;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRTVs[iNumRenderTargets++] = pRenderTarget->Get_RTV().Get();
    }

    ID3D11DepthStencilView* pDSV = nullptr;

    if (bUseDepth)
        pDSV = m_pOriginalDSV.Get();

    m_pContext->OMSetRenderTargets(iNumRenderTargets,pRTVs, pDSV);

    // 현재 MRT의 첫 번째 RenderTarget 크기로 Viewport 설정
    auto pFirstTarget = pMRTList->front();

    D3D11_VIEWPORT Viewport{};
    Viewport.TopLeftX = 0.f;
    Viewport.TopLeftY = 0.f;
    Viewport.Width = static_cast<float>(pFirstTarget->Get_Width());
    Viewport.Height = static_cast<float>(pFirstTarget->Get_Height());
    Viewport.MinDepth = 0.f;
    Viewport.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &Viewport);

    return S_OK;
}
HRESULT Target_Manager::End_MRT()
{
    ID3D11RenderTargetView* pBackBufferRTV = m_pBackBufferRTV.Get();

    m_pContext->OMSetRenderTargets(1,&pBackBufferRTV,m_pOriginalDSV.Get() );

    // Begin_MRT 전에 저장했던 Viewport로 복구
    m_pContext->RSSetViewports(m_iOriginalViewportCount, &m_OriginalViewport );

    return S_OK;
}
HRESULT Target_Manager::Bind_ShaderResource(const _wstring& strTargetTag, shared_ptr<class Shader> pShader, const _char* pConstantName)
{
    auto    pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT Target_Manager::Copy_RenderTarget(const _wstring& strTargetTag, ComPtr<ID3D11Texture2D> pOut)
{
    auto    pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Copy_RenderTarget(pOut);
}

#ifdef _DEBUG
HRESULT Target_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    auto    pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT Target_Manager::Debug_Render(const _wstring& strMRTTag, shared_ptr<class Shader> pShader, const _char* pConstantName, shared_ptr<class VIBuffer_Rect> pVIBuffer)
{
    auto    pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        if (nullptr != pRenderTarget)
            pRenderTarget->Debug_Render(pShader, pConstantName, pVIBuffer);
    }

    return S_OK;
}
#endif

shared_ptr<RenderTarget> Target_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
    auto    iter = m_RenderTargets.find(strTargetTag);

    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<shared_ptr<class RenderTarget>>* Target_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto    iter = m_MRTs.find(strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

unique_ptr<Target_Manager> Target_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return unique_ptr<Target_Manager>(new Target_Manager(pDevice, pContext));
}
