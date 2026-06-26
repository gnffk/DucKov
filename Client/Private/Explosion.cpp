#include "Explosion.h"

#include "GameInstance.h"

Explosion::Explosion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: PartObject{ pDevice, pContext }

{
}

Explosion::Explosion(const Explosion& Prototype)
	: PartObject{ Prototype }
{
}

Explosion::~Explosion()
{

}


HRESULT Explosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Explosion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/*m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(64.5f, 20.f, 64.5f, 1.f));*/

	return S_OK;
}

void Explosion::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Explosion::Update(_float fTimeDelta)
{
	Make_CombinedWorldMatrix(
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())
	);

	m_fAnimTime += fTimeDelta;

	m_iFrameIndex = static_cast<int32_t>(m_fAnimTime * m_fAnimFPS);

	if (m_bLoop)
	{
		m_iFrameIndex %= m_iFrameCount;
	}
	else
	{
		if (m_iFrameIndex >= m_iFrameCount)
		{
			m_iFrameIndex = m_iFrameCount - 1;

		
			return;
		}
	}

	m_pVIBufferCom->Smoke_Stay(fTimeDelta);

	__super::Update(fTimeDelta);
}

void Explosion::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(Explosion));

	__super::Late_Update(fTimeDelta);
}

HRESULT Explosion::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 proj, view, Trans;
	CGameInstance::Get().Get_MainCamerwaViewMatrix(view);
	CGameInstance::Get().Get_MainCamerwaProjectionMatrix(proj);
	CGameInstance::Get().GetWorldMatrix(Trans);
	_float4 vCamPos{};
	CGameInstance::Get().Get_MainCameraPosition(vCamPos);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition",&vCamPos,sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameIndex", &m_iFrameIndex, sizeof(int32_t))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameCol", &m_iFrameCol, sizeof(int32_t))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iFrameRow", &m_iFrameRow, sizeof(int32_t))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &view)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &proj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT Explosion::Ready_Components()
{
	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Particle_Point>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_VIBuffer_Particle_Explosion")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Shader_VtxInstance_Particle_Point")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), TEXT("Prototype_Component_Texture_Explosion")));
	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

unique_ptr<Explosion> Explosion::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Explosion>(new Explosion(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Explosion");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Explosion::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Explosion(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Explosion");
		return nullptr;
	}

	return pInstance;
}

