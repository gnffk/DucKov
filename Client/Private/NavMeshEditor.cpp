#include "NavMeshEditor.h"
#include "GameInstance.h"
#include "Terrain.h"
NavMeshEditor::NavMeshEditor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
}

NavMeshEditor::~NavMeshEditor()
{
}

HRESULT NavMeshEditor::Initialize()
{
	m_pShader = Shader::Create(m_pDevice, m_pContext, TEXT("../../Resources/Shaders/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	return S_OK;
}

void NavMeshEditor::Update()
{
	if (!m_bEnabled)
		return;

	if (ImGui::GetIO().WantCaptureMouse)
		return;

	const bool bShift =
		CGameInstance::Get().Key_Pressing(DIK_LSHIFT);

	const bool bClick =
		CGameInstance::Get().Mouse_Down(MOUSEKEYSTATE::DIM_LB) &&
		bShift;

	const bool bDrag =
		CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_LB) &&
		bShift;

	// 아무 입력도 없으면 Picking 자체를 하지 않는다.
	// 이게 프레임 드랍 잡는 핵심.
	if (!bClick && !bDrag)
		return;

	// Edit 모드에서 선택된 점도 없는데 드래그 중이면 Picking할 필요 없음.
	if (m_eMode == MODE::EDIT && bDrag)
	{
		if (m_iSelectedTriangle < 0 || m_iSelectedPoint < 0)
		{
			if (!bClick)
				return;
		}
	}

	_float3 vPick = {};

	// 클릭/드래그가 있을 때만 Terrain Picking
	if (!Pick_OnTerrain(vPick))
	{
		if (!Pick_OnPlane(vPick))
			return;
	}

	if (m_eMode == MODE::INSTALL && bClick)
	{
		Snap_Point(vPick);

		m_PendingPoints.push_back(vPick);

		if (m_PendingPoints.size() == ETOUI(POINT_CELL::END))
		{
			Add_Triangle(m_PendingPoints.data());
			m_PendingPoints.clear();
		}
	}
	else if (m_eMode == MODE::EDIT)
	{
		if (bClick)
			Find_NearPoint(vPick, m_iSelectedTriangle, m_iSelectedPoint);

		if (bDrag && 0 <= m_iSelectedTriangle && 0 <= m_iSelectedPoint)
		{
			Snap_Point(vPick);

			auto& Triangle = m_Triangles[m_iSelectedTriangle];
			Triangle.Points[m_iSelectedPoint] = vPick;

			Normalize_Winding(Triangle);

			// 드래그 중 매 프레임 Neighbor 재계산하면 무거움.
			// 일단 버퍼만 갱신.
			Rebuild_Buffer(Triangle);
		}
	}
	else if (m_eMode == MODE::ERASE && bClick)
	{
		const int32_t iTriangle = Find_NearTriangle(vPick);

		if (0 <= iTriangle)
			Delete_Triangle(static_cast<size_t>(iTriangle));
	}
}
HRESULT NavMeshEditor::Render()
{
	if (!m_bEnabled || !m_bShowAll || nullptr == m_pShader)
		return S_OK;

	_float4x4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	_float4x4 ViewMatrix = {};
	_float4x4 ProjMatrix = {};
	CGameInstance::Get().Get_MainCameraMatrix(ViewMatrix, ProjMatrix);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	for (size_t i = 0; i < m_Triangles.size(); ++i)
	{
		if (nullptr == m_Triangles[i].Buffer)
			continue;

		_float4 vColor = (static_cast<int32_t>(i) == m_iSelectedTriangle) ?
			_float4(0.f, 1.f, 0.2f, 1.f) :
			_float4(0.f, 0.55f, 1.f, 1.f);

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
			return E_FAIL;

		m_pShader->Begin(0);
		m_Triangles[i].Buffer->Bind_Resources();
		m_Triangles[i].Buffer->Render();
	}

	return S_OK;
}

void NavMeshEditor::IMGUI_Render()
{
	ImGui::Begin("Navigation Mesh Editor");

	ImGui::Checkbox("Enabled", &m_bEnabled);
	ImGui::SameLine();
	ImGui::Checkbox("Show Mesh", &m_bShowAll);

	const char* pModeNames[] = { "Install", "Edit", "Erase" };
	int32_t iMode = static_cast<int32_t>(m_eMode);
	if (ImGui::Combo("Mode", &iMode, pModeNames, IM_ARRAYSIZE(pModeNames)))
		m_eMode = static_cast<MODE>(iMode);

	ImGui::DragFloat("Draw Plane Y", &m_fDrawPlaneY, 0.1f, -1000.f, 1000.f);
	ImGui::DragFloat("Pick Radius", &m_fPickRadius, 0.1f, 0.1f, 100.f);

	ImGui::Separator();
	ImGui::Text("Shift + Left Click: draw/select/delete");
	ImGui::Text("New points snap to existing vertices inside Pick Radius.");
	ImGui::Text("Triangles: %d", static_cast<int32_t>(m_Triangles.size()));
	ImGui::Text("Pending Points: %d / 3", static_cast<int32_t>(m_PendingPoints.size()));

	if (ImGui::Button("Cancel Pending"))
		m_PendingPoints.clear();

	ImGui::SameLine();
	if (ImGui::Button("Normalize All"))
	{
		Normalize_AllWinding();
		Rebuild_Neighbors();
		Rebuild_AllBuffers();
	}

	ImGui::Separator();
	ImGui::InputText("Nav File", m_szNavigationFile, IM_ARRAYSIZE(m_szNavigationFile));
	ImGui::InputText("Neighbor File", m_szNeighborFile, IM_ARRAYSIZE(m_szNeighborFile));

	if (ImGui::Button("Load"))
	{
		const bool bLoaded = Load_NavigationData(CGameInstance::Get().StringToWString(m_szNavigationFile).c_str());
		strcpy_s(m_szStatus, bLoaded ? "Loaded navigation data." : "Load failed. Check path.");
	}

	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		Normalize_AllWinding();
		Rebuild_Neighbors();

		const bool bSavedNav = Save_NavigationData(CGameInstance::Get().StringToWString(m_szNavigationFile).c_str());
		const bool bSavedNeighbor = Save_NeighborData(CGameInstance::Get().StringToWString(m_szNeighborFile).c_str());

		strcpy_s(m_szStatus, (bSavedNav && bSavedNeighbor) ? "Saved navigation data." : "Save failed. Check path.");
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
		Clear();

	ImGui::Text("Status: %s", m_szStatus);

	ImGui::Separator();

	if (0 <= m_iSelectedTriangle && m_iSelectedTriangle < static_cast<int32_t>(m_Triangles.size()))
	{
		auto& Triangle = m_Triangles[m_iSelectedTriangle];

		ImGui::Text("Selected Triangle: %d", m_iSelectedTriangle);

		bool bChanged = false;
		for (uint32_t i = 0; i < ETOUI(POINT_CELL::END); ++i)
		{
			char szLabel[32] = {};
			sprintf_s(szLabel, "Point %u", i);
			bChanged |= ImGui::DragFloat3(szLabel, reinterpret_cast<float*>(&Triangle.Points[i]), 0.1f);
		}

		if (bChanged)
		{
			Normalize_Winding(Triangle);
			Rebuild_Neighbors();
			Rebuild_Buffer(Triangle);
		}

		if (ImGui::Button("Delete Selected"))
			Delete_Triangle(static_cast<size_t>(m_iSelectedTriangle));
	}

	ImGui::Separator();
	ImGui::BeginChild("NavMeshTriangles", ImVec2(0.f, 180.f), true);
	for (int32_t i = 0; i < static_cast<int32_t>(m_Triangles.size()); ++i)
	{
		char szLabel[64] = {};
		sprintf_s(szLabel, "Triangle %d", i);
		if (ImGui::Selectable(szLabel, i == m_iSelectedTriangle))
		{
			m_iSelectedTriangle = i;
			m_iSelectedPoint = -1;
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void NavMeshEditor::Add_Triangle(const _float3* pPoints)
{
	TRIANGLE Triangle = {};
	memcpy(Triangle.Points, pPoints, sizeof(_float3) * ETOUI(POINT_CELL::END));

	Normalize_Winding(Triangle);
	Rebuild_Buffer(Triangle);

	m_Triangles.push_back(std::move(Triangle));
	m_iSelectedTriangle = static_cast<int32_t>(m_Triangles.size()) - 1;
	m_iSelectedPoint = -1;

	Rebuild_Neighbors();
}

void NavMeshEditor::Delete_Triangle(size_t iTriangleIndex)
{
	if (iTriangleIndex >= m_Triangles.size())
		return;

	m_Triangles.erase(m_Triangles.begin() + iTriangleIndex);
	m_iSelectedTriangle = -1;
	m_iSelectedPoint = -1;

	Rebuild_Neighbors();
}

void NavMeshEditor::Clear()
{
	m_Triangles.clear();
	m_PendingPoints.clear();
	m_iSelectedTriangle = -1;
	m_iSelectedPoint = -1;
}

void NavMeshEditor::Normalize_Winding(TRIANGLE& Triangle)
{
	if (SignedAreaXZ(Triangle) > 0.f)
		std::swap(Triangle.Points[ETOUI(POINT_CELL::B)], Triangle.Points[ETOUI(POINT_CELL::C)]);
}

void NavMeshEditor::Normalize_AllWinding()
{
	for (auto& Triangle : m_Triangles)
		Normalize_Winding(Triangle);
}

void NavMeshEditor::Rebuild_Neighbors()
{
	for (auto& Triangle : m_Triangles)
	{
		for (uint32_t i = 0; i < ETOUI(LINE_CELL::END); ++i)
			Triangle.NeighborIndices[i] = -1;
	}

	auto SamePoint = [](const _float3& A, const _float3& B)
		{
			constexpr float EPSILON = 0.0001f;

			return fabsf(A.x - B.x) < EPSILON &&
				fabsf(A.y - B.y) < EPSILON &&
				fabsf(A.z - B.z) < EPSILON;
		};

	auto HasPoint = [&](const TRIANGLE& Triangle, const _float3& Point)
		{
			for (uint32_t i = 0; i < ETOUI(POINT_CELL::END); ++i)
			{
				if (SamePoint(Triangle.Points[i], Point))
					return true;
			}

			return false;
		};

	for (int32_t i = 0; i < static_cast<int32_t>(m_Triangles.size()); ++i)
	{
		for (int32_t j = 0; j < static_cast<int32_t>(m_Triangles.size()); ++j)
		{
			if (i == j)
				continue;

			const TRIANGLE& Dest = m_Triangles[j];

			if (HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::A)]) && HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::B)]))
				m_Triangles[i].NeighborIndices[ETOUI(LINE_CELL::AB)] = j;
			if (HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::B)]) && HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::C)]))
				m_Triangles[i].NeighborIndices[ETOUI(LINE_CELL::BC)] = j;
			if (HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::C)]) && HasPoint(Dest, m_Triangles[i].Points[ETOUI(POINT_CELL::A)]))
				m_Triangles[i].NeighborIndices[ETOUI(LINE_CELL::CA)] = j;
		}
	}
}

