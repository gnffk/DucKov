#include "Particle_Spark.h"

#include "GameInstance.h"

Particle_Spark::Particle_Spark(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext) : Particle_System{ pDevice, pContext }
{
}

Particle_Spark::Particle_Spark(const Particle_Spark& Prototype) : Particle_System{ Prototype }
{
}

Particle_Spark::~Particle_Spark()
{

}


HRESULT Particle_Spark::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Particle_Spark::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_pTransformCom != nullptr)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	return S_OK;
}

void Particle_Spark::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void Particle_Spark::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_pVIBufferCom != nullptr)
		m_pVIBufferCom->Update_Spray(fTimeDelta);

	__super::Update(fTimeDelta);
}

void Particle_Spark::Late_Update(_float fTimeDelta)
{

	CGameInstance::Get().Add_RenderObject(RENDERGROUP::NONBLEND, SHARED_THIS(Particle_Spark));

	__super::Late_Update(fTimeDelta);
}

void Particle_Spark::Add_Particle(const PARTICLE_SPAWN_DESC& Desc)
{
	if (m_pVIBufferCom == nullptr)
		return;


	m_pVIBufferCom->Add_Spray(Desc.vSpawnPos, Desc.iCount);
}

HRESULT Particle_Spark::Render()
{
    if (m_pTransformCom == nullptr)
        return E_FAIL;

    if (m_pShaderCom == nullptr)
        return E_FAIL;

    if (m_pVIBufferCom == nullptr)
        return E_FAIL;

    if (m_pTextureCom == nullptr)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 View, Proj;
    CGameInstance::Get().Get_MainCameraMatrix(View, Proj);

    _matrix matView = XMLoadFloat4x4(&View);
    _matrix matViewInv = XMMatrixInverse(nullptr, matView);

    _float4x4 ViewInv{};
    XMStoreFloat4x4(&ViewInv, matViewInv);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewInvMatrix", &ViewInv)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &View)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &Proj)))
        return E_FAIL;

    _float4 vSparkColor = { 1.f, 0.94f, 0.f, 1.f };

    // Bloom °­µµ
    _float fIntensity = 10.f;
    _float fAlphaClip = 0.05f;
    _float fFadeInPower = 8.f;
    _float fFadeOutPower = 1.0f;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vSparkColor", &vSparkColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fIntensity", &fIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaClip", &fAlphaClip, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeInPower", &fFadeInPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeOutPower", &fFadeOutPower, sizeof(_float))))
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
HRESULT Particle_Spark::Ready_Components()
{

	m_pVIBufferCom = dynamic_pointer_cast<VIBuffer_Particle_Rect>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_VIBuffer_Particle_Spark")));
	if (FAILED(__super::Add_Component(TEXT("Com_VIBuffer"), m_pVIBufferCom)))
		return E_FAIL;

	m_pShaderCom = dynamic_pointer_cast<Shader>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Shader_Spark_Particle_Rect")));
	if (FAILED(__super::Add_Component(TEXT("Com_Shader"), m_pShaderCom)))
		return E_FAIL;

	m_pTextureCom = dynamic_pointer_cast<Texture>(CGameInstance::Get().Clone_Prototype(CGameInstance::Get().Get_Level(),
		TEXT("Prototype_Com_Texture_Spark")));
	if (FAILED(__super::Add_Component(TEXT("Com_Texture"), m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}



unique_ptr<Particle_Spark> Particle_Spark::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = unique_ptr<Particle_Spark>(new Particle_Spark(pDevice, pContext));

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Particle_Spark");
		return nullptr;
	}

	return pInstance;
}


shared_ptr<Prototype> Particle_Spark::Clone(void* pArg)
{
	auto	pInstance = shared_ptr<GameObject>(new Particle_Spark(*this));

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Particle_Spark");
		return nullptr;
	}

	return pInstance;
}

