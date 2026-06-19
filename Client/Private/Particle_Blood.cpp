#include "Particle_Blood.h"

#include "GameInstance.h"

Particle_Blood::Particle_Blood(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : GameObject{ pDevice, pContext }
{
}

Particle_Blood::Particle_Blood(const Particle_Blood& Prototype): GameObject{ Prototype }
{
}

Particle_Blood::~Particle_Blood()
{

}


HRESULT Particle_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Particle_Blood::Initialize(void* pArg)
{
	PARTICLE_BLOOD_DESC* pDesc = static_cast<PARTICLE_BLOOD_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pDesc != nullptr && m_pTransformCom != nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION,XMVectorSet(pDesc->vSpawnPos.x,pDesc->vSpawnPos.y,pDesc->vSpawnPos.z,1.f));
	}

	Reset_Blood();

	return S_OK;
}

void Particle_Blood::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Particle_Blood::Update(_float fTimeDelta)
{


	m_pVIBufferCom->Spray(m_vVelocity,m_vStartPosition,fTimeDelta);


	__super::Update(fTimeDelta);
}

void Particle_Blood::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(Particle_Blood));

	__super::Late_Update(fTimeDelta);
}

HRESULT Particle_Blood::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);


	_float4 vBloodColor = { 0.65f, 0.02f, 0.01f, 1.f };
	_float  fAlphaPower = 1.5f;
	_float  fDarkPower = 0.55f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloodColor",&vBloodColor,sizeof(_float4))))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaPower",&fAlphaPower,sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDarkPower",&fDarkPower,sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
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

HRESULT Particle_Blood::Ready_Components()
{
	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Particle_Rect>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), 
		TEXT("Prototype_Com_VIBuffer_Particle_Blood")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(), 
		TEXT("Prototype_Com_Shader_Blood_Particle_Rect")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Texture_Blood")));
	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


void Particle_Blood::Reset_Blood()
{
	if (m_pVIBufferCom == nullptr)
		return;

	m_pVIBufferCom->Reset_Spray(m_vVelocity,m_vStartPosition);
}

unique_ptr<Particle_Blood> Particle_Blood::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Particle_Blood>(new Particle_Blood(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Particle_Blood");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Particle_Blood::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Particle_Blood(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Particle_Blood");
		return nullptr;
	}

	return pInstance;
}