HRESULT NavMeshEditor::Rebuild_Buffer(TRIANGLE& Triangle)
{
	Triangle.Buffer = VIBuffer_Cell::Create(m_pDevice, m_pContext, Triangle.Points);
	if (nullptr == Triangle.Buffer)
		return E_FAIL;

	return S_OK;
}

HRESULT NavMeshEditor::Rebuild_AllBuffers()
{
	for (auto& Triangle : m_Triangles)
	{
		if (FAILED(Rebuild_Buffer(Triangle)))
			return E_FAIL;
	}

	return S_OK;
}

bool NavMeshEditor::Save_NavigationData(const _tchar* pNavigationDataFilePath)
{
	if (!Ready_SavePath(pNavigationDataFilePath))
		return false;

	HANDLE hFile = CreateFile(pNavigationDataFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	DWORD dwByte = {};
	for (const auto& Triangle : m_Triangles)
		WriteFile(hFile, Triangle.Points, sizeof(_float3) * ETOUI(POINT_CELL::END), &dwByte, nullptr);

	CloseHandle(hFile);
	return true;
}

bool NavMeshEditor::Save_NeighborData(const _tchar* pNavigationNeighborFilePath)
{
	if (!Ready_SavePath(pNavigationNeighborFilePath))
		return false;

	HANDLE hFile = CreateFile(pNavigationNeighborFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	DWORD dwByte = {};
	for (const auto& Triangle : m_Triangles)
		WriteFile(hFile, Triangle.NeighborIndices, sizeof(int32_t) * ETOUI(LINE_CELL::END), &dwByte, nullptr);

	CloseHandle(hFile);
	return true;
}

bool NavMeshEditor::Load_NavigationData(const _tchar* pNavigationDataFilePath)
{
	HANDLE hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	Clear();

	DWORD dwByte = {};
	while (true)
	{
		_float3 Points[ETOUI(POINT_CELL::END)] = {};
		ReadFile(hFile, Points, sizeof(_float3) * ETOUI(POINT_CELL::END), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (dwByte == sizeof(_float3) * ETOUI(POINT_CELL::END))
			Add_Triangle(Points);
	}

	CloseHandle(hFile);

	Normalize_AllWinding();
	Rebuild_Neighbors();
	Rebuild_AllBuffers();

	return true;
}

bool NavMeshEditor::Ready_SavePath(const _tchar* pFilePath) const
{
	try
	{
		std::filesystem::path SavePath = pFilePath;
		const auto ParentPath = SavePath.parent_path();

		if (!ParentPath.empty() && !std::filesystem::exists(ParentPath))
			std::filesystem::create_directories(ParentPath);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool NavMeshEditor::Pick_OnPlane(_float3& vOutPoint) const
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT rcClient = {};
	GetClientRect(g_hWnd, &rcClient);

	const float fWidth = static_cast<float>(rcClient.right - rcClient.left);
	const float fHeight = static_cast<float>(rcClient.bottom - rcClient.top);
	if (fWidth <= 0.f || fHeight <= 0.f)
		return false;

	const float fPX = static_cast<float>(pt.x) / (fWidth * 0.5f) - 1.f;
	const float fPY = static_cast<float>(pt.y) / -(fHeight * 0.5f) + 1.f;

	_float4x4 ViewMatrix = {};
	_float4x4 ProjMatrix = {};
	CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix);

	const _matrix InvView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix));
	const _matrix InvProj = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ProjMatrix));

	_vector vNear = XMVectorSet(fPX, fPY, 0.f, 1.f);
	vNear = XMVector3TransformCoord(vNear, InvProj);
	vNear = XMVector3TransformCoord(vNear, InvView);

	_vector vFar = XMVectorSet(fPX, fPY, 1.f, 1.f);
	vFar = XMVector3TransformCoord(vFar, InvProj);
	vFar = XMVector3TransformCoord(vFar, InvView);

	const _vector vDir = XMVector3Normalize(vFar - vNear);
	const float fDirY = XMVectorGetY(vDir);
	if (fabsf(fDirY) < 0.0001f)
		return false;

	const float fT = (m_fDrawPlaneY - XMVectorGetY(vNear)) / fDirY;
	if (fT < 0.f)
		return false;

	XMStoreFloat3(&vOutPoint, vNear + vDir * fT);
	return true;
}

