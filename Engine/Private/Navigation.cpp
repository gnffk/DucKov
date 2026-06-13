#include "Navigation.h"
#include "Cell.h"

#include "GameInstance.h"
#include <queue>
namespace
{
    struct NAV_ASTAR_NODE
    {
        int32_t iCellIndex = -1;

        float fG = FLT_MAX;
        float fH = 0.f;

        int32_t iParentCellIndex = -1;
        bool bClosed = false;
    };

    struct NAV_ASTAR_OPEN
    {
        int32_t iCellIndex = -1;
        float fCost = 0.f;

        bool operator<(const NAV_ASTAR_OPEN& rhs) const
        {
            return fCost > rhs.fCost;
        }
    };

    float DistanceXZ(const _float3& vA, const _float3& vB)
    {
        float fX = vA.x - vB.x;
        float fZ = vA.z - vB.z;

        return sqrtf(fX * fX + fZ * fZ);
    }
}

Navigation::Navigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Component{ pDevice, pContext }
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

_float3 Navigation::Get_CellCenter(int32_t iCellIndex)
{
    _float3 vCenter{};

    if (iCellIndex < 0 ||
        iCellIndex >= static_cast<int32_t>(m_Cells.size()))
    {
        return vCenter;
    }

    if (m_Cells[iCellIndex] == nullptr)
        return vCenter;

    _vector vA = m_Cells[iCellIndex]->Get_Point(POINT_CELL::A);

    _vector vB = m_Cells[iCellIndex]->Get_Point(POINT_CELL::B);

    _vector vC = m_Cells[iCellIndex]->Get_Point(POINT_CELL::C);

    _vector vCenterVector = (vA + vB + vC) / 3.f;

    vCenterVector = XMVectorSetW(vCenterVector, 1.f);

    XMStoreFloat3(&vCenter, vCenterVector);

    return vCenter;
}
int32_t Navigation::Get_NeighborIndex(int32_t iCellIndex,LINE_CELL eLine) const
{
    if (iCellIndex < 0 || iCellIndex >= static_cast<int32_t>(m_Cells.size()))
    {
        return -1;
    }

    if (m_Cells[iCellIndex] == nullptr)
        return -1;

    return m_Cells[iCellIndex]->Get_NeighborIndex(eLine);
}
_bool Navigation::Build_AStarPath( _fvector vStartPosition, _fvector vGoalPosition, vector<_float3>& PathPoints)
{
    PathPoints.clear();

    int32_t iStartCellIndex =Find_CellIndex(vStartPosition);

    int32_t iGoalCellIndex = Find_CellIndex(vGoalPosition);

    if (iStartCellIndex == -1 || iGoalCellIndex == -1)
    {
        return false;
    }

    if (iStartCellIndex == iGoalCellIndex)
    {
        _float3 vGoal{};
        XMStoreFloat3(&vGoal, vGoalPosition);

        PathPoints.push_back(vGoal);
        return true;
    }

    unordered_map<int32_t, NAV_ASTAR_NODE> Nodes;
    priority_queue<NAV_ASTAR_OPEN> OpenList;

    _float3 vGoalCenter = Get_CellCenter(iGoalCellIndex);

    NAV_ASTAR_NODE StartNode{};
    StartNode.iCellIndex = iStartCellIndex;
    StartNode.fG = 0.f;
    StartNode.fH = DistanceXZ(Get_CellCenter(iStartCellIndex),vGoalCenter);
    StartNode.iParentCellIndex = -1;

    Nodes[iStartCellIndex] = StartNode;

    OpenList.push( NAV_ASTAR_OPEN{ iStartCellIndex, StartNode.fG + StartNode.fH});

    bool bFound = false;

    while (!OpenList.empty())
    {
        NAV_ASTAR_OPEN CurrentOpen = OpenList.top();

        OpenList.pop();

        auto CurrentIter = Nodes.find(CurrentOpen.iCellIndex);

        if (CurrentIter == Nodes.end())
            continue;

        NAV_ASTAR_NODE& CurrentNode = CurrentIter->second;

        if (CurrentNode.bClosed)
            continue;

        CurrentNode.bClosed = true;

        if (CurrentNode.iCellIndex == iGoalCellIndex)
        {
            bFound = true;
            break;
        }

        _float3 vCurrentCenter = Get_CellCenter(CurrentNode.iCellIndex);

        for (uint32_t i = 0; i < ETOUI(LINE_CELL::END); ++i)
        {
            int32_t iNeighborCellIndex =Get_NeighborIndex(CurrentNode.iCellIndex, static_cast<LINE_CELL>(i));

            if (iNeighborCellIndex == -1)
                continue;

            if (iNeighborCellIndex < 0 || iNeighborCellIndex >= static_cast<int32_t>(m_Cells.size()))
            {
                continue;
            }

            _float3 vNeighborCenter = Get_CellCenter(iNeighborCellIndex);

            float fMoveCost = DistanceXZ(vCurrentCenter, vNeighborCenter);

            float fNewG = CurrentNode.fG + fMoveCost;

            auto NeighborIter = Nodes.find(iNeighborCellIndex);

            if (NeighborIter == Nodes.end())
            {
                NAV_ASTAR_NODE NeighborNode{};
                NeighborNode.iCellIndex = iNeighborCellIndex;
                NeighborNode.fG = fNewG;
                NeighborNode.fH = DistanceXZ( vNeighborCenter,vGoalCenter);
                NeighborNode.iParentCellIndex =CurrentNode.iCellIndex;

                Nodes[iNeighborCellIndex] =NeighborNode;

                OpenList.push(NAV_ASTAR_OPEN{ iNeighborCellIndex, NeighborNode.fG + NeighborNode.fH});
            }
            else
            {
                NAV_ASTAR_NODE& NeighborNode = NeighborIter->second;

                if (NeighborNode.bClosed)
                    continue;

                if (fNewG < NeighborNode.fG)
                {
                    NeighborNode.fG = fNewG;
                    NeighborNode.iParentCellIndex = CurrentNode.iCellIndex;

                    OpenList.push( NAV_ASTAR_OPEN{iNeighborCellIndex, NeighborNode.fG + NeighborNode.fH});
                }
            }
        }
    }

    if (!bFound)
        return false;

    vector<int32_t> ReverseCellPath;

    int32_t iTraceCellIndex = iGoalCellIndex;

    while (iTraceCellIndex != -1)
    {
        ReverseCellPath.push_back(iTraceCellIndex);

        auto Iter = Nodes.find(iTraceCellIndex);

        if (Iter == Nodes.end())
            break;

        iTraceCellIndex = Iter->second.iParentCellIndex;
    }

    reverse( ReverseCellPath.begin(), ReverseCellPath.end());

    // 시작 Cell은 현재 위치라서 제외하고,
    // 다음 Cell Center부터 waypoint로 넣음.
    for (uint32_t i = 1; i < ReverseCellPath.size(); ++i)
    {
        _float3 vCenter = Get_CellCenter(ReverseCellPath[i]);

        _vector vCenterPosition = XMLoadFloat3(&vCenter);

        vCenterPosition = XMVectorSetW(vCenterPosition, 1.f);

        // 높이 보정
        int32_t iPrevCellIndex = m_iCurrentCellIndex;

        m_iCurrentCellIndex = ReverseCellPath[i];

        vCenterPosition = SetUp_OnNavigation(vCenterPosition);

        m_iCurrentCellIndex = iPrevCellIndex;

        XMStoreFloat3( &vCenter, vCenterPosition);

        PathPoints.push_back(vCenter);
    }

    _float3 vGoal{};
    XMStoreFloat3(&vGoal, vGoalPosition);

    PathPoints.push_back(vGoal);

    return true;
}

_bool Navigation::Can_MoveStraight(_fvector vStartPosition, _fvector vEndPosition, _float fStep)
{
    _vector vStart = vStartPosition;
    _vector vEnd = vEndPosition;

    _vector vDir = vEnd - vStart;
    vDir = XMVectorSetY(vDir, 0.f);

    _float fDistance =
        XMVectorGetX(XMVector3Length(vDir));

    if (fDistance <= 0.001f)
        return true;

    int32_t iSampleCount =
        max(1, static_cast<int32_t>(ceilf(fDistance / fStep)));

    for (int32_t i = 0; i <= iSampleCount; ++i)
    {
        _float fRatio =
            static_cast<_float>(i) /
            static_cast<_float>(iSampleCount);

        _vector vSample =
            XMVectorLerp(vStart, vEnd, fRatio);

        vSample =
            XMVectorSetW(vSample, 1.f);

        if (Find_CellIndex(vSample) == -1)
            return false;
    }

    return true;
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

_vector Navigation::Get_BlockingNormal(_fvector vPosition)
{
    if (m_iCurrentCellIndex < 0)
        return XMVectorZero();

    return m_Cells[m_iCurrentCellIndex]->Get_BlockingNormal(vPosition);
}