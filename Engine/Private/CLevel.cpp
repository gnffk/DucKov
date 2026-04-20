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

string  CLevel::WStringToString(const std::wstring& wstr) {
    if (wstr.empty())
        return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        nullptr, 0,
        nullptr, nullptr
    );

    std::string result(sizeNeeded - 1, 0);

    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        result.data(), sizeNeeded,
        nullptr, nullptr
    );

    return result;
}