bool NavMeshEditor::Snap_Point(_float3& vPoint) const
{
	const float fSnapDistanceSq = m_fPickRadius * m_fPickRadius;
	float fNearestDistance = fSnapDistanceSq;
	bool bSnapped = false;

	for (const auto& Triangle : m_Triangles)
	{
		for (uint32_t i = 0; i < ETOUI(POINT_CELL::END); ++i)
		{
			const float fDistance = DistanceSqXZ(vPoint, Triangle.Points[i]);
			if (fDistance < fNearestDistance)
			{
				fNearestDistance = fDistance;
				vPoint = Triangle.Points[i];
				bSnapped = true;
			}
		}
	}

	return bSnapped;
}

int32_t NavMeshEditor::Find_NearTriangle(const _float3& vPoint) const
{
	for (int32_t i = 0; i < static_cast<int32_t>(m_Triangles.size()); ++i)
	{
		if (Is_PointInTriangleXZ(vPoint, m_Triangles[i]))
			return i;
	}

	int32_t iNearest = -1;
	float fNearestDistance = m_fPickRadius * m_fPickRadius;

	for (int32_t i = 0; i < static_cast<int32_t>(m_Triangles.size()); ++i)
	{
		_float3 vCenter =
		{
			(m_Triangles[i].Points[0].x + m_Triangles[i].Points[1].x + m_Triangles[i].Points[2].x) / 3.f,
			(m_Triangles[i].Points[0].y + m_Triangles[i].Points[1].y + m_Triangles[i].Points[2].y) / 3.f,
			(m_Triangles[i].Points[0].z + m_Triangles[i].Points[1].z + m_Triangles[i].Points[2].z) / 3.f
		};

		const float fDistance = DistanceSqXZ(vPoint, vCenter);
		if (fDistance < fNearestDistance)
		{
			fNearestDistance = fDistance;
			iNearest = i;
		}
	}

	return iNearest;
}
bool NavMeshEditor::Pick_OnTerrain(_float3& vOutPoint) const
{
	auto pTerrain = dynamic_pointer_cast<Terrain>(
		CGameInstance::Get().Find_Object(
			CGameInstance::Get().Get_Level(),
			L"Base",
			L"Terrian"
		)
	);

	if (nullptr == pTerrain)
		return false;

#ifdef _DEBUG
	return pTerrain->Picking_Terrain_ForNavMesh(vOutPoint);
#endif

#ifdef NDEBUG
	return false;
#endif
	
}
bool NavMeshEditor::Find_NearPoint(const _float3& vPoint, int32_t& iOutTriangle, int32_t& iOutPoint) const
{
	iOutTriangle = -1;
	iOutPoint = -1;

	float fNearestDistance = m_fPickRadius * m_fPickRadius;

	for (int32_t i = 0; i < static_cast<int32_t>(m_Triangles.size()); ++i)
	{
		for (int32_t j = 0; j < static_cast<int32_t>(ETOUI(POINT_CELL::END)); ++j)
		{
			const float fDistance = DistanceSqXZ(vPoint, m_Triangles[i].Points[j]);
			if (fDistance < fNearestDistance)
			{
				fNearestDistance = fDistance;
				iOutTriangle = i;
				iOutPoint = j;
			}
		}
	}

	return 0 <= iOutTriangle;
}

