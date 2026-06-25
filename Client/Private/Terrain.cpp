#include "Terrain.h"

#include "GameInstance.h"

Terrain::Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Terrain::Terrain(const Terrain& Prototype)
	: GameObject{ Prototype }
{
}

Terrain::~Terrain()
{
	int a = 10;
}


HRESULT Terrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

#if _DEBUG
	if (FAILED(Create_SplatTexture()))
		return E_FAIL;

#endif // _DEBUG




	return S_OK;
}

void Terrain::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);



}

void Terrain::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

#if _DEBUG
	GUI_TerrainPaint();

	if (m_bTerrainPaintMode)
	{
		if (CGameInstance::Get().Mouse_Pressing(MOUSEKEYSTATE::DIM_LB))
		{
			_float3 vPickPos{};

			if (Picking_Terrain(vPickPos))
			{
				Paint_Splat(vPickPos);
			}
		}
		else {
			m_CheckPickTerrainNum = { 0.f,0.f,0.f,0.f };
		}
	}
#endif
}

void Terrain::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Terrain));

	__super::Late_Update(fTimeDelta);
}

HRESULT Terrain::Render()
{

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();
	_float2 vTerrainSize = { (float)(m_pVIBufferCom->GetNumVerticesX() - 1),(float)(m_pVIBufferCom->GetNumVerticesZ() - 1) };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TerrainSize", &vTerrainSize, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;
	if (FAILED(m_pGrassTex->Bind_ShaderResource(m_pShaderCom, "g_GrassTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pMudTex->Bind_ShaderResource(m_pShaderCom, "g_MudTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pRoadTex->Bind_ShaderResource(m_pShaderCom, "g_RoadTexture", 0)))
		return E_FAIL;





#if _DEBUG	
	ID3D11ShaderResourceView* pSRV = m_pSplatSRV.Get();
	if (FAILED(m_pShaderCom->Bind_SRV("g_SplatTexture", m_pSplatSRV.Get())))
		return E_FAIL;

#else
	if (FAILED(m_pSplatTex->Bind_ShaderResource(m_pShaderCom, "g_SplatTexture", 0)))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT Terrain::Ready_Components()
{
	__super::Clear_Compnent();

	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Terrain>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_VIBuffer_Terrain")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_Terrian")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pGrassTex = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_Terrain_Grass")));

	if (FAILED(__super::Add_Component(TEXT("Com_Texture_Grass"), m_pGrassTex)))
		return E_FAIL;


	m_pMudTex = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_Terrain_Mud")));

	if (FAILED(__super::Add_Component(TEXT("Com_Texture_Mud"), m_pMudTex)))
		return E_FAIL;

	m_pRoadTex = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_Terrain_Road")));

	if (FAILED(__super::Add_Component(TEXT("Com_Texture_Raod"), m_pRoadTex)))
		return E_FAIL;


#if _RELEASE
	m_pSplatTex = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_Terrain_Splat")));

	if (FAILED(__super::Add_Component(TEXT("Com_Texture_Splat"), m_pSplatTex)))
		return E_FAIL;
#endif
	return S_OK;
}

#if _DEBUG
void Terrain::Save_SplatPNG(const char* pFilePath)
{
	vector<unsigned char> pixels;
	pixels.resize(m_iSplatWidth * m_iSplatHeight * 4);

	for (uint32_t y = 0; y < m_iSplatHeight; ++y)
	{
		for (uint32_t x = 0; x < m_iSplatWidth; ++x)
		{
			uint32_t srcIndex = y * m_iSplatWidth + x;
			uint32_t dstIndex = srcIndex * 4;

			_float4& src = m_SplatPixels[srcIndex];

			pixels[dstIndex + 0] = (unsigned char)(std::clamp(src.x, 0.f, 1.f) * 255.f); // R Grass
			pixels[dstIndex + 1] = (unsigned char)(std::clamp(src.y, 0.f, 1.f) * 255.f); // G Mud
			pixels[dstIndex + 2] = (unsigned char)(std::clamp(src.z, 0.f, 1.f) * 255.f); // B
			pixels[dstIndex + 3] = 255;                                                  // A
		}
	}

	stbi_write_png(pFilePath,m_iSplatWidth,m_iSplatHeight,4,pixels.data(),m_iSplatWidth * 4);
}
_bool Terrain::Load_SplatPNG(const char* pFilePath)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	unsigned char* data = stbi_load(pFilePath,&width,&height,&channels,4);

	if (data == nullptr)
		return false;

	m_iSplatWidth = width;
	m_iSplatHeight = height;

	m_SplatPixels.resize(m_iSplatWidth * m_iSplatHeight);

	for (uint32_t y = 0; y < m_iSplatHeight; ++y)
	{
		for (uint32_t x = 0; x < m_iSplatWidth; ++x)
		{
			uint32_t index = y * m_iSplatWidth + x;
			uint32_t srcIndex = index * 4;

			m_SplatPixels[index].x = data[srcIndex + 0] / 255.f; // R Grass
			m_SplatPixels[index].y = data[srcIndex + 1] / 255.f; // G Mud
			m_SplatPixels[index].z = data[srcIndex + 2] / 255.f;
			m_SplatPixels[index].w = data[srcIndex + 3] / 255.f;
		}
	}

	stbi_image_free(data);

	return true;
}
void Terrain::GUI_TerrainPaint()
{
	ImGui::Begin("Terrain Paint");

	ImGui::Checkbox("Paint Mode", &m_bTerrainPaintMode);

	ImGui::Separator();

	ImGui::Text("Paint Layer");

	if (ImGui::RadioButton("Grass", m_iPaintChannel == 0))
		m_iPaintChannel = 0;

	if (ImGui::RadioButton("Mud", m_iPaintChannel == 1))
		m_iPaintChannel = 1;

	if (ImGui::RadioButton("Road", m_iPaintChannel == 2))
		m_iPaintChannel = 2;

	ImGui::Separator();

	ImGui::DragFloat("Brush Radius", &m_fBrushRadius, 1.f, 1.f, 200.f);
	ImGui::DragFloat("Brush Power", &m_fBrushPower, 0.01f, 0.001f, 1.f);

	ImGui::Separator();

	if (ImGui::Button("Save Splat PNG"))
	{
		Save_SplatPNG("../../Resources/Textures/Terrian/Terrain_Splat1.png");
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Splat Debug", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Splat Size : %u x %u", m_iSplatWidth, m_iSplatHeight);

		if (m_bHasSplatDebug)
		{
			ImGui::Text("Pick World : %.3f, %.3f, %.3f",
				m_vDebugPickWorld.x,
				m_vDebugPickWorld.y,
				m_vDebugPickWorld.z);

			ImGui::Text("Pick Local : %.3f, %.3f, %.3f",
				m_vDebugPickLocal.x,
				m_vDebugPickLocal.y,
				m_vDebugPickLocal.z);

			ImGui::Text("Splat Pixel : %d, %d",
				m_iDebugSplatX,
				m_iDebugSplatY);

			ImGui::Text("Recon Local : %.3f, %.3f",
				m_fDebugReconLocalX,
				m_fDebugReconLocalZ);

			ImGui::Text("Error : %.3f, %.3f",
				m_fDebugErrorX,
				m_fDebugErrorZ);
		}
		else
		{
			ImGui::Text("No paint debug data yet.");
		}

		ImGui::Separator();

		if (m_pSplatSRV)
		{
			ImVec2 imageSize = ImVec2(256.f, 256.f);
			ImVec2 imagePos = ImGui::GetCursorScreenPos();

			ImGui::Image(
				(ImTextureID)m_pSplatSRV.Get(),
				imageSize,
				ImVec2(0.f, 0.f),
				ImVec2(1.f, 1.f));

			if (m_bHasSplatDebug)
			{
				float u = 0.f;
				float v = 0.f;

				if (m_iSplatWidth > 1)
					u = (float)m_iDebugSplatX / (float)(m_iSplatWidth - 1);

				if (m_iSplatHeight > 1)
					v = (float)m_iDebugSplatY / (float)(m_iSplatHeight - 1);

				ImVec2 p;
				p.x = imagePos.x + u * imageSize.x;
				p.y = imagePos.y + v * imageSize.y;

				ImDrawList* drawList = ImGui::GetWindowDrawList();

				drawList->AddCircleFilled(
					p,
					5.f,
					IM_COL32(255, 0, 0, 255));

				drawList->AddLine(
					ImVec2(p.x - 10.f, p.y),
					ImVec2(p.x + 10.f, p.y),
					IM_COL32(255, 255, 0, 255),
					2.f);

				drawList->AddLine(
					ImVec2(p.x, p.y - 10.f),
					ImVec2(p.x, p.y + 10.f),
					IM_COL32(255, 255, 0, 255),
					2.f);
			}
		}
	}
	ImGui::End();
}
HRESULT Terrain::Create_SplatTexture()
{
	if (!Load_SplatPNG("../../Resources/Textures/Terrian/Terrain_Splat1.png"))
	{
		m_SplatPixels.resize(m_iSplatWidth * m_iSplatHeight);

		for (auto& pixel : m_SplatPixels)
		{
			pixel.x = 1.f;
			pixel.y = 0.f;
			pixel.z = 0.f;
			pixel.w = 0.f;
		}
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = m_iSplatWidth;
	desc.Height = m_iSplatHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_SplatPixels.data();
	initData.SysMemPitch = m_iSplatWidth * sizeof(_float4);

	if (FAILED(m_pDevice->CreateTexture2D(&desc, &initData, m_pSplatTexture2D.GetAddressOf())))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(
		m_pSplatTexture2D.Get(),
		&srvDesc,
		m_pSplatSRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}
_bool Terrain::Picking_Terrain(_float3& vPickingPoint)
{
	if (m_pVIBufferCom == nullptr || m_pTransformCom == nullptr)
		return false;

	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	float mouseX = static_cast<float>(pt.x);
	float mouseY = static_cast<float>(pt.y);

	float width = CGameInstance::Get().Get_ViewportSize().x;
	float height = CGameInstance::Get().Get_ViewportSize().y;

	if (width <= 0.f || height <= 0.f)
		return false;

	float px = mouseX / (width * 0.5f) - 1.0f;
	float py = mouseY / -(height * 0.5f) + 1.0f;

	_float4x4 proj{}, view{}, world{};
	CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);

	world = m_pTransformCom->GetWorldMatrix();

	XMMATRIX matProj = XMLoadFloat4x4(&proj);
	XMMATRIX matView = XMLoadFloat4x4(&view);
	XMMATRIX matWorld = XMLoadFloat4x4(&world);

	XMMATRIX invProj = XMMatrixInverse(nullptr, matProj);
	XMMATRIX invView = XMMatrixInverse(nullptr, matView);
	XMMATRIX invWorld = XMMatrixInverse(nullptr, matWorld);

	XMVECTOR nearPoint = XMVectorSet(px, py, 0.f, 1.f);
	nearPoint = XMVector3TransformCoord(nearPoint, invProj);
	nearPoint = XMVector3TransformCoord(nearPoint, invView);

	XMVECTOR farPoint = XMVectorSet(px, py, 1.f, 1.f);
	farPoint = XMVector3TransformCoord(farPoint, invProj);
	farPoint = XMVector3TransformCoord(farPoint, invView);

	XMVECTOR rayOrigin = nearPoint;
	XMVECTOR rayDir = XMVector3Normalize(farPoint - nearPoint);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invWorld);
	rayDir = XMVector3TransformNormal(rayDir, invWorld);
	rayDir = XMVector3Normalize(rayDir);

	auto& indices = m_pVIBufferCom->GetIndices();
	auto& vertices = m_pVIBufferCom->Getvertices();

	if (indices == nullptr || vertices == nullptr)
		return false;

	if (indices->empty() || vertices->empty())
		return false;

	float nearestDist = FLT_MAX;
	bool bHit = false;

	XMVECTOR nearestHitLocal = XMVectorZero();

	for (size_t i = 0; i + 2 < indices->size(); i += 3)
	{
		uint32_t i0 = (*indices)[i];
		uint32_t i1 = (*indices)[i + 1];
		uint32_t i2 = (*indices)[i + 2];

		if (i0 >= vertices->size() ||
			i1 >= vertices->size() ||
			i2 >= vertices->size())
			continue;

		XMVECTOR v0 = XMLoadFloat3(&(*vertices)[i0].vPosition);
		XMVECTOR v1 = XMLoadFloat3(&(*vertices)[i1].vPosition);
		XMVECTOR v2 = XMLoadFloat3(&(*vertices)[i2].vPosition);

		float dist = 0.f;

		if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
		{
			if (dist < nearestDist)
			{
				nearestDist = dist;
				nearestHitLocal = rayOrigin + rayDir * dist;
				bHit = true;
			}
		}
	}

	if (!bHit)
		return false;

	float x = XMVectorGetX(nearestHitLocal);
	float z = XMVectorGetZ(nearestHitLocal);

	float terrainSizeX = static_cast<float>(m_pVIBufferCom->GetNumVerticesX() - 1);
	float terrainSizeZ = static_cast<float>(m_pVIBufferCom->GetNumVerticesZ() - 1);

	if (x < 0.f || z < 0.f || x > terrainSizeX || z > terrainSizeZ)
		return false;

	// Paint용이니까 Local 좌표 그대로 반환
	XMStoreFloat3(&vPickingPoint, nearestHitLocal);

	return true;

}
void Terrain::Paint_Splat(_float3 vLocalPos)
{
	float terrainSizeX = (float)(m_pVIBufferCom->GetNumVerticesX() - 1);
	float terrainSizeZ = (float)(m_pVIBufferCom->GetNumVerticesZ() - 1);

	if (vLocalPos.x < 0.f || vLocalPos.z < 0.f ||
		vLocalPos.x > terrainSizeX || vLocalPos.z > terrainSizeZ)
		return;

	int centerX = (int)(vLocalPos.x / terrainSizeX * (m_iSplatWidth - 1));
	int centerY = (int)(vLocalPos.z / terrainSizeZ * (m_iSplatHeight - 1));

	centerX = std::clamp(centerX, 0, (int)m_iSplatWidth - 1);
	centerY = std::clamp(centerY, 0, (int)m_iSplatHeight - 1);

#if _DEBUG
	m_bHasSplatDebug = true;

	m_vDebugPickLocal = vLocalPos;

	m_iDebugSplatX = centerX;
	m_iDebugSplatY = centerY;

	m_fDebugReconLocalX =
		(float)centerX / (float)(m_iSplatWidth - 1) * terrainSizeX;

	m_fDebugReconLocalZ =
		(float)centerY / (float)(m_iSplatHeight - 1) * terrainSizeZ;

	m_fDebugErrorX = m_fDebugReconLocalX - vLocalPos.x;
	m_fDebugErrorZ = m_fDebugReconLocalZ - vLocalPos.z;
#endif

	int radius = (int)m_fBrushRadius;

	for (int y = centerY - radius; y <= centerY + radius; ++y)
	{
		for (int x = centerX - radius; x <= centerX + radius; ++x)
		{
			if (x < 0 || y < 0 || x >= (int)m_iSplatWidth || y >= (int)m_iSplatHeight)
				continue;

			float dx = (float)(x - centerX);
			float dy = (float)(y - centerY);
			float dist = sqrtf(dx * dx + dy * dy);

			if (dist > m_fBrushRadius)
				continue;

			float falloff = 1.f - dist / m_fBrushRadius;
			float power = m_fBrushPower * falloff;
			power = std::clamp(power, 0.f, 1.f);

			_float4& pixel = m_SplatPixels[y * m_iSplatWidth + x];

			float r = pixel.x;
			float g = pixel.y;
			float b = pixel.z;

			float targetR = (m_iPaintChannel == 0) ? 1.f : 0.f;
			float targetG = (m_iPaintChannel == 1) ? 1.f : 0.f;
			float targetB = (m_iPaintChannel == 2) ? 1.f : 0.f;

			r = r * (1.f - power) + targetR * power;
			g = g * (1.f - power) + targetG * power;
			b = b * (1.f - power) + targetB * power;

			r = std::clamp(r, 0.f, 1.f);
			g = std::clamp(g, 0.f, 1.f);
			b = std::clamp(b, 0.f, 1.f);

			float total = (std::max)(r + g + b, 0.0001f);

			pixel.x = r / total;
			pixel.y = g / total;
			pixel.z = b / total;
			pixel.w = 1.f;
		}
	}

	Update_SplatTexture();
}
_bool Terrain::Picking_Terrain_ForNavMesh(_float3& vPickingPoint)
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	float mouseX = static_cast<float>(pt.x);
	float mouseY = static_cast<float>(pt.y);

	float width = CGameInstance::Get().Get_ViewportSize().x;
	float height = CGameInstance::Get().Get_ViewportSize().y;

	if (width <= 0.f || height <= 0.f)
		return false;

	float px = mouseX / (width * 0.5f) - 1.0f;
	float py = mouseY / -(height * 0.5f) + 1.0f;

	_float4x4 proj, view, world;
	CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);
	world = m_pTransformCom->GetWorldMatrix();

	XMMATRIX matProj = XMLoadFloat4x4(&proj);
	XMMATRIX matView = XMLoadFloat4x4(&view);
	XMMATRIX matWorld = XMLoadFloat4x4(&world);

	XMMATRIX invProj = XMMatrixInverse(nullptr, matProj);
	XMMATRIX invView = XMMatrixInverse(nullptr, matView);
	XMMATRIX invWorld = XMMatrixInverse(nullptr, matWorld);

	XMVECTOR nearPoint = XMVectorSet(px, py, 0.f, 1.f);
	nearPoint = XMVector3TransformCoord(nearPoint, invProj);
	nearPoint = XMVector3TransformCoord(nearPoint, invView);

	XMVECTOR farPoint = XMVectorSet(px, py, 1.f, 1.f);
	farPoint = XMVector3TransformCoord(farPoint, invProj);
	farPoint = XMVector3TransformCoord(farPoint, invView);

	XMVECTOR rayOrigin = nearPoint;
	XMVECTOR rayDir = XMVector3Normalize(farPoint - nearPoint);

	// World Ray -> Terrain Local Ray
	rayOrigin = XMVector3TransformCoord(rayOrigin, invWorld);
	rayDir = XMVector3TransformNormal(rayDir, invWorld);
	rayDir = XMVector3Normalize(rayDir);

	auto& indices = m_pVIBufferCom->GetIndices();
	auto& vertices = m_pVIBufferCom->Getvertices();

	float nearestDist = FLT_MAX;
	bool bHit = false;

	XMVECTOR nearestHitLocal = XMVectorZero();

	// NavMesh 찍을 때는 전체 Terrain 검사
	for (size_t i = 0; i < indices->size(); i += 3)
	{
		XMVECTOR v0 = XMLoadFloat3(&(*vertices)[(*indices)[i]].vPosition);
		XMVECTOR v1 = XMLoadFloat3(&(*vertices)[(*indices)[i + 1]].vPosition);
		XMVECTOR v2 = XMLoadFloat3(&(*vertices)[(*indices)[i + 2]].vPosition);

		float dist = 0.f;

		if (TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, dist))
		{
			if (dist < nearestDist)
			{
				nearestDist = dist;
				nearestHitLocal = rayOrigin + rayDir * dist;
				bHit = true;
			}
		}
	}

	if (!bHit)
		return false;

	float x = XMVectorGetX(nearestHitLocal);
	float z = XMVectorGetZ(nearestHitLocal);

	float terrainSizeX = static_cast<float>(m_pVIBufferCom->GetNumVerticesX() - 1);
	float terrainSizeZ = static_cast<float>(m_pVIBufferCom->GetNumVerticesZ() - 1);

	if (x < 0.f || z < 0.f || x > terrainSizeX || z > terrainSizeZ)
		return false;

	// Terrain Local Hit -> World Hit
	XMVECTOR nearestHitWorld = XMVector3TransformCoord(nearestHitLocal, matWorld);

	XMStoreFloat3(&vPickingPoint, nearestHitWorld);

	return true;
}
void Terrain::Update_SplatTexture()
{
	D3D11_MAPPED_SUBRESOURCE mapped{};

	if (SUCCEEDED(m_pContext->Map(m_pSplatTexture2D.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mapped)))
	{
		for (uint32_t y = 0; y < m_iSplatHeight; ++y)
		{
			memcpy((uint8_t*)mapped.pData + y * mapped.RowPitch,m_SplatPixels.data() + y * m_iSplatWidth,m_iSplatWidth * sizeof(_float4));
		}

		m_pContext->Unmap(m_pSplatTexture2D.Get(), 0);
	}
}
#endif
unique_ptr<Terrain> Terrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Terrain>(new Terrain(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Terrain");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Terrain::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Terrain(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Terrain");
		return nullptr;
	}

	return pInstance;
}

