#include "Engine_Macro.h"
#include "Bullet.h"
#include "GameInstance.h"
#include "AABB_Collider.h"
#include "OBB_Collider.h"
#include "Sphere_Collider.h"
Bullet::Bullet(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }

{
}

Bullet::Bullet(const Bullet& Prototype)
	: GameObject{ Prototype }
{
}

Bullet::~Bullet()
{
	//a.clear();
}


HRESULT Bullet::Initialize_Prototype()
{

	return S_OK;
}

HRESULT Bullet::Initialize(void* pArg)
{
	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc != nullptr)
	{
		m_vDir = pDesc->vDir;
		m_fSpeed = pDesc->fSpeed;

		_matrix matWorld =
			XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			XMMatrixTranslation(pDesc->vStartPos.x,pDesc->vStartPos.y,pDesc->vStartPos.z);

		_float4x4 World{};
		XMStoreFloat4x4(&World, matWorld);

		m_pTransformCom->Set_WorldMatrix(World);
	}
	m_fLifeTimer = 0.f;
	m_fLifeTime = 5.f;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Trail()))
		return E_FAIL;

	return S_OK;
}

void Bullet::Priority_Update(_float fTimeDelta)
{
	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(Bullet));


	for (int type = 0; type < (int)COLLIDER::COLLIDER_END; ++type)
	{
		auto& colliderList = m_pColliderComs[type];

		for (size_t i = 0; i < colliderList.size(); ++i)
		{

			CGameInstance::Get().Add_Collider(colliderList[i]->Get_GroupTag(), colliderList[i].get());

		}
	}



}

void Bullet::Update(_float fTimeDelta)
{
	m_fLifeTimer += fTimeDelta;

	if (m_fLifeTimer >= m_fLifeTime)
	{
		Set_Dead();
		return;
	}
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vDir = XMLoadFloat3(&m_vDir);

	vDir = XMVector3Normalize(vDir);

	vPos += vDir * m_fSpeed * fTimeDelta;

	// 기존 TrailPoint 수명 증가
	for (auto& point : m_TrailPoints)
		point.fLife += fTimeDelta;

	// 오래된 점 제거
	m_TrailPoints.erase(
		remove_if(
			m_TrailPoints.begin(),
			m_TrailPoints.end(),
			[this](const TrailPoint& p)
			{
				return p.fLife > m_fTrailLife;
			}
		),
		m_TrailPoints.end()
	);


	_float3 vPosition{};
	XMStoreFloat3(&vPosition, vPos);

	// 너무 촘촘하게 쌓이는 것 방지
	bool bAddPoint = true;

	if (!m_TrailPoints.empty())
	{
		_vector vLast = XMLoadFloat3(&m_TrailPoints.back().vPos);
		_vector vCurr = XMLoadFloat3(&vPosition);

		float fDist = XMVectorGetX(XMVector3Length(vCurr - vLast));

		if (fDist < 0.03f)
			bAddPoint = false;
	}

	if (bAddPoint)
	{
		m_TrailPoints.push_back({ vPosition, 0.f });

		if (m_TrailPoints.size() > MAX_TRAIL_POINTS)
			m_TrailPoints.erase(m_TrailPoints.begin());
	}

	Build_TrailMesh();

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void Bullet::Late_Update(_float fTimeDelta)
{

}

HRESULT Bullet::Render()
{

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 World = m_pTransformCom->GetWorldMatrix();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &World)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (uint32_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom,"g_DiffuseTexture",i,(uint32_t)ETOUI(TEXTURETYPE::DIFFUSE),0)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	// 총알 모델 렌더 후 Trail 렌더
	if (FAILED(Render_Trail()))
		return E_FAIL;

	return S_OK;
}
HRESULT Bullet::Render_Trail()
{
	if (m_TrailVertices.size() < 4)
		return S_OK;

	if (m_pTrailVB == nullptr || m_pTrailShaderCom == nullptr)
		return S_OK;

	D3D11_MAPPED_SUBRESOURCE mapped{};

	if (FAILED(m_pContext->Map(m_pTrailVB.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mapped)))
	{
		return E_FAIL;
	}

	memcpy(mapped.pData,m_TrailVertices.data(),sizeof(VTXBULLETTRAIL) * m_TrailVertices.size());

	m_pContext->Unmap(m_pTrailVB.Get(), 0);

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_float4x4 Identity{};
	XMStoreFloat4x4(&Identity, XMMatrixIdentity());

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_WorldMatrix", &Identity)))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;


	if (FAILED(m_pTrailTextureCom->Bind_ShaderResource(m_pTrailShaderCom,"g_DiffuseTexture",0)))
	{
		return E_FAIL;
	}

	UINT stride = sizeof(VTXBULLETTRAIL);
	UINT offset = 0;

	m_pContext->IASetVertexBuffers(0,1,m_pTrailVB.GetAddressOf(),&stride,&offset);

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	if (FAILED(m_pTrailShaderCom->Begin(0)))
		return E_FAIL;

	m_pContext->Draw(static_cast<UINT>(m_TrailVertices.size()), 0);

	return S_OK;
}
HRESULT Bullet::Ready_Components()
{
	__super::Clear_Compnent();


	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Model_Bullet")));
	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;


	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Com_Shader_Vtx_FBX_Tex")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	auto pOBBCom =dynamic_pointer_cast<BaseCollider>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_OBB_Collider")));

	pOBBCom->Set_Tag(COLLIDER::COLLIDER_OBB);
	pOBBCom->Set_GroupTag(L"Bullet");
	pOBBCom->Set_Extend(_float3{ 0.1f,0.1f,0.1f });

	if (FAILED(__super::Add_Component(TEXT("Com_Collider"), pOBBCom)))
		return E_FAIL;

	pOBBCom->SetOwner(SHARED_THIS(Bullet).get());

	m_pColliderComs[(int)COLLIDER::COLLIDER_OBB].push_back(pOBBCom);

	return S_OK;
}

