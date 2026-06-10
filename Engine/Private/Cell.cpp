#include "Cell.h"

#include "GameInstance.h"

Cell::Cell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}

Cell::~Cell()
{

}

HRESULT Cell::Initialize(const _float3* pPoints, int32_t iIndex)
{
    m_iIndex = iIndex;
    memcpy(m_vPoints, pPoints, sizeof(_float3) * ETOUI(POINT_CELL::END));

    _vector     vLine = {};

    vLine = XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]) - XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]);
    m_vNormals[ETOUI(LINE_CELL::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)]) - XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]);
    m_vNormals[ETOUI(LINE_CELL::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    vLine = XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]) - XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)]);
    m_vNormals[ETOUI(LINE_CELL::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

    for (size_t i = 0; i < ETOUI(LINE_CELL::END); i++)
    {
        XMStoreFloat3(&m_vNormals[i],
            XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));
    }

    XMStoreFloat4(&m_vPlane, XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]), XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]), XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)])));

#ifdef _DEBUG
    m_pVIBuffer = VIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool Cell::isIn(_fvector vResultPos, int32_t* pNeighborIndex)
{
    for (size_t i = 0; i < ETOUI(LINE_CELL::END); i++)
    {
        _vector     vDir = XMVector3Normalize(vResultPos - XMLoadFloat3(&m_vPoints[i]));
        if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
        {
            *pNeighborIndex = m_iNeighborIndices[i];
            return false;
        }
    }

    return true;
}

_bool Cell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
    // XMVectorEqual();
    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)]), vDestPoint))
            return true;
    }

    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]), vDestPoint))
            return true;
    }

    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::C)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::A)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ETOUI(POINT_CELL::B)]), vDestPoint))
            return true;
    }

    return false;
}

_float Cell::Compute_Height(_fvector vPosition)
{
    /* ax + by + cz + d = 0 */
    return (-m_vPlane.x * XMVectorGetX(vPosition) - m_vPlane.z * XMVectorGetZ(vPosition) - m_vPlane.w) / m_vPlane.y;
}

void Cell::WriteFile(HANDLE hFile)
{
    DWORD       dwByte;

    ::WriteFile(hFile, m_iNeighborIndices, sizeof(uint32_t) * 3, &dwByte, nullptr);
}

_vector Cell::Get_BlockingNormal(_fvector vPosition)
{
    _vector vBlockingNormal = XMVectorZero();

    for (uint32_t i = 0; i < ETOUI(LINE_CELL::END); ++i)
    {
        _vector vLineNormal = XMLoadFloat3(&m_vNormals[i]);
        _vector vPoint = XMLoadFloat3(&m_vPoints[i]);

        _vector vToPos = vPosition - vPoint;

        float fDot = XMVectorGetX(XMVector3Dot(vLineNormal, vToPos));

        if (fDot > 0.f)
        {
            vBlockingNormal += vLineNormal;
        }
    }

    vBlockingNormal = XMVectorSetY(vBlockingNormal, 0.f);

    if (XMVector3Equal(vBlockingNormal, XMVectorZero()))
        return XMVectorZero();

    return XMVector3Normalize(vBlockingNormal);
}

#ifdef _DEBUG

HRESULT Cell::Render()
{
    m_pVIBuffer->Bind_Resources();

    m_pVIBuffer->Render();

    return S_OK;
}

#endif

shared_ptr<Cell> Cell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, int32_t iIndex)
{
    auto		pInstance = shared_ptr<Cell>(new Cell(pDevice, pContext));

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX("Failed to Created : Cell");
        return nullptr;
    }

    return pInstance;
}