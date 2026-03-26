#include "CLevel.h"
CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pDeviceContext{ pContext }
{

}


HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}