HRESULT Bullet::Ready_Trail()
{
	m_pTrailShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_BulletTrail")));

	if (m_pTrailShaderCom == nullptr)
		return E_FAIL;

	// Trail Texture 추가
	m_pTrailTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Texture_BulletTrail")));

	if (m_pTrailTextureCom == nullptr)
		return E_FAIL;


	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(VTXBULLETTRAIL) * MAX_TRAIL_VERTICES;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(VTXBULLETTRAIL);

	if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, m_pTrailVB.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

void Bullet::Build_TrailMesh()
{
	m_TrailVertices.clear();

	if (m_TrailPoints.size() < 2)
		return;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_matrix matView = XMLoadFloat4x4(&View);
	_matrix matInvView = XMMatrixInverse(nullptr, matView);

	_vector vCamPos = matInvView.r[3];

	const size_t iPointCount = m_TrailPoints.size();

	for (size_t i = 0; i < iPointCount; ++i)
	{
		_vector vCurr = XMLoadFloat3(&m_TrailPoints[i].vPos);

		_vector vDir{};

		if (i == 0)
		{
			_vector vNext = XMLoadFloat3(&m_TrailPoints[i + 1].vPos);
			vDir = vNext - vCurr;
		}
		else if (i == iPointCount - 1)
		{
			_vector vPrev = XMLoadFloat3(&m_TrailPoints[i - 1].vPos);
			vDir = vCurr - vPrev;
		}
		else
		{
			_vector vPrev = XMLoadFloat3(&m_TrailPoints[i - 1].vPos);
			_vector vNext = XMLoadFloat3(&m_TrailPoints[i + 1].vPos);
			vDir = vNext - vPrev;
		}

		vDir = XMVector3Normalize(vDir);

		_vector vToCam = XMVector3Normalize(vCamPos - vCurr);

		// 카메라 기준으로 Trail의 좌우 방향
		_vector vSide = XMVector3Cross(vToCam, vDir);
		vSide = XMVector3Normalize(vSide);

		float fAlpha = 1.f - (m_TrailPoints[i].fLife / m_fTrailLife);

		if (fAlpha < 0.f)
			fAlpha = 0.f;

		float fWidth = m_fTrailWidth * fAlpha;

		_vector vLeft = vCurr + vSide * fWidth * 0.5f;
		_vector vRight = vCurr - vSide * fWidth * 0.5f;

		_float3 vLeftPos{};
		_float3 vRightPos{};

		XMStoreFloat3(&vLeftPos, vLeft);
		XMStoreFloat3(&vRightPos, vRight);

		float fV = static_cast<float>(i) / static_cast<float>(iPointCount - 1);

		_float4 vColor = { 1.f, 1.f, 0.05f, fAlpha };

		m_TrailVertices.push_back(VTXBULLETTRAIL{ vLeftPos, _float2{ 0.f, fV }, vColor });

		m_TrailVertices.push_back(VTXBULLETTRAIL{ vRightPos, _float2{ 1.f, fV }, vColor });
	}
}

unique_ptr<Bullet> Bullet::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Bullet>(new Bullet(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bullet");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Bullet::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Bullet(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Bullet");
		return nullptr;
	}

	return pInstance;
}
