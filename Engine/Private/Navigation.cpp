#include "Navigation.h"
#include "Cell.h"

#include "GameInstance.h"

Navigation::Navigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : Component{ pDevice, pContext }
{

}


Navigation::~Navigation()
{

}

HRESULT Navigation::Initialize_Prototype(const _tchar* pNavigationDataFilePath, const _tchar* pNavigationNeighborsFilePath)
{
    DWORD       dwByte = {};
    HANDLE      hFile = {};

    hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (0 == hFile)
        return E_FAIL;

    while (true)
    {
        _float3     vPoints[ETOUI(POINT_CELL::END)] = {};

        ReadFile(hFile, vPoints, sizeof(_float3) * ETOUI(POINT_CELL::END), &dwByte, nullptr);
        if (0 == dwByte)
            break;

        auto        pCell = Cell::Create(m_pDevice, m_pContext, vPoints, (int32_t)m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    if (nullptr == pNavigationNeighborsFilePath)
    {
        if (FAILED(Ready_Neighbors()))
            return E_FAIL;
    }
    else
    {
        if (FAILED(Ready_Neighbors(pNavigationNeighborsFilePath)))
            return E_FAIL;
    }



    //hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation_Neighbors.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    //if (0 == hFile)
    //    return E_FAIL;

    //for (auto& pCell : m_Cells)
    //    pCell->WriteFile(hFile);
    //
    //CloseHandle(hFile);

#ifdef _DEBUG

    m_pShader = Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT Navigation::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    auto        pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

    return S_OK;
}

_vector Navigation::SetUp_OnNavigation(_fvector vPosition)
{
    if (m_iCurrentCellIndex < 0 ||
        m_iCurrentCellIndex >= static_cast<int32_t>(m_Cells.size()))
    {
        if (false == Set_CurrentCell(vPosition))
            return vPosition;
    }

    return XMVectorSetY(
        vPosition,
        m_Cells[m_iCurrentCellIndex]->Compute_Height(vPosition)
    );
}
HRESULT Navigation::Ready_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT_CELL::A), pSourCell->Get_Point(POINT_CELL::B)))
            {
                pSourCell->Set_Neighbor(LINE_CELL::AB, pDestCell);
            }

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT_CELL::B), pSourCell->Get_Point(POINT_CELL::C)))
            {
                pSourCell->Set_Neighbor(LINE_CELL::BC, pDestCell);
            }

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(POINT_CELL::C), pSourCell->Get_Point(POINT_CELL::A)))
            {
                pSourCell->Set_Neighbor(LINE_CELL::CA, pDestCell);
            }

        }
    }

    return S_OK;
}

HRESULT Navigation::Ready_Neighbors(const _tchar* pNeighbors)
{
    DWORD       dwByte = {};
    HANDLE      hFile = {};

    hFile = CreateFile(pNeighbors, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (0 == hFile)
        return E_FAIL;

    uint32_t        iCellIndex = {};

    while (true)
    {
        int32_t     iNeighborIndices[ETOUI(LINE_CELL::END)] = {};

        ReadFile(hFile, iNeighborIndices, sizeof(int32_t) * ETOUI(LINE_CELL::END), &dwByte, nullptr);
        if (0 == dwByte)
            break;

        m_Cells[iCellIndex++]->Set_Neighbors(iNeighborIndices);
    }

    CloseHandle(hFile);

    return S_OK;
}

_bool Navigation::isMove(_fvector vResultPos)
{
    if (-1 == m_iCurrentCellIndex)
        return false;

    int32_t     iNeighborIndex = { -1 };

    if (true == m_Cells[m_iCurrentCellIndex]->isIn(vResultPos, &iNeighborIndex))
    {
        return true;
    }

    else
    {
        /*  나간 방향에 이웃이 존재한다면. */
        if (-1 != iNeighborIndex)
        {
            while (true)
            {
                if (true == m_Cells[iNeighborIndex]->isIn(vResultPos, &iNeighborIndex))
                    break;

                if (-1 == iNeighborIndex)
                    return false;
            }


            m_iCurrentCellIndex = iNeighborIndex;
            return true;
        }

        /*  나간 방향에 이웃이 없다면. */
        else
            return false;
    }
}

#ifdef _DEBUG
HRESULT Navigation::Render()
{
    _float4x4       WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    _float4x4 View, Proj;
    CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &View)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &Proj)))
        return E_FAIL;

    _float4     vColor = _float4(1.f, 1.f, 0.f, 1.f);

    if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
        return E_FAIL;

    m_pShader->Begin(0);

    /*for (auto& pCell : m_Cells)
    {
        if (nullptr != pCell)
            pCell->Render();
    }*/

    m_Cells[m_iCurrentCellIndex]->Render();

    return S_OK;
}
#endif

int32_t Navigation::Find_CellIndex(_fvector vPosition)
{
    for (int32_t i = 0; i < static_cast<int32_t>(m_Cells.size()); ++i)
    {
        if (nullptr == m_Cells[i])
            continue;

        int32_t iNeighborIndex = -1;

        if (true == m_Cells[i]->isIn(vPosition, &iNeighborIndex))
            return i;
    }

    return -1;
}
_bool Navigation::Set_CurrentCell(_fvector vPosition)
{
    const int32_t iCellIndex = Find_CellIndex(vPosition);

    if (-1 == iCellIndex)
        return false;

    m_iCurrentCellIndex = iCellIndex;

    return true;
}
unique_ptr<Navigation> Navigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pNavigationDataFilePath, const _tchar* pNavigationNeighborsFilePath)
{
    auto		pInstance = unique_ptr<Navigation>(new Navigation(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFilePath, pNavigationNeighborsFilePath)))
    {
        MSG_BOX("Failed to Created : Navigation");
        return nullptr;
    }

    return pInstance;
}


shared_ptr<Prototype> Navigation::Clone(void* pArg)
{
    auto		pInstance = shared_ptr<Navigation>(new Navigation(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : Navigation");
        return nullptr;
    }

    return pInstance;
}

