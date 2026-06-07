#pragma once

#include "Client_Defines.h"
#include "Shader.h"
#include "VIBuffer_Cell.h"

NS_BEGIN(Client)

class NavMeshEditor final
{
public:
	enum class MODE
	{
		INSTALL,
		EDIT,
		ERASE,
		TERRAIN,
		END
	};
private:
	struct TRIANGLE
	{
		_float3 Points[ETOUI(POINT_CELL::END)] = {};
		int32_t NeighborIndices[ETOUI(LINE_CELL::END)] = { -1, -1, -1 };
		unique_ptr<VIBuffer_Cell> Buffer = nullptr;
	};

private:
	NavMeshEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

public:
	~NavMeshEditor();

public:
	HRESULT Initialize();
	void Update();
	HRESULT Render();
	void IMGUI_Render();

private:
	void Add_Triangle(const _float3* pPoints);
	void Delete_Triangle(size_t iTriangleIndex);
	void Clear();

	void Normalize_Winding(TRIANGLE& Triangle);
	void Normalize_AllWinding();
	void Rebuild_Neighbors();
	HRESULT Rebuild_Buffer(TRIANGLE& Triangle);
	HRESULT Rebuild_AllBuffers();

	bool Save_NavigationData(const _tchar* pNavigationDataFilePath);
	bool Save_NeighborData(const _tchar* pNavigationNeighborFilePath);
	bool Load_NavigationData(const _tchar* pNavigationDataFilePath);
	bool Ready_SavePath(const _tchar* pFilePath) const;

	bool Pick_OnPlane(_float3& vOutPoint) const;
	bool Snap_Point(_float3& vPoint) const;
	int32_t Find_NearTriangle(const _float3& vPoint) const;
	bool Find_NearPoint(const _float3& vPoint, int32_t& iOutTriangle, int32_t& iOutPoint) const;
	bool Is_PointInTriangleXZ(const _float3& vPoint, const TRIANGLE& Triangle) const;
	float DistanceSqXZ(const _float3& vSour, const _float3& vDest) const;
	float SignedAreaXZ(const TRIANGLE& Triangle) const;

private:
	bool Pick_OnTerrain(_float3& vOutPoint) const;

private:
	ComPtr<ID3D11Device> m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	vector<TRIANGLE> m_Triangles;
	vector<_float3> m_PendingPoints;

	MODE m_eMode = MODE::INSTALL;
	bool m_bEnabled = true;
	bool m_bShowAll = true;
	float m_fDrawPlaneY = 0.f;
	float m_fPickRadius = 2.f;

	int32_t m_iSelectedTriangle = -1;
	int32_t m_iSelectedPoint = -1;
	char m_szNavigationFile[MAX_PATH] = "../../Resources/DataFiles/Navigation.dat";
	char m_szNeighborFile[MAX_PATH] = "../../Resources/DataFiles/Navigation_Neighbors.dat";
	char m_szStatus[128] = "Ready";

	shared_ptr<Shader> m_pShader = nullptr;

public:
	static unique_ptr<NavMeshEditor> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
};

NS_END
