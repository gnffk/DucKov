#include "Cloud.h"
#include "GameInstance.h"
#include "Model.h"
#include "Shader.h"

NS_BEGIN(Client)

Cloud::Cloud(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: GameObject{ pDevice, pContext }
{
}

Cloud::Cloud(const Cloud& Prototype)
	: GameObject{ Prototype }
{
}

Cloud::~Cloud()
{
}

HRESULT Cloud::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Cloud::Initialize(void* pArg)
{
	CLOUD_DESC* pDesc = static_cast<CLOUD_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pDesc != nullptr)
	{
		m_vStartPos = pDesc->vSpawnPos;
		m_fMoveSpeed = pDesc->fMoveSpeed;
		m_fResetDistance = pDesc->fResetDistance;

		if (m_pTransformCom != nullptr)
		{
			_vector vPos = XMVectorSet(
				m_vStartPos.x,
				m_vStartPos.y,
				m_vStartPos.z,
				1.f
			);

			m_pTransformCom->Set_State(STATE::POSITION, vPos);
		}

		m_ModelComponentName = pDesc->m_strPrototypeBaseName;
	}
	else
	{
		_float4 vCurPos{};
		XMStoreFloat4(&vCurPos, m_pTransformCom->Get_State(STATE::POSITION));

		m_vStartPos = {
			vCurPos.x,
			vCurPos.y,
			vCurPos.z
		};
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	float x = CGameInstance::Get().Random(10.f, 20.f);
	float y = CGameInstance::Get().Random(1.f, 1.f);
	float z = CGameInstance::Get().Random(10.f, 20.f);

	m_pTransformCom->Set_Scale(0.01f * x,0.01f * y,0.01f * z);
	return S_OK;
}

void Cloud::Priority_Update(_float fTimeDelta)
{
}

void Cloud::Update(_float fTimeDelta)
{
	if (m_pTransformCom == nullptr)
		return;

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

	// 월드 기준 왼쪽(-X)으로 이동
	_float fMove = m_fMoveSpeed * fTimeDelta;
	_float fMovey = m_fMoveSpeed * fTimeDelta / 2;

	vPos += XMVectorSet(-fMove, 0.f,-fMovey, 0.f);

	_float4 vCurrentPos{};
	XMStoreFloat4(&vCurrentPos, vPos);

	// 시작 위치에서 왼쪽으로 일정 거리 이상 이동하면 다시 시작 위치로 복귀
	if (vCurrentPos.x <= m_vStartPos.x - m_fResetDistance)
	{
		vPos = XMVectorSet(
			m_vStartPos.x,
			m_vStartPos.y,
			m_vStartPos.z,
			1.f
		);
	}

	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void Cloud::Late_Update(_float fTimeDelta)
{
	// 구름이 알파 블렌딩이면 BLEND로 넣는 게 좋음

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::SHADOW, SHARED_THIS(Cloud));


}

HRESULT Cloud::Render()
{
	if (m_pShaderCom == nullptr || m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 ViewMatrix{}, ProjMatrix{};

	if (FAILED(CGameInstance::Get().Get_MainCamerwaViewMatrix(ViewMatrix)))
		return E_FAIL;

	if (FAILED(CGameInstance::Get().Get_MainCamerwaProjectionMatrix(ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	for (uint32_t i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT Cloud::Render_Shadow()
{
	if (m_pShaderCom == nullptr || m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix(
		"g_ViewMatrix",
		CGameInstance::Get().Get_ShadowLightTransform(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix(
		"g_ProjMatrix",
		CGameInstance::Get().Get_ShadowLightTransform(D3DTS::PROJ))))
		return E_FAIL;

	for (uint32_t i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", (uint32_t)i, (uint32_t)ETOUI(TEXTURETYPE::DIFFUSE), 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT Cloud::Ready_Components()
{
	// Shader
	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),TEXT("Prototype_Com_Shader_Obstacle")));

	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pModelCom = dynamic_pointer_cast<Model>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),m_ModelComponentName.c_str()));

	if (m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(__super::Add_Component(TEXT("Com_Model"), m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

unique_ptr<Cloud> Cloud::Create(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext
)
{
	auto pInstance = unique_ptr<Cloud>(new Cloud(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Cloud");
		return nullptr;
	}

	return pInstance;
}

shared_ptr<Prototype> Cloud::Clone(void* pArg)
{
	auto pInstance = shared_ptr<GameObject>(new Cloud(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Cloud");
		return nullptr;
	}

	return pInstance;
}

NS_END