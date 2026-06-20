#include "Particle_Blood.h"

#include "GameInstance.h"

Particle_Blood::Particle_Blood(ComPtr<ID3D11Device> pDevice,ComPtr<ID3D11DeviceContext> pContext): Particle_System{ pDevice, pContext }
{
}

Particle_Blood::Particle_Blood(const Particle_Blood& Prototype): Particle_System{ Prototype }
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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	if (m_pTransformCom != nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION,XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	return S_OK;
}

void Particle_Blood::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Particle_Blood::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_pVIBufferCom != nullptr)
		m_pVIBufferCom->Update_Spray(fTimeDelta);

	__super::Update(fTimeDelta);
}

void Particle_Blood::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONLIGHT, SHARED_THIS(Particle_Blood));

	__super::Late_Update(fTimeDelta);
}

void Particle_Blood::Add_Particle(const PARTICLE_SPAWN_DESC& Desc)
{
	if (m_pVIBufferCom == nullptr)
		return;


	m_pVIBufferCom->Add_Spray(Desc.vSpawnPos, Desc.iCount);
}

HRESULT Particle_Blood::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;



	_float4x4 View, Proj;
	CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

	_matrix matView = XMLoadFloat4x4(&View);
	_matrix matViewInv = XMMatrixInverse(nullptr, matView);

	_float4x4 ViewInv;
	XMStoreFloat4x4(&ViewInv, matViewInv);



	_float4 vBloodColor = { 0.65f, 0.02f, 0.01f, 1.f };
	_float  fAlphaPower = 1.5f;
	_float  fDarkPower = 0.55f;

	_float2 vNoiseSpeed = { 0.25f, 0.12f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNoiseSpeed", &vNoiseSpeed, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloodColor",&vBloodColor,sizeof(_float4))))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaPower",&fAlphaPower,sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDarkPower",&fDarkPower,sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewInvMatrix", &ViewInv)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pNormalTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
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

	m_pNoiseTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Texture_Noise")));
	if (FAILED(__super::Add_Component(TEXT("Com_NoiseTexture"), m_pNoiseTextureCom)))
		return E_FAIL;

	m_pMaskTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Texture_Mask")));
	if (FAILED(__super::Add_Component(TEXT("Com_MaskTexture"), m_pMaskTextureCom)))
		return E_FAIL;

	m_pNormalTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Texture_BloodN")));
	if (FAILED(__super::Add_Component(TEXT("Com_NormalTexture"), m_pNormalTextureCom)))
		return E_FAIL;

	return S_OK;
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

