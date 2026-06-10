#pragma once

/* 네비게이션의 하나의 삼각형 */
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class Cell final
{
private:
	Cell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	~Cell();

public:
	_vector Get_Point(POINT_CELL ePoint) {
		return XMLoadFloat3(&m_vPoints[ETOUI(ePoint)]);
	}

	void Set_Neighbor(LINE_CELL eLine, shared_ptr<Cell> pCell) {
		m_iNeighborIndices[ETOUI(eLine)] = pCell->m_iIndex;
	}

	void Set_Neighbors(int32_t* pNeighborIndices) {
		memcpy(m_iNeighborIndices, pNeighborIndices, sizeof(int32_t) * 3);
	}



public:
	HRESULT Initialize(const _float3* pPoints, int32_t iIndex);
	_bool isIn(_fvector vResultPos, int32_t* pNeighborIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_fvector vPosition);

public:
	void WriteFile(HANDLE hFile);


#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private:
	int32_t							m_iIndex = { -1 };
	_float3							m_vPoints[ETOUI(POINT_CELL::END)] = {};
	_float3							m_vNormals[ETOUI(LINE_CELL::END)] = {};
	int32_t							m_iNeighborIndices[ETOUI(LINE_CELL::END)] = { -1, -1, -1 };
	_float4							m_vPlane = {};

#ifdef _DEBUG
private:
	unique_ptr<class VIBuffer_Cell>		m_pVIBuffer = { nullptr };

#endif
public:
	_vector Get_BlockingNormal(_fvector vPosition);

public:
	static shared_ptr<Cell> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, int32_t iIndex);

};

NS_END