bool NavMeshEditor::Is_PointInTriangleXZ(const _float3& vPoint, const TRIANGLE& Triangle) const
{
	auto Sign = [](const _float3& A, const _float3& B, const _float3& C)
		{
			return (A.x - C.x) * (B.z - C.z) - (B.x - C.x) * (A.z - C.z);
		};

	const float fD1 = Sign(vPoint, Triangle.Points[0], Triangle.Points[1]);
	const float fD2 = Sign(vPoint, Triangle.Points[1], Triangle.Points[2]);
	const float fD3 = Sign(vPoint, Triangle.Points[2], Triangle.Points[0]);

	const bool bHasNegative = (fD1 < 0.f) || (fD2 < 0.f) || (fD3 < 0.f);
	const bool bHasPositive = (fD1 > 0.f) || (fD2 > 0.f) || (fD3 > 0.f);

	return !(bHasNegative && bHasPositive);
}

float NavMeshEditor::DistanceSqXZ(const _float3& vSour, const _float3& vDest) const
{
	const float fX = vSour.x - vDest.x;
	const float fZ = vSour.z - vDest.z;
	return fX * fX + fZ * fZ;
}

float NavMeshEditor::SignedAreaXZ(const TRIANGLE& Triangle) const
{
	const _float3& A = Triangle.Points[ETOUI(POINT_CELL::A)];
	const _float3& B = Triangle.Points[ETOUI(POINT_CELL::B)];
	const _float3& C = Triangle.Points[ETOUI(POINT_CELL::C)];

	return (B.x - A.x) * (C.z - A.z) - (B.z - A.z) * (C.x - A.x);
}

unique_ptr<NavMeshEditor> NavMeshEditor::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto pInstance = unique_ptr<NavMeshEditor>(new NavMeshEditor(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : NavMeshEditor");
		return nullptr;
	}

	return pInstance;
}
